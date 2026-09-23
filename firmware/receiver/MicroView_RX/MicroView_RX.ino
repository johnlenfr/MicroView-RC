/*
  NRF24L01 receiver v3.19a - EXTENDED ENDPOINTS + FWD THROTTLE + FULL OUTPUT MAP + BINDING + FAILSAFE
  Board: Arduino Uno

  IMPORTANT:
  RF24_config.h must contain:
      #define SOFTSPI
      const uint8_t SOFT_SPI_MOSI_PIN = 3;
      const uint8_t SOFT_SPI_MISO_PIN = 5;
      const uint8_t SOFT_SPI_SCK_PIN  = 6;

  WIRING - nRF24L01 -> Arduino Uno
      CE   -> D7
      CSN  -> D8
      MOSI -> D3
      MISO -> D5
      SCK  -> D6
      VCC  -> 3.3V
      GND  -> GND
      IRQ  -> not connected

  Receiver outputs:
      ROLL     (CH1) -> D9   servo
      PITCH    (CH2) -> D10  servo
      YAW      (CH3) -> A3   servo
      THROTTLE (CH4) -> D12  servo / ESC
      JR        (CH6) -> D2   digital output
      JL        (CH7) -> D4   digital output
      PB RIGHT  (CH8) -> A1   digital output
      PB LEFT   (CH9) -> A2   digital output
      POT       (CH0) -> D11  servo pulse OR hardware PWM 0..100%

  Model battery telemetry input:
      BAT+ --- 22k ---+--- A0
                      |
                     10k
                      |
                     GND
      Optional: 100 nF from A0 to GND.

  RF parameters validated on the bench:
      address = Rx001..Rx005 (stored model binding)
      channel = 76
      data rate = 250 kbps
      PA = LOW (bench test; can be raised later)

  Optional Serial debug: 115200 baud (RX_SERIAL_DEBUG = 1)
*/

#include <Arduino.h>
#include <DigitalIO.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <EEPROM.h>

#ifndef SOFTSPI
#error "Enable #define SOFTSPI in RF24_config.h before compiling this receiver."
#endif

#define RX_SERIAL_DEBUG 0
// 0 = normal scrolling output, compatible with Arduino Serial Monitor
// 1 = fixed table using ANSI cursor control (PuTTY / CoolTerm / TeraTerm)
#define RX_DEBUG_ANSI 0

// Set to a non-zero millivolt value to test ACK telemetry without a battery divider.
// Example: 7400 sends a fixed 7.400 V value to the transmitter.
#define RX_BATTERY_TEST_MV 0

// Final operational defaults:
// - fixed 3.800 V battery telemetry test value
// - serial debug enabled with ANSI fixed-table output
// - 1 s failsafe: neutral steering axes, BIDIR throttle neutral / FWD throttle STOP, AUX off
#define FAILSAFE_TIME 1000UL
#define FAILSAFE_POT 50
#define FAILSAFE_ROLL 127
#define FAILSAFE_PITCH 127
#define FAILSAFE_YAW 127
#define FAILSAFE_THROTTLE_BIDIR 127
#define FAILSAFE_AUX 0

// The v4.1u TX encodes an extended +/-125% servo range in ch1..ch4.
// byte 0..255 corresponds to 875..2125 us. 100% stick travel is still
// approximately 1000..2000 us. A FWD-ONLY throttle uses the 1000 us code
// as STOP during failsafe instead of neutral 1500 us.
const uint8_t FWD_ONLY_STOP_CODE = 26;

#define CE_PIN  7
#define CSN_PIN 8

// Proportional outputs
#define ROLL_PIN      9
#define PITCH_PIN    10
#define YAW_PIN      A3
#define THROTTLE_PIN 12

// AUX / switch outputs
#define AUX_JR_PIN     2
#define AUX_JL_PIN     4
#define AUX_RIGHT_PIN A1
#define AUX_LEFT_PIN  A2

// POT output (CH0)
#define POT_OUTPUT_PIN 11

// Select ONE POT output mode here:
#define POT_MODE_SERVO  0
#define POT_MODE_ANALOG 1
#define POT_OUTPUT_MODE POT_MODE_SERVO

// POT_MODE_SERVO:
//   D11 outputs a standard RC servo pulse, 1000..2000 us for ch0 = 0..100%.
// POT_MODE_ANALOG:
//   D11 uses hardware PWM (analogWrite) with a 0..100% duty cycle. The
//   ATmega328P has no DAC, so add an external RC low-pass filter if a smooth
//   DC voltage (approximately 0..5 V) is required. D11 is Timer2 PWM and
//   remains available because Servo.h uses Timer1 on the Uno/Nano.

