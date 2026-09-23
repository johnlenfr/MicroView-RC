/*
  ATtiny85 transmitter supervisor v3.17.1
  Companion for MicroView TX 4.1.13 master/slave one-wire + RF test.

  PB0 / physical pin 5 : BUZZER
  PB1 / physical pin 6 : VIBRO driver
  PB3 / physical pin 2 : transmitter battery ADC3
  PB4 / physical pin 3 : one-wire DATA to MicroView D2 through 1 kOhm

  Compile for 8 MHz internal clock. With ATTinyCore, Burn Bootloader once
  after changing clock/BOD so the fuses match the selected settings.

  Protocol v4 diagnostic (MicroView is master):
    Command LOW pulse = 1000 + command*1000 us, command 0..9.
    ATtiny never transmits asynchronously.
    After every valid command it sends a framed reply:
      SYNC    ~15000 us
      MEASURE raw LOW width measured on PB4
      ACK     1000 + decoded_command*1000 us
      PIN     1000 + state*1000 us (bit0 PB0, bit1 PB1)
      BAT     LOW width in us = transmitter battery mV

  Wide 1 ms spacing plus ordered fields removes the 50/80/200 us ambiguity
  of the previous diagnostic protocol and prevents battery/command collisions.
*/

#include <Arduino.h>

const uint8_t BUZZER_PIN = PIN_PB0;
const uint8_t VIBRO_PIN  = PIN_PB1;
const uint8_t DATA_PIN   = PIN_PB4;
const uint8_t BATTERY_ADC_CHANNEL = 3;

const uint16_t R1_KOHM = 100;
const uint16_t R2_KOHM = 10;
const uint16_t ADC_REF_MV = 1100;
const uint8_t ADC_SAMPLES = 8;
const uint16_t TX_MIN_MV = 3000U;
const uint16_t TX_MAX_MV = 11000U;

const uint16_t CMD_BASE_US = 1000U;
const uint16_t CMD_STEP_US = 1000U;
const uint16_t CMD_BOUNDARY_OFFSET_US = 200U;
const uint16_t RESP_SYNC_US = 15000U;
const uint16_t RESP_GAP_US = 2500U;

uint8_t activeAlertLevel = 0;
uint8_t activeAlertMask = 0;
unsigned long alertPatternStartMs = 0;

bool pollLowActive = false;
uint32_t pollLowStartUs = 0;
uint16_t capturedPulseUs = 0;
bool capturedPulseReady = false;
bool oneWireTxActive = false;

bool responsePending = false;
uint16_t responseMeasuredUs = 0;
uint8_t responseCommand = 0;

void writeAlertPins(bool buzzerOn, bool vibroOn) {
  digitalWrite(BUZZER_PIN, buzzerOn ? HIGH : LOW);
  digitalWrite(VIBRO_PIN, vibroOn ? HIGH : LOW);
}

uint8_t readAlertPinState() {
  uint8_t state = 0;
  if (digitalRead(BUZZER_PIN) == HIGH) state |= 0x01U;
  if (digitalRead(VIBRO_PIN) == HIGH) state |= 0x02U;
  return state;
}

void setAlertOutputs(bool on) {
  writeAlertPins(on && (activeAlertMask & 0x01U),
                 on && (activeAlertMask & 0x02U));
}

void stopAlert() {
  activeAlertLevel = 0;
  activeAlertMask = 0;
  writeAlertPins(false, false);
}

void decodeAlertCommand(uint8_t command, uint8_t &level, uint8_t &mask) {
  level = 0; mask = 0;
  switch (command) {
    case 1: level = 1; mask = 1; break;
    case 2: level = 1; mask = 2; break;
    case 3: level = 1; mask = 3; break;
    case 4: level = 2; mask = 1; break;
    case 5: level = 2; mask = 2; break;
    case 6: level = 2; mask = 3; break;
    case 7: level = 3; mask = 1; break;
    case 8: level = 3; mask = 2; break;
    case 9: level = 3; mask = 3; break;
    default: break;
  }
}

void applyAlertCommand(uint8_t command) {
  if (command == 0 || command > 9) { stopAlert(); return; }
  uint8_t level, mask;
  decodeAlertCommand(command, level, mask);
  if (!level || !mask) { stopAlert(); return; }
  if (activeAlertLevel == level && activeAlertMask == mask) return;
  writeAlertPins(false, false);
  activeAlertLevel = level;
  activeAlertMask = mask;
  alertPatternStartMs = millis();
  setAlertOutputs(true);
}

void updateAlertPattern(unsigned long now) {
  if (activeAlertLevel == 0) { setAlertOutputs(false); return; }
  const unsigned long elapsed = now - alertPatternStartMs;
  if (activeAlertLevel == 1) {
    if (elapsed < 60UL) setAlertOutputs(true); else stopAlert();
    return;
  }
  if (activeAlertLevel == 2) {
    const unsigned long t = elapsed % 4000UL;
    setAlertOutputs((t < 100UL) || (t >= 200UL && t < 300UL));
    return;
  }
  const unsigned long t = elapsed % 1600UL;
  setAlertOutputs((t < 200UL) || (t >= 350UL && t < 550UL) || (t >= 700UL && t < 1100UL));
}