#define MODEL_BATTERY_PIN A0

const uint16_t SERVO_MIN_US = 875;
const uint16_t SERVO_MAX_US = 2125;
const uint16_t POT_SERVO_MIN_US = 1000;
const uint16_t POT_SERVO_MAX_US = 2000;

const uint8_t RF_CHANNEL = 76;
const uint8_t MODEL_COUNT = 5;
const uint8_t BIND_MAGIC0 = 0xB7;
const uint8_t BIND_MAGIC1 = 0x42;
const unsigned long BIND_WINDOW_MS = 30000UL;
const int EEPROM_BIND_MAGIC0 = 0;
const int EEPROM_BIND_MAGIC1 = 1;
const int EEPROM_BIND_MODEL  = 2;

RF24 radio(CE_PIN, CSN_PIN);

uint8_t boundModel = 0;
bool bindWindowActive = true;
unsigned long bindWindowStart = 0;

void makeModelAddress(uint8_t modelIndex, byte address[5]) {
  address[0] = 'R';
  address[1] = 'x';
  address[2] = '0';
  address[3] = '0';
  address[4] = (byte)('1' + modelIndex);
}

void saveBoundModel(uint8_t modelIndex) {
  EEPROM.update(EEPROM_BIND_MAGIC0, BIND_MAGIC0);
  EEPROM.update(EEPROM_BIND_MAGIC1, BIND_MAGIC1);
  EEPROM.update(EEPROM_BIND_MODEL, modelIndex);
}

void loadBoundModel() {
  if (EEPROM.read(EEPROM_BIND_MAGIC0) == BIND_MAGIC0 &&
      EEPROM.read(EEPROM_BIND_MAGIC1) == BIND_MAGIC1 &&
      EEPROM.read(EEPROM_BIND_MODEL) < MODEL_COUNT) {
    boundModel = EEPROM.read(EEPROM_BIND_MODEL);
  } else {
    // Backward-compatible first boot: a freshly flashed RX starts on MODEL01.
    boundModel = 0;
    saveBoundModel(boundModel);
  }
}

void openBoundModelPipe() {
  byte address[5];
  makeModelAddress(boundModel, address);
  radio.openReadingPipe(1, address);
}

struct Data_Package {
  byte ch0; // potentiometer 0..100
  byte ch1; // ROLL 0..255
  byte ch2; // PITCH 0..255
  byte ch3; // YAW 0..255
  byte ch4; // THROTTLE 0..255
  byte ch5; // TX battery in 0.1 V
  byte ch6; // JR
  byte ch7; // JL
  byte ch8; // PB RIGHT
  byte ch9; // PB LEFT
};

struct Telemetry_Package {
  uint16_t batteryMv;
};

Data_Package Data;
Telemetry_Package telemetry;

Servo servoRoll;
Servo servoPitch;
Servo servoYaw;
Servo servoThrottle;
#if POT_OUTPUT_MODE == POT_MODE_SERVO
Servo servoPot;
#endif

uint16_t rollUs = 1500;
uint16_t pitchUs = 1500;
uint16_t yawUs = 1500;
uint16_t throttleUs = 1500;
uint16_t potUs = 1500;
byte appliedCh0 = FAILSAFE_POT;
uint8_t potPwmDuty = FAILSAFE_POT;
byte appliedCh1 = 127;
byte appliedCh2 = 127;
byte appliedCh3 = 127;
byte appliedCh4 = 127;
bool throttleForwardOnly = false;
byte appliedCh6 = 0;
byte appliedCh7 = 0;
byte appliedCh8 = 0;
byte appliedCh9 = 0;

unsigned long lastReceiveTime = 0;
unsigned long lastServoUpdate = 0;
unsigned long lastBatterySample = 0;
unsigned long lastDebugPrint = 0;
unsigned long rateWindowStart = 0;
uint32_t totalPackets = 0;
uint16_t windowPackets = 0;
uint16_t packetsPerSecond = 0;
bool everReceived = false;

const unsigned long SERVO_INTERVAL_MS = 20;
const unsigned long BATTERY_SAMPLE_INTERVAL_MS = 100;
const unsigned long DEBUG_INTERVAL_MS = 50;
const uint8_t CHANNEL_DEADBAND = 1;

const uint8_t BAT_R1_K = 22;
const uint8_t BAT_R2_K = 10;
const uint16_t ADC_REF_MV = 5000; // adjust if you later calibrate the Uno 5 V reference

void resetData() {
  Data.ch0 = FAILSAFE_POT;
  Data.ch1 = FAILSAFE_ROLL;
  Data.ch2 = FAILSAFE_PITCH;
  Data.ch3 = FAILSAFE_YAW;
  Data.ch4 = throttleForwardOnly ? FWD_ONLY_STOP_CODE : FAILSAFE_THROTTLE_BIDIR;
  Data.ch5 = 0;
  Data.ch6 = FAILSAFE_AUX;
  Data.ch7 = FAILSAFE_AUX;
  Data.ch8 = FAILSAFE_AUX;
  Data.ch9 = FAILSAFE_AUX;
}

bool channelChanged(byte value, byte applied) {
  const int16_t delta = (int16_t)value - (int16_t)applied;
  return (delta > CHANNEL_DEADBAND || delta < -CHANNEL_DEADBAND);
}

uint16_t readModelBatteryMv() {
#if RX_BATTERY_TEST_MV > 0
  return (uint16_t)RX_BATTERY_TEST_MV;
#else
  analogRead(MODEL_BATTERY_PIN); // discard first conversion after mux/reference activity
  uint16_t sum = 0;
  for (uint8_t i = 0; i < 4; ++i) sum += analogRead(MODEL_BATTERY_PIN);
  const uint16_t adc = (sum + 2U) / 4U;

  const uint32_t num = (uint32_t)adc * ADC_REF_MV * (BAT_R1_K + BAT_R2_K);
  return (uint16_t)((num + (1023UL * BAT_R2_K) / 2UL) /
                    (1023UL * BAT_R2_K));
#endif
}


void applyPotOutput(bool force = false) {
  byte value = Data.ch0;
  if (value > 100) value = 100;
  if (!force && value == appliedCh0) return;

  appliedCh0 = value;
#if POT_OUTPUT_MODE == POT_MODE_SERVO
  potUs = (uint16_t)map(appliedCh0, 0, 100, POT_SERVO_MIN_US, POT_SERVO_MAX_US);
  servoPot.writeMicroseconds(potUs);
#else
  potPwmDuty = appliedCh0;
  analogWrite(POT_OUTPUT_PIN, (uint8_t)map(appliedCh0, 0, 100, 0, 255));
#endif
}

void updateOutputs() {
  applyPotOutput();

  if (channelChanged(Data.ch1, appliedCh1)) {
    appliedCh1 = Data.ch1;
    rollUs = (uint16_t)map(appliedCh1, 0, 255, SERVO_MIN_US, SERVO_MAX_US);
    servoRoll.writeMicroseconds(rollUs);
  }

  if (channelChanged(Data.ch2, appliedCh2)) {
    appliedCh2 = Data.ch2;
    pitchUs = (uint16_t)map(appliedCh2, 0, 255, SERVO_MIN_US, SERVO_MAX_US);
    servoPitch.writeMicroseconds(pitchUs);
  }

  if (channelChanged(Data.ch3, appliedCh3)) {
    appliedCh3 = Data.ch3;
    yawUs = (uint16_t)map(appliedCh3, 0, 255, SERVO_MIN_US, SERVO_MAX_US);
    servoYaw.writeMicroseconds(yawUs);
  }

  if (channelChanged(Data.ch4, appliedCh4)) {
    appliedCh4 = Data.ch4;
    throttleUs = (uint16_t)map(appliedCh4, 0, 255, SERVO_MIN_US, SERVO_MAX_US);
    servoThrottle.writeMicroseconds(throttleUs);
  }

  if (Data.ch6 != appliedCh6) {
    appliedCh6 = Data.ch6;
    digitalWrite(AUX_JR_PIN, appliedCh6 ? HIGH : LOW);
  }
  if (Data.ch7 != appliedCh7) {
    appliedCh7 = Data.ch7;
    digitalWrite(AUX_JL_PIN, appliedCh7 ? HIGH : LOW);
  }
  if (Data.ch8 != appliedCh8) {
    appliedCh8 = Data.ch8;
    digitalWrite(AUX_RIGHT_PIN, appliedCh8 ? HIGH : LOW);
  }
  if (Data.ch9 != appliedCh9) {
    appliedCh9 = Data.ch9;
    digitalWrite(AUX_LEFT_PIN, appliedCh9 ? HIGH : LOW);
  }
}