uint16_t readBatteryMv() {
  uint32_t sum = 0;
  for (uint8_t i = 0; i < ADC_SAMPLES; ++i) sum += analogRead(BATTERY_ADC_CHANNEL);
  const uint16_t adc = (uint16_t)((sum + ADC_SAMPLES / 2U) / ADC_SAMPLES);
  uint32_t pinMv = ((uint32_t)adc * ADC_REF_MV + 511UL) / 1023UL;
  uint32_t battMv = pinMv * (R1_KOHM + R2_KOHM) / R2_KOHM;
  if (battMv < TX_MIN_MV) battMv = TX_MIN_MV;
  if (battMv > TX_MAX_MV) battMv = TX_MAX_MV;
  return (uint16_t)battMv;
}

void sendOneWirePulse(uint16_t widthUs) {
  oneWireTxActive = true;
  digitalWrite(DATA_PIN, LOW);
  pinMode(DATA_PIN, OUTPUT);
  delayMicroseconds(widthUs);
  pinMode(DATA_PIN, INPUT);
  delayMicroseconds(25);
  oneWireTxActive = false;
  pollLowActive = false;
}

void serviceWireInputPolling() {
  if (oneWireTxActive || responsePending) return;
  const bool low = (digitalRead(DATA_PIN) == LOW);
  const uint32_t nowUs = micros();
  if (low) {
    if (!pollLowActive) { pollLowActive = true; pollLowStartUs = nowUs; }
    return;
  }
  if (pollLowActive) {
    const uint32_t width = nowUs - pollLowStartUs;
    pollLowActive = false;
    if (width >= 500UL && width <= 12000UL) {
      capturedPulseUs = (uint16_t)width;
      capturedPulseReady = true;
    }
  }
}

void serviceCapturedPulse() {
  if (!capturedPulseReady) return;
  const uint16_t widthUs = capturedPulseUs;
  capturedPulseReady = false;

  // Polling can notice the rising edge slightly late, so measured LOW pulses
  // are occasionally longer than the pulse generated by the MicroView.
  // Use asymmetric decision bins instead of rounding to the nearest 1 ms:
  //   cmd 7 nominal 8.0 ms -> accepted bin 7.8..8.8 ms
  //   cmd 8 nominal 9.0 ms -> accepted bin 8.8..9.8 ms
  // This prevents a +0.6/+0.7 ms polling delay from becoming the next code.
  if (widthUs < (CMD_BASE_US - CMD_BOUNDARY_OFFSET_US)) return;
  const uint32_t shifted = (uint32_t)widthUs -
                           (uint32_t)(CMD_BASE_US - CMD_BOUNDARY_OFFSET_US);
  const uint8_t command = (uint8_t)(shifted / CMD_STEP_US);
  if (command > 9) return;
  const uint16_t lower = (uint16_t)(CMD_BASE_US - CMD_BOUNDARY_OFFSET_US +
                                    (uint16_t)command * CMD_STEP_US);
  const uint16_t upper = (uint16_t)(lower + CMD_STEP_US);
  if (widthUs < lower || widthUs >= upper) return;

  applyAlertCommand((uint8_t)command);
  responseMeasuredUs = widthUs;
  responseCommand = (uint8_t)command;
  responsePending = true;
}

void serviceResponseFrame() {
  if (!responsePending) return;
  if (digitalRead(DATA_PIN) == LOW) return;

  const uint16_t measured = responseMeasuredUs;
  const uint8_t command = responseCommand;
  responsePending = false;

  delayMicroseconds(RESP_GAP_US);
  sendOneWirePulse(RESP_SYNC_US);
  delayMicroseconds(RESP_GAP_US);
  sendOneWirePulse(measured);
  delayMicroseconds(RESP_GAP_US);
  sendOneWirePulse(CMD_BASE_US + (uint16_t)command * CMD_STEP_US);
  delayMicroseconds(RESP_GAP_US);
  sendOneWirePulse(CMD_BASE_US + (uint16_t)readAlertPinState() * CMD_STEP_US);
  delayMicroseconds(RESP_GAP_US);
  sendOneWirePulse(readBatteryMv());
}

void setup() {
  digitalWrite(DATA_PIN, LOW);
  pinMode(DATA_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(VIBRO_PIN, OUTPUT);
  writeAlertPins(false, false);
  analogReference(INTERNAL);
  delay(10);
  analogRead(BATTERY_ADC_CHANNEL);
}

void loop() {
  const unsigned long now = millis();
  serviceWireInputPolling();
  serviceCapturedPulse();
  serviceResponseFrame();
  updateAlertPattern(now);
}