void printOnOff(byte v) {
  Serial.print(v ? F("ON ") : F("OFF"));
}

void printVoltage10(byte v10) {
  Serial.print(v10 / 10);
  Serial.print('.');
  Serial.print(v10 % 10);
  Serial.print('V');
}

void printVoltageMv(uint16_t mv) {
  Serial.print(mv / 1000U);
  Serial.print('.');
  uint16_t frac = mv % 1000U;
  if (frac < 100U) Serial.print('0');
  if (frac < 10U) Serial.print('0');
  Serial.print(frac);
  Serial.print('V');
}

void printDebugScrolling(unsigned long now) {
  Serial.print(F("RF="));
  if (!everReceived) Serial.print(F("WAIT"));
  else if ((unsigned long)(now - lastReceiveTime) <= FAILSAFE_TIME) Serial.print(F("OK"));
  else Serial.print(F("LOST"));

  Serial.print(F(" pps=")); Serial.print(packetsPerSecond);
  Serial.print(F(" total=")); Serial.print(totalPackets);
  Serial.print(F(" age="));
  if (everReceived) Serial.print(now - lastReceiveTime); else Serial.print(F("NA"));
  Serial.print(F("ms"));

  Serial.print(F(" | LG_DG/YAW(ch3)=")); Serial.print(Data.ch3);
  Serial.print(F(" LG_HB/THR(ch4)=")); Serial.print(Data.ch4);
  Serial.print(F(" RD_HB(ch2)=")); Serial.print(Data.ch2);
  Serial.print(F(" RD_DG(ch1)=")); Serial.print(Data.ch1);

  Serial.print(F(" | JL=")); Serial.print(Data.ch7);
  Serial.print(F(" JR=")); Serial.print(Data.ch6);
  Serial.print(F(" PB_L=")); Serial.print(Data.ch9);
  Serial.print(F(" PB_R=")); Serial.print(Data.ch8);

  Serial.print(F(" | POT=")); Serial.print(Data.ch0); Serial.print('%');
#if POT_OUTPUT_MODE == POT_MODE_SERVO
  Serial.print(F("/")); Serial.print(potUs); Serial.print(F("us"));
#else
  Serial.print(F(" PWM=")); Serial.print(potPwmDuty); Serial.print('%');
#endif
  Serial.print(F(" TXBAT=")); printVoltage10((byte)(Data.ch5 & 0x7F));
  Serial.print(F(" THR=")); Serial.print(throttleForwardOnly ? F("FWD") : F("BI"));
  Serial.print(F(" | RXBAT_ACK=")); printVoltageMv(telemetry.batteryMv);
  Serial.print(F(" | R=")); Serial.print(rollUs);
  Serial.print(F(" P=")); Serial.print(pitchUs);
  Serial.print(F(" Y=")); Serial.print(yawUs);
  Serial.print(F(" T=")); Serial.print(throttleUs); Serial.println(F("us"));
}

void printDebugAnsi(unsigned long now) {
  Serial.print(F("\033[2J\033[H"));
  Serial.println(F("+--------------------------------------------------------------+"));
  Serial.println(F("|       RC RX DEBUG - REAL 10-BYTE PACKET + ACK TELEMETRY     |"));
  Serial.println(F("+-------------------------------+------+-----------------------+"));
  Serial.println(F("| Command                       | Ch   | Received value        |"));
  Serial.println(F("+-------------------------------+------+-----------------------+"));

  Serial.print(F("| Left stick Right/Left (YAW)   | ch3  | ")); Serial.print(Data.ch3); Serial.println(F("                   |"));
  Serial.print(F("| Left stick Up/Down (THROTTLE) | ch4  | ")); Serial.print(Data.ch4); Serial.println(F("                   |"));
  Serial.print(F("| Right stick Up/Down           | ch2  | ")); Serial.print(Data.ch2); Serial.println(F("                   |"));
  Serial.print(F("| Right stick Right/Left        | ch1  | ")); Serial.print(Data.ch1); Serial.println(F("                   |"));

  Serial.print(F("| Joystick LEFT switch          | ch7  | ")); printOnOff(Data.ch7); Serial.println(F("                  |"));
  Serial.print(F("| Joystick RIGHT switch         | ch6  | ")); printOnOff(Data.ch6); Serial.println(F("                  |"));
  Serial.print(F("| Separate switch #1 LEFT       | ch9  | ")); printOnOff(Data.ch9); Serial.println(F("                  |"));
  Serial.print(F("| Separate switch #2 RIGHT      | ch8  | ")); printOnOff(Data.ch8); Serial.println(F("                  |"));

  Serial.print(F("| Potentiometer                 | ch0  | ")); Serial.print(Data.ch0); Serial.println(F(" %                 |"));
  Serial.print(F("| TX battery carried in packet  | ch5  | ")); printVoltage10((byte)(Data.ch5 & 0x7F)); Serial.println(F("                 |"));
  Serial.print(F("| RX battery ACK -> transmitter | ACK  | ")); printVoltageMv(telemetry.batteryMv); Serial.println(F("              |"));
  Serial.println(F("+-------------------------------+------+-----------------------+"));

  Serial.print(F("| RF link: "));
  if (!everReceived) Serial.print(F("WAITING"));
  else if ((unsigned long)(now - lastReceiveTime) <= FAILSAFE_TIME) Serial.print(F("OK     "));
  else Serial.print(F("LOST   "));
  Serial.print(F(" | packets/s: ")); Serial.print(packetsPerSecond);
  Serial.print(F(" | total: ")); Serial.print(totalPackets);
  Serial.println(F("        |"));
  Serial.print(F("| Last packet age: "));
  if (everReceived) Serial.print(now - lastReceiveTime); else Serial.print(F("N/A"));
  Serial.print(F(" ms | R:")); Serial.print(rollUs);
  Serial.print(F(" P:")); Serial.print(pitchUs);
  Serial.print(F(" Y:")); Serial.print(yawUs);
  Serial.print(F(" T:")); Serial.print(throttleUs);
#if POT_OUTPUT_MODE == POT_MODE_SERVO
  Serial.print(F(" POT:")); Serial.print(potUs); Serial.println(F(" us |"));
#else
  Serial.print(F(" POT:")); Serial.print(potPwmDuty); Serial.println(F("% PWM |"));
#endif
  Serial.println(F("+--------------------------------------------------------------+"));
}

void setup() {
  loadBoundModel();
#if RX_SERIAL_DEBUG
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println(F("RC RX v3.19a - ENDPOINTS + FWD THR + FULL OUTPUTS"));
  Serial.println(F("NRF: CE=D7 CSN=D8 MOSI=D3 MISO=D5 SCK=D6"));
  Serial.println(F("OUT: ROLL=D9 PITCH=D10 YAW=A3 THR=D12 | JR=D2 JL=D4 PB_R=A1 PB_L=A2"));
#if POT_OUTPUT_MODE == POT_MODE_SERVO
  Serial.println(F("POT: ch0 -> D11 SERVO 1000..2000 us | RX BAT=A0"));
#else
  Serial.println(F("POT: ch0 -> D11 HW PWM 0..100% (RC filter for DC) | RX BAT=A0"));
#endif
  Serial.print(F("RF: bound MODEL0")); Serial.print(boundModel + 1);
  Serial.println(F(" channel=76 rate=250kbps PA=LOW"));
  Serial.println(F("BIND: normal model pipe, 30 s after startup"));
#if RX_BATTERY_TEST_MV > 0
  Serial.print(F("RX BATTERY TELEMETRY TEST VALUE = "));
  Serial.print(RX_BATTERY_TEST_MV);
  Serial.println(F(" mV"));
#else
  Serial.println(F("RX BATTERY TELEMETRY = real A0 divider measurement"));
#endif
#endif

  resetData();
  telemetry.batteryMv = readModelBatteryMv();

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(RF_CHANNEL);
  radio.setAutoAck(true);
  radio.enableAckPayload();
  openBoundModelPipe();
  radio.writeAckPayload(1, &telemetry, sizeof(telemetry));
  radio.startListening();

  pinMode(AUX_JR_PIN, OUTPUT);
  pinMode(AUX_JL_PIN, OUTPUT);
  pinMode(AUX_RIGHT_PIN, OUTPUT);
  pinMode(AUX_LEFT_PIN, OUTPUT);
  digitalWrite(AUX_JR_PIN, LOW);
  digitalWrite(AUX_JL_PIN, LOW);
  digitalWrite(AUX_RIGHT_PIN, LOW);
  digitalWrite(AUX_LEFT_PIN, LOW);

  servoRoll.attach(ROLL_PIN, SERVO_MIN_US, SERVO_MAX_US);
  servoPitch.attach(PITCH_PIN, SERVO_MIN_US, SERVO_MAX_US);
  servoYaw.attach(YAW_PIN, SERVO_MIN_US, SERVO_MAX_US);
  servoThrottle.attach(THROTTLE_PIN, SERVO_MIN_US, SERVO_MAX_US);
#if POT_OUTPUT_MODE == POT_MODE_SERVO
  servoPot.attach(POT_OUTPUT_PIN, POT_SERVO_MIN_US, POT_SERVO_MAX_US);
#else
  pinMode(POT_OUTPUT_PIN, OUTPUT);
  analogWrite(POT_OUTPUT_PIN, 0);
#endif
  servoRoll.writeMicroseconds(rollUs);
  servoPitch.writeMicroseconds(pitchUs);
  servoYaw.writeMicroseconds(yawUs);
  servoThrottle.writeMicroseconds(throttleUs);
  applyPotOutput(true);

  const unsigned long now = millis();
  bindWindowStart = now;
  lastReceiveTime = now;
  lastServoUpdate = now;
  lastBatterySample = now;
  lastDebugPrint = now;
  rateWindowStart = now;
}

void loop() {
  unsigned long now = millis();

  if ((unsigned long)(now - lastBatterySample) >= BATTERY_SAMPLE_INTERVAL_MS) {
    lastBatterySample = now;
    telemetry.batteryMv = readModelBatteryMv();
  }

  bool received = false;
  uint8_t pipeNum = 0xFF;
  while (radio.available(&pipeNum)) {
    if (pipeNum == 1) {
      radio.read(&Data, sizeof(Data));

      // Bind uses the same 10-byte payload and the same proven pipe as normal
      // control traffic. ch0 is normally 0..100, so 0xB7 cannot occur in a
      // legitimate control packet and safely identifies a bind command.
      const bool hasBindMagic =
          Data.ch0 == BIND_MAGIC0 &&
          Data.ch1 == BIND_MAGIC1;

      if (hasBindMagic) {
        // A bind-looking frame is NEVER passed to the actuators. Outside the
        // 30 s pairing window it is simply ignored (the hardware ACK may still
        // have gone out, but the TX verification on the new address will fail).
        if (bindWindowActive && Data.ch2 < MODEL_COUNT) {
          boundModel = Data.ch2;
          saveBoundModel(boundModel);

          // The hardware ACK for this frame has already gone out on the old pipe.
          // Now move the RX to the newly assigned model address.
          radio.stopListening();
          openBoundModelPipe();
          radio.startListening();
          radio.writeAckPayload(1, &telemetry, sizeof(telemetry));
          bindWindowActive = false;

          resetData();
          everReceived = false;
#if RX_SERIAL_DEBUG
          Serial.print(F("BIND OK -> MODEL0")); Serial.println(boundModel + 1);
#endif
        }
      } else {
        // ch5 bit7 carries the model-specific throttle mode. The remaining
        // seven bits keep the historical TX battery value in 0.1 V units.
        throttleForwardOnly = (Data.ch5 & 0x80U) != 0;

        received = true;
        everReceived = true;
        ++totalPackets;
        ++windowPackets;
        lastReceiveTime = millis();

        // Preload telemetry for the NEXT acknowledged command packet.
        radio.writeAckPayload(1, &telemetry, sizeof(telemetry));
      }
    } else {
      // Defensive drain for any unexpected enabled pipe.
      byte discard[10];
      radio.read(&discard, sizeof(discard));
    }
  }

  now = millis();

  // Pairing commands are accepted only shortly after RX power-up.
  if (bindWindowActive && (unsigned long)(now - bindWindowStart) > BIND_WINDOW_MS) {
    bindWindowActive = false;
  }

  if ((unsigned long)(now - rateWindowStart) >= 1000UL) {
    packetsPerSecond = windowPackets;
    windowPackets = 0;
    rateWindowStart += 1000UL;
  }

  if (everReceived && (unsigned long)(now - lastReceiveTime) > FAILSAFE_TIME) {
    resetData();
  }

  if ((unsigned long)(now - lastServoUpdate) >= SERVO_INTERVAL_MS) {
    lastServoUpdate = now;
    updateOutputs();
  }

#if RX_SERIAL_DEBUG
  if ((unsigned long)(now - lastDebugPrint) >= DEBUG_INTERVAL_MS) {
    lastDebugPrint = now;
#if RX_DEBUG_ANSI
    printDebugAnsi(now);
#else
    printDebugScrolling(now);
#endif
  }
#endif

  (void)received;
}
