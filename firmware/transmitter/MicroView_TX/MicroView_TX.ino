/*
 !!!!! for soft_SPI // DON'T FORGET TO UNCOMMENT IN "RF24_config.h" the line "#define SOFTSPI" !!!!!

   REMOTE RC-CONTROL
   http://johnlenfr.1s.fr
   v1.0

 BOARD: Arduino MicroView (integrated OLED display)
 SHIELD: JOYSTICK / APC220

/////PIN MAPPING ON MICROVIEW/////
 PIN 01/RESET:  connect to reset switch if needed
 PIN 02/A5/SCL: PIN_PB_JOYSTICK_RIGHT / PIN_PB_JOYSTICK_LEFT / PIN_PB_RIGHT / PIN_PB_LEFT
 PIN 03/A4/SDA: THROTTLE (potentiometer 10K)
 PIN 04/A3:     YAW (potentiometer 10K)
 PIN 05/A2:     PITCH (potentiometer 10K)
 PIN 06/A1:     ROLL (potentiometer 10K)
 PIN 07/A0:     potentiometer

 PIN 08/GND: connect to grnd

 PIN 09/D0/Rx:  NRF24L01+ CE
 PIN 10/D1/Tx:  NRF24L01+ CSN
 PIN 11/D2:    1-wire pulse input from ATtiny85, transmitter battery in millivolts
 PIN 12/D3/PWM: NRF24L01+ MOSI
 PIN 13/D5/PWM: NRF24L01+ MISO
 PIN 14/D6/PWM: NRF24L01+ SCK

 PIN 15:       +5V
 PIN 16:       +VIN (3.3V - 16V)


D2 battery protocol:
 - line idle HIGH (MicroView INPUT_PULLUP)
 - ATtiny pulls line LOW for N microseconds
 - N = transmitter battery voltage in millivolts (e.g. 3870 us = 3.870 V)
 - pulse repeated about every 250 ms
 - debug numeric values are displayed with one decimal digit

-------------------------------------------------------------------------------
 VERSION TX 4.1.17 - RF RETRY DISPLAY LAYOUT
-------------------------------------------------------------------------------
 Base : v3.6 / v3.5 / v3.3 pour l'ecran principal.

 Fonctions :
 - mode 4 Ch. permanent
 - ecran principal : 4 sliders de sticks + pot_gauge central
 - menu ouvert par JL + JR maintenus 800 ms
 - EXPO individuel ROLL/PITCH/YAW/THROTTLE
 - D/R (Dual Rate) individuel ROLL/PITCH/YAW/THROTTLE
 - REVERSE individuel ROLL/PITCH/YAW/THROTTLE/JR/JL/PBR/PBL
 - TRIM individuel ROLL/PITCH/YAW/THROTTLE
 - DEADBAND reglable par modele
 - AUX MODE individuel : INSTANTANE ou TOGGLE pour JR/JL/PB RIGHT/PB LEFT
 - calibration min/centre/max des 4 voies
 - reset usine des reglages
 - reglages memorises en EEPROM
 - v3.10 : navigation intuitive : U/D selection, R entrer, L retour, JR valider
 - v3.8 : calibration validee avec PB LEFT
 - v3.10 : batterie telecommande simulee 2.0..5.0 V toutes les 5 s
 - v3.10 : batterie a gauche, POT centre, qualite LINK a droite
 - v3.10 : AUX affiches avec temoins graphiques au lieu de ON/OFF
 - v3.12 : AUX 7x7 px : cercle=INSTANT, carre=TOGGLE ; vide=OFF, plein=ON
 - v3.12 : aucun separateur horizontal ni repere central ajoute aux sliders
 - v3.13 : POT remplace par pot_gauge 270 deg a double arc, repere 50% fixe
 - v3.13 : 5 memoires MODEL01..MODEL05 avec parametres independants
 - v3.14 : pot_gauge CLASSIC/CUMULATIVE selon le design a double cercle
 - v3.14 : surbrillance inversee au centre numerique exact (512 sur 0..1023)
 - v3.13 : friendly name modifiable (A-Z, 0-9, espace), 6 caracteres max
 - v3.13 : nom du modele actif affiche en haut au centre
 - v3.13.1 : correction du preprocesseur Arduino pour ModelProfile/RcSettings
 - v3.13.1 : navigation joystick inhibee pendant toute la calibration
 - v3.13.1 : apres calibration, navigation deverrouillee seulement stick revenu au centre

 Navigation menu v3.14 :
 - JL + JR maintenus 800 ms : ouvrir le menu
 - joystick DROIT vers la droite : entrer dans l item selectionne
 - joystick DROIT vers la gauche : retour / sortie de l item
 - joystick DROIT vers le haut   : item precedent / augmenter en edition
 - joystick DROIT vers le bas    : item suivant / diminuer en edition
 - bouton joystick DROIT (JR)    : valider / sauvegarder
 - calibration : PB LEFT capture/sauvegarde

 Hors menu, JR/JL/PB RIGHT/PB LEFT restent des voies AUX normales.

 Calibration A5 connue :
 - JL      ~= 252
 - JR      ~= 701
 - PB LEFT ~= 786
 - NONE    ~= 1023

 JL+JR mesure en reel : environ 226 ADC. Fenetre resserree a 215..238.

  v3.14.3 NOTE:
  - 5 model slots only: MODEL01..MODEL05
  - reverse available on all 8 real controls
  - startup/logo feature removed from the sketch
  - pot_gauge inner radius reduced to 6 px for a wider annulus
  - cumulative fill uses continuous radial lines to eliminate empty pixels
  - compact EEPROM layout; legacy migration remains removed to save Flash
  - v3.14.5 : batterie modele recue par ACK payload NRF24
  - v3.14.5 : barre RX 0..100% sous le bandeau haut
  - v3.14.5 : BAT MIN/MAX par modele, pas de 0.1 V
  - v3.14.5 : nom modele alterne normal/inverse sous BAT MIN
  - v3.14.6 : jauge RX = cadre 52x4 toujours visible + remplissage 50x2
  - v3.14.6 : simulation batterie TX legere restauree sans random()
  - v3.14.7 : alarme batterie TX par clignotement du contour uniquement
  - v3.14.8 : simulation batterie RX legere, desactivable par #define
 - v3.14.9 : batterie TX geree en millivolts (ATtiny -> TX), plus en pourcentage
 - v3.14.9 : menu TX BAT global MIN/MAX ; MIN = seuil alarme, MAX = jauge pleine
 - v3.14.9 : alarme RX = vrai clignotement du texte modele, sans inversion XOR
 - v3.15.0 : protocole D2 reel depuis ATtiny85 : largeur impulsion LOW = tension TX en mV
 - v3.15.0 : reception D2 non bloquante par interruption INT0 ; simulation TX retiree
 - v3.15.1 : protocole D2 accepte 1.8..10.5 ms = 1.8..10.5 V
 - v3.15.1 : ATtiny85 prevu pour pont 100k/10k, mesure utile 2..10 V
 - v3.15.2 : simulation batterie RX supprimee ; telemetrie ACK reelle uniquement
 - v3.15.3 : PBJL = BACK/CANCEL dans tout le menu ; gauche joystick ignore
 - v3.15.4 : optimisation Flash sans retrait de fonctions
 - v4.0a : pilote OLED SSD1306 64x48 integre ; MicroView.h/MicroView_mod supprimes
 - v4.0 : sliders et pot gauge dessines directement, sans widgets ni allocation dynamique
 - v4.0b : mode diagnostic COM pour tension exacte ATtiny85 D2
 - v4.0c : batterie TX affichee par 4 quarts ; interieur plein sur toute la hauteur
 - v4.0d : tension TX bridee MIN/MAX puis mappee lineairement en 0/4..4/4
 - v4.0f : debug batterie etendu: RAW/MIN/MAX/CLAMP/LEVEL + SAVE/CANCEL
 - v4.0g : verification chaine TX BAT: EDIT -> RAM -> EEPROM -> fermeture menu -> reboot
 - v4.0h : version propre/finale ; debug serie retire ; NRF24 toujours actif ; mapping TX BAT valide
 - v4.0l : RF prioritaire a cadence fixe 20 ms ; ACK telemetrie ne force plus le redraw OLED
 - v4.1a : perte RX visible: jauge lien 0/clignotante apres 300 ms sans ACK; nom modele clignote si lien ou telemetrie perdus
 - v4.1b : nom modele toujours fixe; alarme tension/telemetrie RX = barre RX vide clignotante
 - v4.1c : perte de liaison = seul logo RF clignote; barre RX vide fixe. Barre RX clignote seulement si lien present mais telemetrie absente ou tension <= MIN
 - v4.0m : acquisition analogique allegee : sticks/pot 100 Hz, boutons 200 Hz, 2 echantillons utiles
 - v4.0n : plus de rattrapage RF en rafale ; chaque emission reprogramme la suivante a +20 ms
 - v4.0n : OLED execute uniquement apres une emission RF, jamais juste avant une echeance RF
 - v4.0o : cadence periodique sur horloge absolue +20 ms ; petit retard recupere en douceur
 - v4.0o : si une trame entiere est ratee, resynchronisation a maintenant+20 ms sans rafale
 - v4.1 : suppression des emissions RF urgentes AUX/menu ; toutes les commandes partent sur la trame 50 Hz suivante
 - v4.1 : cadence finale deterministe, ADC optimise, OLED uniquement apres trame RF
 - v4.1i : mapping restaure exactement comme v4.1g validee au test : A3=YAW, A4=THROTTLE, ch3=YAW, ch4=THROTTLE
 - v4.1l : affichage MicroView corrige : THROTTLE sur curseur vertical gauche, YAW sur curseur horizontal bas
 - v4.1m : chaque MODEL01..05 utilise une adresse RX distincte Rx001..Rx005
 - v4.1m : MODEL > BIND RX permet d'appairer le RX actif au modele selectionne
 - v4.1.5 : sortie alertes ATtiny fiabilisee ; table BUZZ/VIB/B+V explicite
 - v4.1.5 : page debug 3 temporaire ALERT DBG conservee
 - v4.1.7 : protocole alertes espace (200 us), ACK et etat PB0/PB1 separes
 - v4.1.16 : RF DEBUG R lit directement dans OBSERVE_TX.ARC_CNT pour compatibilite anciennes bibliotheques RF24
 - v4.1.17 : RF DEBUG separation visuelle compacte entre latence et retries : Lx.xMS/Rx
*/

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <DigitalIO.h> // soft SPI RF24
#include <nRF24L01.h>
#include <RF24.h>
#include <EEPROM.h>

#define WIRE_SERIAL_DIAG 0

// -----------------------------------------------------------------------------
// v4.0 - Minimal integrated MicroView OLED driver
// No MicroView library is required. The internal SSD1306 is driven directly
// through the ATmega328P hardware SPI. The visible panel is 64x48 (384 bytes).
// -----------------------------------------------------------------------------
#define OLED_W 64
#define OLED_H 48
#define OLED_PAGES 6
#define OLED_PWR_PIN 4

#define WHITE 1
#define BLACK 0
#define NORM  0
#define PAGE  0
#define POT_GAUGE_CLASSIC    0
#define POT_GAUGE_CUMULATIVE 1

#define OLED_DC_LOW()   do { PORTB &= ~_BV(PB0); DDRB |=  _BV(DDB0); } while (0)
#define OLED_DC_HIGH()  do { PORTB |=  _BV(PB0); DDRB &= ~_BV(DDB0); } while (0)
#define OLED_CS_LOW()   do { PORTB &= ~_BV(PB2); DDRB |=  _BV(DDB2); } while (0)
#define OLED_CS_HIGH()  do { PORTB |=  _BV(PB2); DDRB &= ~_BV(DDB2); } while (0)
#define OLED_RST_LOW()  do { PORTD &= ~_BV(PD7); DDRD |=  _BV(DDD7); } while (0)
#define OLED_RST_HIGH() do { PORTD |=  _BV(PD7); DDRD &= ~_BV(DDD7); } while (0)

static uint8_t screenBuffer[OLED_W * OLED_PAGES];

// ASCII 32..90 only: all characters used by this transmitter UI.
static const uint8_t font5x7_32_90[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x5F,0x00,0x00,
  0x00,0x07,0x00,0x07,0x00, 0x14,0x7F,0x14,0x7F,0x14,
  0x24,0x2A,0x7F,0x2A,0x12, 0x23,0x13,0x08,0x64,0x62,
  0x36,0x49,0x55,0x22,0x50, 0x00,0x05,0x03,0x00,0x00,
  0x00,0x1C,0x22,0x41,0x00, 0x00,0x41,0x22,0x1C,0x00,
  0x14,0x08,0x3E,0x08,0x14, 0x08,0x08,0x3E,0x08,0x08,
  0x00,0x50,0x30,0x00,0x00, 0x08,0x08,0x08,0x08,0x08,
  0x00,0x60,0x60,0x00,0x00, 0x20,0x10,0x08,0x04,0x02,
  0x3E,0x51,0x49,0x45,0x3E, 0x00,0x42,0x7F,0x40,0x00,
  0x42,0x61,0x51,0x49,0x46, 0x21,0x41,0x45,0x4B,0x31,
  0x18,0x14,0x12,0x7F,0x10, 0x27,0x45,0x45,0x45,0x39,
  0x3C,0x4A,0x49,0x49,0x30, 0x01,0x71,0x09,0x05,0x03,
  0x36,0x49,0x49,0x49,0x36, 0x06,0x49,0x49,0x29,0x1E,
  0x00,0x36,0x36,0x00,0x00, 0x00,0x56,0x36,0x00,0x00,
  0x08,0x14,0x22,0x41,0x00, 0x14,0x14,0x14,0x14,0x14,
  0x00,0x41,0x22,0x14,0x08, 0x02,0x01,0x51,0x09,0x06,
  0x32,0x49,0x79,0x41,0x3E, 0x7E,0x11,0x11,0x11,0x7E,
  0x7F,0x49,0x49,0x49,0x36, 0x3E,0x41,0x41,0x41,0x22,
  0x7F,0x41,0x41,0x22,0x1C, 0x7F,0x49,0x49,0x49,0x41,
  0x7F,0x09,0x09,0x09,0x01, 0x3E,0x41,0x49,0x49,0x7A,
  0x7F,0x08,0x08,0x08,0x7F, 0x00,0x41,0x7F,0x41,0x00,
  0x20,0x40,0x41,0x3F,0x01, 0x7F,0x08,0x14,0x22,0x41,
  0x7F,0x40,0x40,0x40,0x40, 0x7F,0x02,0x0C,0x02,0x7F,
  0x7F,0x04,0x08,0x10,0x7F, 0x3E,0x41,0x41,0x41,0x3E,
  0x7F,0x09,0x09,0x09,0x06, 0x3E,0x41,0x51,0x21,0x5E,
  0x7F,0x09,0x19,0x29,0x46, 0x46,0x49,0x49,0x49,0x31,
  0x01,0x01,0x7F,0x01,0x01, 0x3F,0x40,0x40,0x40,0x3F,
  0x1F,0x20,0x40,0x20,0x1F, 0x3F,0x40,0x38,0x40,0x3F,
  0x63,0x14,0x08,0x14,0x63, 0x07,0x08,0x70,0x08,0x07,
  0x61,0x51,0x49,0x45,0x43
};

class MicroViewOLEDLite {
public:
  MicroViewOLEDLite() : cursorX(0), cursorY(0) {}

  void begin() {
    pinMode(OLED_PWR_PIN, OUTPUT);
    digitalWrite(OLED_PWR_PIN, HIGH);
    delay(10);
    OLED_CS_HIGH(); OLED_DC_LOW(); OLED_RST_HIGH();

    DDRB |= _BV(DDB3) | _BV(DDB5); // MOSI + SCK
    PORTB &= ~(_BV(PB3) | _BV(PB5));
    SPCR = _BV(SPE) | _BV(MSTR);     // SPI mode 0, F_CPU/4
    SPSR &= ~_BV(SPI2X);

    OLED_RST_HIGH(); delay(5);
    OLED_RST_LOW();  delay(10);
    OLED_RST_HIGH();

    command(0xAE);
    command(0xD5); command(0x80);
    command(0xA8); command(0x2F);
    command(0xD3); command(0x00);
    command(0x40);
    command(0x8D); command(0x14);
    command(0xA6); command(0xA4);
    command(0xA1); command(0xC8);
    command(0xDA); command(0x12);
    command(0x81); command(0x8F);
    command(0xD9); command(0xF1);
    command(0xDB); command(0x40);
    clear(PAGE);
    clearHardware();
    command(0xAF);
  }

  void clear(uint8_t = PAGE) { memset(screenBuffer, 0, sizeof(screenBuffer)); }

  void display() {
    for (uint8_t page = 0; page < OLED_PAGES; ++page) {
      command(0xB0 | page); setColumn0();
      OLED_DC_HIGH(); OLED_CS_LOW();
      const uint16_t base = (uint16_t)page * OLED_W;
      for (uint8_t x = 0; x < OLED_W; ++x) send(screenBuffer[base + x]);
      OLED_CS_HIGH();
    }
  }

  void setCursor(uint8_t x, uint8_t y) { cursorX = x; cursorY = y; }

  void pixel(int16_t x, int16_t y, uint8_t color = WHITE, uint8_t = NORM) {
    if (x < 0 || x >= OLED_W || y < 0 || y >= OLED_H) return;
    const uint16_t i = (uint16_t)(y >> 3) * OLED_W + (uint8_t)x;
    const uint8_t mask = _BV(y & 7);
    if (color) screenBuffer[i] |= mask;
    else       screenBuffer[i] &= (uint8_t)~mask;
  }

  void lineH(int16_t x, int16_t y, int16_t len,
             uint8_t color = WHITE, uint8_t = NORM) {
    if (len <= 0) return;
    for (int16_t i = 0; i < len; ++i) pixel(x + i, y, color);
  }

  void lineV(int16_t x, int16_t y, int16_t len,
             uint8_t color = WHITE, uint8_t = NORM) {
    if (len <= 0) return;
    for (int16_t i = 0; i < len; ++i) pixel(x, y + i, color);
  }

  void line(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
            uint8_t color = WHITE, uint8_t = NORM) {
    int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy;
    for (;;) {
      pixel(x0, y0, color);
      if (x0 == x1 && y0 == y1) break;
      const int16_t e2 = err << 1;
      if (e2 >= dy) { err += dy; x0 += sx; }
      if (e2 <= dx) { err += dx; y0 += sy; }
    }
  }

  void rect(int16_t x, int16_t y, int16_t w, int16_t h,
            uint8_t color = WHITE, uint8_t = NORM) {
    if (w <= 0 || h <= 0) return;
    lineH(x, y, w, color);
    if (h > 1) lineH(x, y + h - 1, w, color);
    if (h > 2) {
      lineV(x, y + 1, h - 2, color);
      if (w > 1) lineV(x + w - 1, y + 1, h - 2, color);
    }
  }

  void rectFill(int16_t x, int16_t y, int16_t w, int16_t h,
                uint8_t color = WHITE, uint8_t = NORM) {
    if (w <= 0 || h <= 0) return;
    for (int16_t px = x; px < x + w; ++px) lineV(px, y, h, color);
  }

  void circle(int16_t x0, int16_t y0, int8_t r,
              uint8_t color = WHITE, uint8_t = NORM) {
    int16_t f = 1 - r, ddFx = 1, ddFy = -2 * r, x = 0, y = r;
    pixel(x0, y0 + r, color); pixel(x0, y0 - r, color);
    pixel(x0 + r, y0, color); pixel(x0 - r, y0, color);
    while (x < y) {
      if (f >= 0) { --y; ddFy += 2; f += ddFy; }
      ++x; ddFx += 2; f += ddFx;
      pixel(x0+x,y0+y,color); pixel(x0-x,y0+y,color);
      pixel(x0+x,y0-y,color); pixel(x0-x,y0-y,color);
      pixel(x0+y,y0+x,color); pixel(x0-y,y0+x,color);
      pixel(x0+y,y0-x,color); pixel(x0-y,y0-x,color);
    }
  }

  void circleFill(int16_t x0, int16_t y0, int8_t r,
                  uint8_t color = WHITE, uint8_t = NORM) {
    // Pixel-centred filled circle. Using a half-pixel larger effective radius
    // gives small circles (especially r=3 AUX icons) a much rounder 7x7 shape:
    //   3,5,7,7,7,5,3 pixels instead of 1,5,5,7,5,5,1.
    const int16_t diameter2 = (int16_t)(2 * r + 1);
    const int16_t radius2x4 = diameter2 * diameter2;
    for (int16_t y = -r; y <= r; ++y) {
      int16_t x = r;
      while ((int16_t)(4 * (x * x + y * y)) > radius2x4) --x;
      lineH(x0 - x, y0 + y, 2 * x + 1, color);
    }
  }

  void print(char c) { drawChar(c); }
  void print(const char *s) { while (*s) drawChar(*s++); }
  void print(const __FlashStringHelper *p) {
    PGM_P s = reinterpret_cast<PGM_P>(p);
    char c;
    while ((c = (char)pgm_read_byte(s++)) != 0) drawChar(c);
  }
  void print(signed char v)   { print((long)v); }
  void print(unsigned char v) { print((unsigned long)v); }
  void print(int v)           { print((long)v); }
  void print(unsigned int v)  { print((unsigned long)v); }
  void print(long v) {
    if (v < 0) { drawChar('-'); v = -v; }
    print((unsigned long)v);
  }
  void print(unsigned long v) {
    char buf[11]; uint8_t n = 0;
    do { buf[n++] = (char)('0' + (v % 10UL)); v /= 10UL; } while (v && n < sizeof(buf));
    while (n) drawChar(buf[--n]);
  }

private:
  uint8_t cursorX, cursorY;

  static inline void waitSPI() { while (!(SPSR & _BV(SPIF))) {} }
  static inline void send(uint8_t v) { SPDR = v; waitSPI(); }
  static void command(uint8_t v) {
    OLED_DC_LOW(); OLED_CS_LOW(); send(v); OLED_CS_HIGH();
  }
  static void setColumn0() { command(0x12); command(0x00); }
  static void clearHardware() {
    for (uint8_t page = 0; page < OLED_PAGES; ++page) {
      command(0xB0 | page); setColumn0();
      OLED_DC_HIGH(); OLED_CS_LOW();
      for (uint8_t x = 0; x < OLED_W; ++x) send(0);
      OLED_CS_HIGH();
    }
  }

  void drawChar(char c) {
    if (c < 32 || c > 90) c = '?';
    const uint16_t base = (uint16_t)(c - 32) * 5U;
    for (uint8_t col = 0; col < 5; ++col) {
      uint8_t bits = pgm_read_byte(&font5x7_32_90[base + col]);
      for (uint8_t row = 0; row < 7; ++row)
        if (bits & _BV(row)) pixel(cursorX + col, cursorY + row, WHITE);
    }
    cursorX += 6;
  }
};

MicroViewOLEDLite uView;

// Shared OLED strings: one Flash copy for repeated labels.
#define PFSTR(s) ((const __FlashStringHelper *)(s))
static const char S_MODEL[] PROGMEM = "MODEL";
static const char S_CALIB[] PROGMEM = "CALIB";
static const char S_JR[] PROGMEM = "JR";
static const char S_JL[] PROGMEM = "JL";
static const char S_PBR[] PROGMEM = "PBR";
static const char S_PBL[] PROGMEM = "PBL";
static const char S_EXPO[] PROGMEM = "EXPO";
static const char S_DR[] PROGMEM = "D/R";
static const char S_REVERSE[] PROGMEM = "REVERSE";
static const char S_TRIM[] PROGMEM = "TRIM";
static const char S_DEADBAND[] PROGMEM = "DEADBAND";
static const char S_AUX_MODE[] PROGMEM = "AUX MODE";
static const char S_POT_DISP[] PROGMEM = "POT DISP";
static const char S_RX_BAT[] PROGMEM = "RX BAT";
static const char S_TX_BAT[] PROGMEM = "TX BAT";
static const char S_RF_POWER[] PROGMEM = "RF POWER";
static const char S_DEBUG_SCR[] PROGMEM = "DEBUG SCR";
static const char S_ENDPOINT[] PROGMEM = "ENDPOINT";
static const char S_THR_MODE[] PROGMEM = "THR MODE";
static const char S_ALERTS[] PROGMEM = "ALERTS";
static const char S_UD_CHG[] PROGMEM = "U/D CHG";
static const char S_UP_DOWN[] PROGMEM = "U:+ D:-";
static const char S_OK_BACK[] PROGMEM = "JR:OK L:<";
static const char S_CAL_BACK[] PROGMEM = "PBL:OK JL:<";

// ---------------- Pins ----------------
#define PIN_PB A5
const uint8_t SENSOR_PINS[5] = {A0, A1, A2, A3, A4};

enum ChannelIndex : uint8_t {
  CH_POT = 0,
  CH_ROLL,
  CH_PITCH,
  CH_YAW,
  CH_THROTTLE,
  CHANNEL_COUNT
};

// Axis settings use indexes 0..3 in this order:
// ROLL, PITCH, YAW, THROTTLE.
// Keeping A3/A4 in their physical slots preserves existing calibration values.
const uint8_t AXIS_COUNT = 4;
const uint8_t AXIS_TO_CHANNEL[AXIS_COUNT] = {
  CH_ROLL, CH_PITCH, CH_YAW, CH_THROTTLE
};

// No fixed physical axis inversion is applied.
// Direction changes are handled only by the per-model REVERSE menu.
const uint8_t PHYSICAL_AXIS_INVERT_MASK = 0;

// ---------------- Stable analog values ----------------
uint16_t sensorValue[CHANNEL_COUNT] = {512, 512, 512, 512, 512};
int32_t filterStateQ4[CHANNEL_COUNT] = {0, 0, 0, 0, 0};

const uint8_t ADC_SAMPLES = 2; // v4.0m: 1 conversion jetee + 2 utiles
const uint8_t ADC_DEADBAND = 3;
const uint8_t FILTER_DIVISOR = 4;

// ---------------- Buttons / A5 ladder ----------------
enum ButtonCode : uint8_t {
  BUTTON_NONE = 0,
  BUTTON_JOYSTICK_RIGHT,
  BUTTON_JOYSTICK_LEFT,
  BUTTON_RIGHT,
  BUTTON_LEFT,
  BUTTON_MENU_COMBO
};

uint16_t sensorValue5_switch = 1023;
uint8_t buttonCandidate = BUTTON_NONE;
uint8_t buttonStable = BUTTON_NONE;
unsigned long buttonCandidateSince = 0;
const unsigned long BUTTON_DEBOUNCE_MS = 6;

// Mesures reelles apres recablage.
const uint16_t BUTTON_JL_MIN       = 242;
const uint16_t BUTTON_JL_MAX       = 290;
const uint16_t BUTTON_JR_MIN       = 665;
const uint16_t BUTTON_JR_MAX       = 735;
const uint16_t BUTTON_LEFT_MIN     = 750;
const uint16_t BUTTON_LEFT_MAX     = 825;
const uint16_t BUTTON_NONE_MIN     = 930;

// PB RIGHT n'a pas encore ete mesure apres recablage.
const uint16_t BUTTON_RIGHT_MIN    = 350;
const uint16_t BUTTON_RIGHT_MAX    = 620;

// JL+JR simultanes mesure en reel : moyenne ~226.
const uint16_t MENU_COMBO_MIN      = 215;
const uint16_t MENU_COMBO_MAX      = 238;
const unsigned long MENU_HOLD_MS   = 800;

// ---------------- AUX outputs ----------------
enum AuxIndex : uint8_t {
  AUX_JR = 0,
  AUX_JL,
  AUX_RIGHT,
  AUX_LEFT,
  AUX_COUNT
};

uint8_t auxOutput[AUX_COUNT]  = {0, 0, 0, 0};
uint8_t auxLatched[AUX_COUNT] = {0, 0, 0, 0};

// Used to undo an accidental toggle if one joystick button was detected a few
// milliseconds before the JL+JR menu combination became stable.
uint8_t lastAuxPressId = BUTTON_NONE;
unsigned long lastAuxPressTime = 0;
uint8_t lastAuxPreviousLatch = 0;
bool lastAuxPressWasToggle = false;
const unsigned long COMBO_ROLLBACK_MS = 300;

// ---------------- Persistent settings / 5 models ----------------
// Fixed internal identifiers. These never change; the friendly name is only
// the human-readable label linked to each MODELxx slot.
enum ModelId : uint8_t {
  MODEL01 = 0,
  MODEL02,
  MODEL03,
  MODEL04,
  MODEL05,
  MODEL_COUNT
};

const uint8_t MODEL_NAME_LEN = 6; // max visible friendly-name chars on main screen
const uint16_t SETTINGS_MAGIC = 0x4D35; // "M5" = compact 5-model layout
const uint8_t SETTINGS_VERSION = 2;
const uint8_t SETTINGS_EXT_MARKER = 0xA1;
const uint8_t SETTINGS_DEBUG_MARKER = 0xD7;
const uint8_t SETTINGS_FEATURE_MARKER = 0xE3;

enum ThrottleMode : uint8_t {
  THROTTLE_BIDIR = 0,
  THROTTLE_FWD_ONLY = 1
};

enum AlertOutputMode : uint8_t {
  ALERT_OUTPUT_OFF = 0,
  ALERT_OUTPUT_BUZZER,
  ALERT_OUTPUT_BUZZ_VIB,
  ALERT_OUTPUT_VIBRO
};

const uint8_t ALERT_LEVEL_1_BIT = 0x01;
const uint8_t ALERT_LEVEL_2_BIT = 0x02;
const uint8_t ALERT_LEVEL_3_BIT = 0x04;
const uint8_t ALERT_LEVEL_ALL = ALERT_LEVEL_1_BIT | ALERT_LEVEL_2_BIT | ALERT_LEVEL_3_BIT;
const uint8_t ENDPOINT_MIN_PERCENT = 50;
const uint8_t ENDPOINT_MAX_PERCENT = 125;

enum EditKind : uint8_t {
  EDIT_EXPO = 0,
  EDIT_DUALRATE,
  EDIT_REVERSE,
  EDIT_TRIM
};

// Working settings. Keeping the same field names as v3.12 limits changes in
// the control-processing code. Model-specific fields are swapped when MODELxx
// changes; physical calibration remains global transmitter settings.
struct RcSettings {
  uint8_t expo[AXIS_COUNT];
  uint8_t dualRate[AXIS_COUNT];
  uint8_t reverseMask;
  int8_t trim[AXIS_COUNT];
  uint8_t deadband;
  uint8_t auxToggleMask;
  uint16_t calMin[AXIS_COUNT];
  uint16_t calCenter[AXIS_COUNT];
  uint16_t calMax[AXIS_COUNT];
};

struct ModelProfile {
  char friendlyName[MODEL_NAME_LEN + 1];
  uint8_t expo[AXIS_COUNT];
  uint8_t dualRate[AXIS_COUNT];
  uint8_t reverseMask;
  int8_t trim[AXIS_COUNT];
  uint8_t deadband;
  uint8_t auxToggleMask;
};

struct PersistedSettings {
  uint16_t magic;
  uint8_t version;
  uint8_t activeModel;
  uint8_t reserved0; // preserves EEPROM offsets from v3.14.2; no logo feature
  uint16_t calMin[AXIS_COUNT];
  uint16_t calCenter[AXIS_COUNT];
  uint16_t calMax[AXIS_COUNT];
  ModelProfile model[MODEL_COUNT];
  uint8_t potGaugeStyle;
  // v3.14.5: 0.1 V units, appended to preserve all v3.14.3 EEPROM offsets.
  uint8_t rxBatteryMin10[MODEL_COUNT];
  uint8_t rxBatteryMax10[MODEL_COUNT];
  // v3.14.9: TX battery limits are GLOBAL (same transmitter for all models).
  // Stored in 0.1 V units and appended to preserve previous EEPROM offsets.
  uint8_t txBatteryMin10;
  uint8_t txBatteryMax10;
  // v4.1j: global nRF24 transmit-power menu. Appended to preserve all prior offsets.
  uint8_t rfPowerLevel;
  // v4.1t: optional secondary RF/debug screen. Appended for EEPROM compatibility.
  uint8_t debugScreenMarker;
  uint8_t debugScreenEnabled; // 0=OFF, 1=ON

  // v4.1u extensions appended only: old EEPROM offsets stay unchanged.
  uint8_t featureMarker;
  uint8_t endpointNeg[MODEL_COUNT][AXIS_COUNT]; // 50..125%, per model/axis
  uint8_t endpointPos[MODEL_COUNT][AXIS_COUNT]; // 50..125%, per model/axis
  uint8_t throttleMode[MODEL_COUNT];             // BIDIR / FWD ONLY
  uint8_t alertOutputMode;                       // global transmitter setting
  uint8_t alertLevelMask;                        // bits L1/L2/L3, never 0 when enabled
};

// Explicit prototypes using custom structs.
// Older Arduino IDE preprocessors otherwise generate these prototypes before
// ModelProfile/RcSettings are declared, causing "does not name a type" errors.
void setDefaultProfile(ModelProfile &p, uint8_t modelIndex);
void triggerInfoAlert();

RcSettings settings;
PersistedSettings storedSettings;
uint8_t activeModel = MODEL01;
uint8_t potGaugeStyle = POT_GAUGE_CLASSIC;
uint8_t debugScreenEnabled = 0;
bool debugScreenActive = false;
uint8_t debugScreenPage = 0; // 0=MAIN, 1=RF DEBUG, 2=WIRE DEBUG (temporary)
uint8_t alertOutputMode = ALERT_OUTPUT_OFF;
uint8_t alertLevelMask = ALERT_LEVEL_ALL;

// ---------------- RX model battery telemetry ----------------
// ACK payload contains one uint16_t: real model voltage in millivolts.
uint16_t rxBatteryMv = 0;
unsigned long lastTelemetryTime = 0;
const unsigned long TELEMETRY_TIMEOUT_MS = 2000UL;

// ---------------- ATtiny85 <-> MicroView D2 one-wire protocol ----------------
// One-wire pulse protocol, deliberately tiny to preserve MicroView Flash:
//   idle = HIGH (D2 INPUT_PULLUP)
//   ATtiny pulls D2 LOW for N microseconds
//   N = battery voltage in millivolts (3870 us -> 3870 mV)
// D2 is INT0 on the ATmega328P, so reception is non-blocking.
#define PIN_TX_BATTERY 2
const unsigned long TX_BATTERY_INPUT_TIMEOUT_MS = 2500UL;
volatile uint16_t txBatteryPulseStartUs = 0;
volatile uint16_t txBatteryPendingMv = 0;
volatile uint8_t txBatteryPulseReady = 0;
unsigned long lastTxBatteryInputTime = 0;

// MicroView <-> ATtiny85 one-wire protocol v3 (master/slave diagnostic).
// The MicroView is the only initiator. The ATtiny no longer sends battery
// pulses asynchronously, eliminating collisions on the half-duplex line.
//
// Command: 1000 + command*1000 us, command 0..9 (1.0..10.0 ms).
// Reply frame from ATtiny, always in this order:
//   SYNC    : ~15.0 ms
//   MEASURE : raw command width measured by ATtiny
//   ACK     : 1000 + decoded_command*1000 us
//   PIN     : 1000 + state*1000 us (bit0=PB0, bit1=PB1)
//   BAT     : pulse width = battery voltage in mV
// Meaning is determined by frame order, not by narrow pulse-width ranges.
const uint16_t ALERT_WIRE_BASE_US = 1000U;
const uint16_t ALERT_WIRE_STEP_US = 1000U;
const uint16_t ALERT_FIELD_TOLERANCE_US = 350U;
const uint16_t WIRE_SYNC_MIN_US = 13000U;
const uint16_t WIRE_SYNC_MAX_US = 17000U;
const uint16_t TX_BATTERY_PULSE_MIN_US = 3000U;
const uint16_t TX_BATTERY_PULSE_MAX_US = 11000U;
const unsigned long ALERT_RESEND_MS = 200UL;
const unsigned long ALERT_LINK_GRACE_MS = 2000UL;
const unsigned long ALERT_RX_CRITICAL_TIMEOUT_MS = 1000UL;

volatile uint8_t wireReplyStage = 0; // 0 wait sync, 1 measure, 2 ack, 3 pin, 4 battery
volatile uint8_t wireMasterTxActive = 0;
uint8_t alertInfoPending = 0;
uint8_t alertLastWireCommand = 0xFF;
unsigned long alertLastWireSendMs = 0;
unsigned long alertInfoHoldUntilMs = 0;
unsigned long systemStartMs = 0;

// Temporary alert-wire diagnostics for page 3.
volatile uint8_t alertAckPendingCommand = 0xFF;
volatile uint16_t alertAckPendingWidthUs = 0;
volatile uint8_t alertAckPulseReady = 0;
uint8_t alertAckCommand = 0xFF;
uint16_t alertAckWidthUs = 0;
unsigned long lastAlertAckTime = 0;
volatile uint8_t alertPinStatePending = 0xFF;
volatile uint8_t alertPinStatePulseReady = 0;
uint8_t alertPinState = 0xFF;
unsigned long lastAlertPinStateTime = 0;

// Raw one-wire timing diagnostics.
volatile uint16_t alertTxEchoPendingWidthUs = 0;
volatile uint8_t alertTxEchoPulseReady = 0;
uint16_t alertTxEchoWidthUs = 0;
unsigned long lastAlertTxEchoTime = 0;
volatile uint16_t alertRemoteMeasurePendingUs = 0;
volatile uint8_t alertRemoteMeasurePulseReady = 0;
uint16_t alertRemoteMeasureUs = 0;
unsigned long lastAlertRemoteMeasureTime = 0;

// ---------------- Transmitter battery display ----------------
// REAL transmitter battery voltage received from the ATtiny85, in millivolts.
// 0 means no valid D2 measurement has been received yet / input timeout.
// Declared before the serial diagnostic helpers because they print this value.
uint16_t txBatteryMv = 0;

uint8_t evaluateContinuousAlertLevel(unsigned long now);

#if WIRE_SERIAL_DIAG
// Raw D2 pulse trace. ISR only stores widths; printing is done from loop().
const uint8_t WIRE_DIAG_BUF_SIZE = 16;
volatile uint16_t wireDiagWidth[WIRE_DIAG_BUF_SIZE];
volatile uint8_t wireDiagHead = 0;
volatile uint8_t wireDiagTail = 0;
uint32_t wireDiagCountSelf = 0;
uint32_t wireDiagCountAck = 0;
uint32_t wireDiagCountPin = 0;
uint32_t wireDiagCountMeasure = 0;
uint32_t wireDiagCountBattery = 0;
uint32_t wireDiagCountUnknown = 0;
unsigned long wireDiagLastSummaryMs = 0;
uint8_t wireDiagForcedCommand = 0xFF; // 0..9 forced, 0xFF = normal menu logic

static void wireDiagPushISR(uint16_t widthUs) {
  const uint8_t next = (uint8_t)((wireDiagHead + 1U) % WIRE_DIAG_BUF_SIZE);
  if (next == wireDiagTail) return;
  wireDiagWidth[wireDiagHead] = widthUs;
  wireDiagHead = next;
}

static const __FlashStringHelper *wireDiagClass(uint16_t w) {
  if (w >= WIRE_SYNC_MIN_US && w <= WIRE_SYNC_MAX_US) return F("SYNC");
  return F("PULSE");
}

static void serialPrintMs1(uint16_t us) {
  const uint16_t tenths = (uint16_t)((us + 50U) / 100U);
  Serial.print(tenths / 10U);
  Serial.print('.');
  Serial.print(tenths % 10U);
}

static void serialPrintVolts1(uint16_t mv) {
  const uint16_t tenths = (uint16_t)((mv + 50U) / 100U);
  Serial.print(tenths / 10U);
  Serial.print('.');
  Serial.print(tenths % 10U);
}

void serviceWireSerialInput() {
  while (Serial.available()) {
    const char c = (char)Serial.read();
    if (c >= '0' && c <= '9') {
      wireDiagForcedCommand = (uint8_t)(c - '0');
      alertLastWireCommand = 0xFF;
      Serial.print(F("[FORCE] command=")); Serial.println(wireDiagForcedCommand);
    } else if (c == 'n' || c == 'N') {
      wireDiagForcedCommand = 0xFF;
      alertLastWireCommand = 0xFF;
      Serial.println(F("[FORCE] cleared; menu/alert logic active"));
    } else if (c == '?' || c == 'h' || c == 'H') {
      Serial.println(F("[HELP] type 0..9 to force wire command; N=normal; ?=help"));
    }
  }
}

void serviceWireSerialDebug(unsigned long now) {
  serviceWireSerialInput();
  while (wireDiagTail != wireDiagHead) {
    noInterrupts();
    const uint16_t w = wireDiagWidth[wireDiagTail];
    wireDiagTail = (uint8_t)((wireDiagTail + 1U) % WIRE_DIAG_BUF_SIZE);
    interrupts();

    if (w >= WIRE_SYNC_MIN_US && w <= WIRE_SYNC_MAX_US) ++wireDiagCountMeasure;
    else ++wireDiagCountUnknown;

    Serial.print(F("[D2] t="));
    Serial.print(now / 1000UL); Serial.print('.'); Serial.print((now % 1000UL) / 100UL);
    Serial.print(F("s width=")); serialPrintMs1(w);
    Serial.print(F("ms class=")); Serial.println(wireDiagClass(w));
  }

  if ((unsigned long)(now - wireDiagLastSummaryMs) >= 500UL) {
    wireDiagLastSummaryMs = now;
    Serial.print(F("[STATE] line=")); Serial.print(digitalRead(PIN_TX_BATTERY) ? 'H' : 'L');
    Serial.print(F(" out=")); Serial.print(alertOutputMode);
    Serial.print(F(" lvl=")); Serial.print(evaluateContinuousAlertLevel(now));
    Serial.print(F(" cmd=")); Serial.print(alertLastWireCommand == 0xFF ? 255 : alertLastWireCommand);
    Serial.print(F(" echo=")); serialPrintMs1(alertTxEchoWidthUs);
    Serial.print(F("ms ack="));
    if ((unsigned long)(now - lastAlertAckTime) <= 1500UL && alertAckCommand <= 9) Serial.print(alertAckCommand); else Serial.print('-');
    Serial.print(F(" at=")); serialPrintMs1(alertRemoteMeasureUs);
    Serial.print(F("ms pin="));
    if ((unsigned long)(now - lastAlertPinStateTime) <= 1500UL && alertPinState <= 3) Serial.print(alertPinState); else Serial.print('-');
    Serial.print(F(" batt=")); serialPrintVolts1(txBatteryMv);
    Serial.println(F("V"));

    Serial.print(F("[COUNT] cmd=")); Serial.print(wireDiagCountSelf);
    Serial.print(F(" ack=")); Serial.print(wireDiagCountAck);
    Serial.print(F(" pin=")); Serial.print(wireDiagCountPin);
    Serial.print(F(" meas=")); Serial.print(wireDiagCountMeasure);
    Serial.print(F(" bat=")); Serial.print(wireDiagCountBattery);
    Serial.print(F(" unk=")); Serial.println(wireDiagCountUnknown);
  }
}
#endif

// Character set available in the friendly-name editor.
const char MODEL_CHARSET[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const uint8_t MODEL_CHARSET_LEN = sizeof(MODEL_CHARSET) - 1;

// Default global TX limits (1S LiPo-style starting point). They are editable
// in SETTINGS > TX BAT, so other battery packs can be configured on the radio.
const uint8_t TX_BATTERY_MIN10_DEFAULT = 33; // 3.3 V = alarm / empty gauge
const uint8_t TX_BATTERY_MAX10_DEFAULT = 42; // 4.2 V = full gauge

// ---------------- Radio link indicator ----------------
// This is a link-quality estimate based on ACK success rate, not RSSI.
// Compatible with older RF24 libraries that do not provide getARC().
uint16_t linkQualityAccum = 0;
uint8_t linkQualitySamples = 0;
uint8_t linkBars = 0;
const uint8_t LINK_WINDOW_SAMPLES = 20;
unsigned long lastAckTime = 0;
const unsigned long LINK_LOST_TIMEOUT_MS = 300UL;

// ---------------- NRF24L01 ----------------
// Normal RF build: D0/D1 are used by the nRF24 CE/CSN interface;
// hardware Serial is disabled in this build.
#define CE_PIN  0
#define CSN_PIN 1
const uint8_t RF_CHANNEL = 76;
const uint8_t BIND_MAGIC0 = 0xB7;
const uint8_t BIND_MAGIC1 = 0x42;
RF24 radio(CE_PIN, CSN_PIN);

// v4.1.16+ - RF24 compatibility helper --------------------------------------
// Some older RF24 library releases do not expose radio.getARC().  The nRF24L01+
// itself always exposes the same information in OBSERVE_TX.ARC_CNT (bits 3..0).
// Read that register directly over the already configured software-SPI pins.
// This keeps R available without requiring a newer RF24 library.
const uint8_t NRF_SOFT_MOSI_PIN = 3;
const uint8_t NRF_SOFT_MISO_PIN = 5;
const uint8_t NRF_SOFT_SCK_PIN  = 6;

static uint8_t nrfCompatSpiTransfer(uint8_t outByte) {
  uint8_t inByte = 0;
  for (uint8_t bit = 0; bit < 8; ++bit) {
    digitalWrite(NRF_SOFT_MOSI_PIN, (outByte & 0x80U) ? HIGH : LOW);
    digitalWrite(NRF_SOFT_SCK_PIN, HIGH);       // nRF24 SPI mode 0: sample on rising edge
    inByte <<= 1;
    if (digitalRead(NRF_SOFT_MISO_PIN)) inByte |= 0x01U;
    digitalWrite(NRF_SOFT_SCK_PIN, LOW);
    outByte <<= 1;
  }
  return inByte;
}

static uint8_t readNrfArcCountCompat() {
  // Preserve the pin directions expected by the RF24 SOFTSPI configuration.
  pinMode(NRF_SOFT_MOSI_PIN, OUTPUT);
  pinMode(NRF_SOFT_MISO_PIN, INPUT);
  pinMode(NRF_SOFT_SCK_PIN, OUTPUT);
  pinMode(CSN_PIN, OUTPUT);
  digitalWrite(NRF_SOFT_SCK_PIN, LOW);
  digitalWrite(CSN_PIN, HIGH);

  // R_REGISTER (0x00) | OBSERVE_TX (0x08), followed by NOP (0xFF).
  digitalWrite(CSN_PIN, LOW);
  (void)nrfCompatSpiTransfer(0x08U);
  const uint8_t observeTx = nrfCompatSpiTransfer(0xFFU);
  digitalWrite(CSN_PIN, HIGH);

  return (uint8_t)(observeTx & 0x0FU); // ARC_CNT
}

struct Data_Package {
  byte ch0; // potentiometre 0..100
  byte ch1; // roll 0..255
  byte ch2; // pitch 0..255
  byte ch3; // yaw 0..255
  byte ch4; // throttle 0..255
  byte ch5; // batterie TX en 0.1 V (ex: 38 = 3.8 V; RX peut ignorer)
  byte ch6; // JR auxiliary
  byte ch7; // JL auxiliary
  byte ch8; // PB RIGHT auxiliary
  byte ch9; // PB LEFT auxiliary
};

Data_Package Data;

// ---------------- RF transaction latency ----------------
// Measured around radio.write(): TX frame + possible retries + received ACK.
// Stored in microseconds; UI renders it in milliseconds without float math.
uint16_t latencyLastUs = 0;
uint16_t latencyAvgUs = 0;
uint16_t latencyMaxUs = 0;
uint8_t retryLast = 0; // ARC: actual retransmissions used by the last RF packet

void updateLatencyStats(uint16_t valueUs) {
  latencyLastUs = valueUs;
  if (latencyAvgUs == 0) latencyAvgUs = valueUs;
  else latencyAvgUs = (uint16_t)(((uint32_t)latencyAvgUs * 7UL + valueUs + 4UL) / 8UL);
  if (valueUs > latencyMaxUs) latencyMaxUs = valueUs;
}

void resetLatencyStats() {
  latencyLastUs = 0;
  latencyAvgUs = 0;
  latencyMaxUs = 0;
  retryLast = 0;
}

void makeModelAddress(uint8_t modelIndex, byte address[5]) {
  address[0] = 'R';
  address[1] = 'x';
  address[2] = '0';
  address[3] = '0';
  address[4] = (byte)('1' + modelIndex);
}

void openActiveModelPipe() {
  byte address[5];
  makeModelAddress(activeModel, address);
  radio.openWritingPipe(address);
}

void resetLinkState() {
  lastAckTime = 0;
  linkQualityAccum = 0;
  linkQualitySamples = 0;
  linkBars = 0;
  lastTelemetryTime = 0;
  rxBatteryMv = 0;
  resetLatencyStats();
}

bool bindActiveModelReceiver() {
  // Bind over the SAME normal model pipe that is already proven to work.
  // The TX probes Rx001..Rx005 to find the single powered receiver, then tells
  // it which MODEL slot it must adopt. Keep only one RX powered while binding.
  Data_Package bindData = {};
  bindData.ch0 = BIND_MAGIC0;
  bindData.ch1 = BIND_MAGIC1;
  bindData.ch2 = activeModel;

  bool bindAck = false;
  byte address[5];

  for (uint8_t probeModel = 0; probeModel < MODEL_COUNT && !bindAck; ++probeModel) {
    makeModelAddress(probeModel, address);
    radio.openWritingPipe(address);

    for (uint8_t attempt = 0; attempt < 4 && !bindAck; ++attempt) {
      bindAck = radio.write(&bindData, sizeof(bindData));
      if (!bindAck) delay(4);
    }
  }

  // The ACK for the bind frame is generated in hardware before the RX sketch
  // handles the packet. Give it time to save EEPROM and switch to its new pipe.
  openActiveModelPipe();

  bool modelAck = false;
  if (bindAck) {
    delay(20);
    for (uint8_t attempt = 0; attempt < 10 && !modelAck; ++attempt) {
      modelAck = radio.write(&Data, sizeof(Data));
      if (!modelAck) delay(5);
    }
  }

  resetLinkState();
  return bindAck && modelAck;
}

enum RfPowerLevel : uint8_t {
  RF_POWER_LOW = 0,
  RF_POWER_MID,
  RF_POWER_MAX
};

// Global transmitter RF power (shared by every model).
uint8_t rfPowerLevel = RF_POWER_LOW;

void applyRfPower() {
  if (rfPowerLevel == RF_POWER_MAX)
    radio.setPALevel(RF24_PA_MAX);
  else if (rfPowerLevel == RF_POWER_MID)
    radio.setPALevel(RF24_PA_HIGH);
  else
    radio.setPALevel(RF24_PA_LOW);
}

// ---------------- Scheduling ----------------
const unsigned long SENSOR_INTERVAL_MS   = 10;     // 100 Hz - v4.0m
const unsigned long BUTTON_INTERVAL_MS   = 5;      // 200 Hz - v4.0m
const uint32_t      SEND_INTERVAL_US     = 20000UL; // exact 50 Hz RF cadence
const unsigned long DISPLAY_INTERVAL_MS  = 50;     // 20 Hz

unsigned long lastSensorTime  = 0;
unsigned long lastButtonTime  = 0;
unsigned long lastDisplayTime = 0;
uint32_t nextSendUs = 0;

bool urgentDisplayUpdate = false;

// D2 external-interrupt handler. Four LOW-pulse ranges coexist on the wire:
//   200..2000 us : MicroView -> ATtiny alert command (generated locally, ignored here)
//   2200..2920 us: ATtiny -> MicroView command ACK/debug
//   3000..3150 us: ATtiny -> MicroView PB0/PB1 state
//   3300..10500 us: ATtiny -> MicroView TX battery in millivolts
void txBatteryPulseISR() {
  const uint16_t nowUs = (uint16_t)micros();

  // Ignore the pulse generated locally by the MicroView master.
  if (wireMasterTxActive) {
    if (!(PIND & _BV(PD2))) txBatteryPulseStartUs = nowUs;
    return;
  }

  if (PIND & _BV(PD2)) {
    const uint16_t widthUs = (uint16_t)(nowUs - txBatteryPulseStartUs);
#if WIRE_SERIAL_DIAG
    wireDiagPushISR(widthUs);
#endif

    // A long SYNC pulse starts a fresh response frame.
    if (widthUs >= WIRE_SYNC_MIN_US && widthUs <= WIRE_SYNC_MAX_US) {
      wireReplyStage = 1;
      return;
    }

    switch (wireReplyStage) {
      case 1: // raw command width measured by ATtiny
        alertRemoteMeasurePendingUs = widthUs;
        alertRemoteMeasurePulseReady = 1;
        wireReplyStage = 2;
        return;

      case 2: { // ACK command 0..9, encoded with 1 ms spacing
        int16_t relative = (int16_t)widthUs - (int16_t)ALERT_WIRE_BASE_US;
        int16_t command = (relative >= 0)
            ? (relative + (ALERT_WIRE_STEP_US / 2)) / ALERT_WIRE_STEP_US
            : -1;
        if (command >= 0 && command <= 9) {
          const uint16_t nominal = ALERT_WIRE_BASE_US + (uint16_t)command * ALERT_WIRE_STEP_US;
          const uint16_t error = (widthUs > nominal) ? (widthUs - nominal) : (nominal - widthUs);
          if (error <= ALERT_FIELD_TOLERANCE_US) {
            alertAckPendingCommand = (uint8_t)command;
            alertAckPendingWidthUs = widthUs;
            alertAckPulseReady = 1;
          }
        }
        wireReplyStage = 3;
        return;
      }

      case 3: { // actual PB0/PB1 state 0..3
        int16_t relative = (int16_t)widthUs - (int16_t)ALERT_WIRE_BASE_US;
        int16_t state = (relative >= 0)
            ? (relative + (ALERT_WIRE_STEP_US / 2)) / ALERT_WIRE_STEP_US
            : -1;
        if (state >= 0 && state <= 3) {
          const uint16_t nominal = ALERT_WIRE_BASE_US + (uint16_t)state * ALERT_WIRE_STEP_US;
          const uint16_t error = (widthUs > nominal) ? (widthUs - nominal) : (nominal - widthUs);
          if (error <= ALERT_FIELD_TOLERANCE_US) {
            alertPinStatePending = (uint8_t)state;
            alertPinStatePulseReady = 1;
          }
        }
        wireReplyStage = 4;
        return;
      }

      case 4: // battery value in mV
        if (widthUs >= TX_BATTERY_PULSE_MIN_US && widthUs <= TX_BATTERY_PULSE_MAX_US) {
          txBatteryPendingMv = widthUs;
          txBatteryPulseReady = 1;
        }
        wireReplyStage = 0;
        return;

      default:
        return;
    }
  } else {
    txBatteryPulseStartUs = nowUs;
  }
}

void updateTxBatteryInput(unsigned long now) {
  if (alertTxEchoPulseReady) {
    noInterrupts();
    const uint16_t widthUs = alertTxEchoPendingWidthUs;
    alertTxEchoPulseReady = 0;
    interrupts();
    alertTxEchoWidthUs = widthUs;
    lastAlertTxEchoTime = now;
  }

  if (alertRemoteMeasurePulseReady) {
    noInterrupts();
    const uint16_t widthUs = alertRemoteMeasurePendingUs;
    alertRemoteMeasurePulseReady = 0;
    interrupts();
    alertRemoteMeasureUs = widthUs;
    lastAlertRemoteMeasureTime = now;
    if (debugScreenEnabled && debugScreenPage == 2) urgentDisplayUpdate = true;
  }

  if (txBatteryPulseReady) {
    noInterrupts();
    const uint16_t mv = txBatteryPendingMv;
    txBatteryPulseReady = 0;
    interrupts();

    txBatteryMv = mv;
    lastTxBatteryInputTime = now;
    urgentDisplayUpdate = true;
  }

  if (alertAckPulseReady) {
    noInterrupts();
    const uint8_t command = alertAckPendingCommand;
    const uint16_t widthUs = alertAckPendingWidthUs;
    alertAckPulseReady = 0;
    interrupts();

    alertAckCommand = command;
    alertAckWidthUs = widthUs;
    lastAlertAckTime = now;
    if (debugScreenEnabled && debugScreenPage == 2) urgentDisplayUpdate = true;
  }

  if (alertPinStatePulseReady) {
    noInterrupts();
    const uint8_t state = alertPinStatePending;
    alertPinStatePulseReady = 0;
    interrupts();

    alertPinState = state & 0x03U;
    lastAlertPinStateTime = now;
    if (debugScreenEnabled && debugScreenPage == 2) urgentDisplayUpdate = true;
  }

  // If the ATtiny or wire disappears, force an empty/low indication so the
  // transmitter cannot silently keep displaying an old battery value.
  if ((unsigned long)(now - lastTxBatteryInputTime) > TX_BATTERY_INPUT_TIMEOUT_MS &&
      txBatteryMv != 0) {
    txBatteryMv = 0;
    urgentDisplayUpdate = true;
  }
}

// ---------------- Menu state ----------------
enum UiPage : uint8_t {
  UI_MAIN = 0,
  UI_MENU_ROOT,
  UI_MODEL_MENU,
  UI_MODEL_SELECT,
  UI_MODEL_RENAME,
  UI_MODEL_BIND,
  UI_AXIS_LIST,
  UI_AXIS_EDIT,
  UI_DEADBAND_EDIT,
  UI_AUX_LIST,
  UI_AUX_EDIT,
  UI_POT_DISPLAY_EDIT,
  UI_BAT_EDIT,
  UI_RF_POWER_EDIT,
  UI_DEBUG_SCREEN_EDIT,
  UI_ENDPOINT_LIST,
  UI_ENDPOINT_EDIT,
  UI_THROTTLE_MODE_EDIT,
  UI_ALERT_MENU,
  UI_ALERT_OUTPUT_EDIT,
  UI_ALERT_LEVELS,
  UI_CAL_CENTER,
  UI_CAL_RANGE,
  UI_RESET_CONFIRM
};

enum MenuStickAction : uint8_t {
  MENU_STICK_NONE = 0,
  MENU_STICK_NEXT,
  MENU_STICK_PLUS,
  MENU_STICK_MINUS
};

UiPage uiPage = UI_MAIN;
bool menuOpen = false;
bool forceMainRedraw = true;
bool comboHoldTriggered = false;
unsigned long comboHoldSince = 0;
bool suppressAuxUntilRelease = false;
bool menuIgnoreButtonsUntilRelease = false;
// Calibration moves the right joystick through its full travel. During CALIB
// it must never be interpreted as menu navigation. After leaving CALIB, keep
// navigation locked until the right stick has physically returned to center.
bool menuStickLockedUntilCenter = false;

// When DEBUG SCREEN is enabled, JR/JL presses outside the menu are
// resolved on release. Any single-button press changes debug page;
// the JL+JR combination still cancels the pending page change and opens SETTINGS.
uint8_t debugNavButton = BUTTON_NONE;
unsigned long debugNavPressSince = 0;

const uint8_t ROOT_ITEM_COUNT = 17;
uint8_t rootIndex = 0;
uint8_t modelMenuIndex = 0; // 0=SELECT, 1=RENAME, 2=BIND RX
uint8_t modelSelectIndex = MODEL01;
uint8_t bindResult = 0; // 0=ready, 1=ACK received, 2=no RX ACK
uint8_t renamePos = 0;
char renameBuffer[MODEL_NAME_LEN + 1] = {0};
uint8_t axisListIndex = 0;
uint8_t auxListIndex = 0;
uint8_t batItemIndex = 0;   // 0=MIN, 1=MAX
uint8_t batEditTarget = 0;  // 0=RX model battery, 1=TX transmitter battery
// Battery menu uses dedicated edit buffers. The live/persisted settings are
// changed only when JR validates, so BACK can simply discard the edit.
uint8_t batEditMin10 = 0;
uint8_t batEditMax10 = 0;
uint8_t editKind = EDIT_EXPO;
uint8_t endpointAxisIndex = 0;
uint8_t endpointSideIndex = 0; // 0=NEG, 1=POS
uint8_t endpointOriginalNeg = 100;
uint8_t endpointOriginalPos = 100;
uint8_t alertMenuIndex = 0;
uint8_t alertLevelIndex = 0;
uint8_t alertLevelsOriginalMask = ALERT_LEVEL_ALL;
int16_t editOriginalValue = 0;
bool resetChoice = false;

// Navigation du menu avec le joystick droit (A1=ROLL, A2=PITCH).
// Si les directions physiques sont inversees sur ton module, change simplement
// true en false sur la ligne correspondante.
const bool MENU_ROLL_RIGHT_IS_HIGH = true;
const bool MENU_PITCH_UP_IS_HIGH   = true;
const uint16_t MENU_STICK_THRESHOLD = 190; // ecart ADC autour du centre
const unsigned long MENU_REPEAT_DELAY_MS = 450;
const unsigned long MENU_REPEAT_INTERVAL_MS = 170;

uint8_t menuStickLastAction = MENU_STICK_NONE;
unsigned long menuStickActionSince = 0;
unsigned long menuStickLastRepeat = 0;

uint16_t calTempMin[AXIS_COUNT];
uint16_t calTempCenter[AXIS_COUNT];
uint16_t calTempMax[AXIS_COUNT];

// ------------------------------------------------------------
// Settings / EEPROM / model memories
// ------------------------------------------------------------
void makeDefaultModelName(char *dst, uint8_t modelIndex) {
  dst[0] = 'M';
  dst[1] = '0';
  dst[2] = (char)('1' + modelIndex);
  for (uint8_t i = 3; i <= MODEL_NAME_LEN; ++i) dst[i] = 0;
}

void setDefaultProfile(ModelProfile &p, uint8_t modelIndex) {
  makeDefaultModelName(p.friendlyName, modelIndex);
  // Default: PB LEFT / ch9 is reversed (reverse bit 7).
  // Bits 0..3 = ROLL/PITCH/YAW/THR, bits 4..7 = JR/JL/PBR/PBL.
  p.reverseMask = (1U << 7);
  p.deadband = 2;
  p.auxToggleMask = 0;
  for (uint8_t i = 0; i < AXIS_COUNT; ++i) {
    p.expo[i] = 0;
    p.dualRate[i] = 100;
    p.trim[i] = 0;
  }
}

void applyGlobalToWorking() {
  for (uint8_t i = 0; i < AXIS_COUNT; ++i) {
    settings.calMin[i] = storedSettings.calMin[i];
    settings.calCenter[i] = storedSettings.calCenter[i];
    settings.calMax[i] = storedSettings.calMax[i];
  }
}

void applyModelToWorking(uint8_t modelIndex) {
  const ModelProfile &p = storedSettings.model[modelIndex];
  settings.reverseMask = p.reverseMask;
  settings.deadband = p.deadband;
  settings.auxToggleMask = p.auxToggleMask;
  for (uint8_t i = 0; i < AXIS_COUNT; ++i) {
    settings.expo[i] = p.expo[i];
    settings.dualRate[i] = p.dualRate[i];
    settings.trim[i] = p.trim[i];
  }
}

void syncWorkingToStorage() {
  ModelProfile &p = storedSettings.model[activeModel];
  p.reverseMask = settings.reverseMask;
  p.deadband = settings.deadband;
  p.auxToggleMask = settings.auxToggleMask;
  for (uint8_t i = 0; i < AXIS_COUNT; ++i) {
    p.expo[i] = settings.expo[i];
    p.dualRate[i] = settings.dualRate[i];
    p.trim[i] = settings.trim[i];
    storedSettings.calMin[i] = settings.calMin[i];
    storedSettings.calCenter[i] = settings.calCenter[i];
    storedSettings.calMax[i] = settings.calMax[i];
  }
  storedSettings.activeModel = activeModel;
  storedSettings.potGaugeStyle = potGaugeStyle;
  storedSettings.reserved0 = SETTINGS_EXT_MARKER;
  storedSettings.rfPowerLevel = rfPowerLevel;
  storedSettings.debugScreenMarker = SETTINGS_DEBUG_MARKER;
  storedSettings.debugScreenEnabled = debugScreenEnabled ? 1 : 0;
  storedSettings.featureMarker = SETTINGS_FEATURE_MARKER;
  storedSettings.alertOutputMode = alertOutputMode;
  storedSettings.alertLevelMask = alertLevelMask ? alertLevelMask : ALERT_LEVEL_1_BIT;
}

uint16_t rxBatteryMinMv() { return (uint16_t)storedSettings.rxBatteryMin10[activeModel] * 100U; }
uint16_t rxBatteryMaxMv() { return (uint16_t)storedSettings.rxBatteryMax10[activeModel] * 100U; }
uint16_t txBatteryMinMv() { return (uint16_t)storedSettings.txBatteryMin10 * 100U; }
uint16_t txBatteryMaxMv() { return (uint16_t)storedSettings.txBatteryMax10 * 100U; }

bool telemetryFresh() {
  return lastTelemetryTime &&
         (unsigned long)(millis() - lastTelemetryTime) <= TELEMETRY_TIMEOUT_MS;
}

bool linkAckFresh() {
  return lastAckTime &&
         (unsigned long)(millis() - lastAckTime) <= LINK_LOST_TIMEOUT_MS;
}

void ensureRxLimits(uint8_t m) {
  if (storedSettings.rxBatteryMin10[m] < 10 ||
      storedSettings.rxBatteryMax10[m] <= storedSettings.rxBatteryMin10[m]) {
    storedSettings.rxBatteryMin10[m] = 60;
    storedSettings.rxBatteryMax10[m] = 84;
  }
}

void ensureTxLimits() {
  if (storedSettings.txBatteryMin10 < 10 ||
      storedSettings.txBatteryMax10 <= storedSettings.txBatteryMin10) {
    storedSettings.txBatteryMin10 = TX_BATTERY_MIN10_DEFAULT;
    storedSettings.txBatteryMax10 = TX_BATTERY_MAX10_DEFAULT;
  }
}


void setDefaultSettings() {
  storedSettings.magic = SETTINGS_MAGIC;
  storedSettings.version = SETTINGS_VERSION;
  storedSettings.activeModel = MODEL01;
  storedSettings.reserved0 = SETTINGS_EXT_MARKER;
  storedSettings.potGaugeStyle = POT_GAUGE_CLASSIC;
  potGaugeStyle = POT_GAUGE_CLASSIC;

  for (uint8_t i = 0; i < AXIS_COUNT; ++i) {
    storedSettings.calMin[i] = 0;
    storedSettings.calCenter[i] = 512;
    storedSettings.calMax[i] = 1023;
  }

  for (uint8_t m = 0; m < MODEL_COUNT; ++m) {
    setDefaultProfile(storedSettings.model[m], m);
    storedSettings.rxBatteryMin10[m] = 60;
    storedSettings.rxBatteryMax10[m] = 84;
    storedSettings.throttleMode[m] = THROTTLE_BIDIR;
    for (uint8_t a = 0; a < AXIS_COUNT; ++a) {
      storedSettings.endpointNeg[m][a] = 100;
      storedSettings.endpointPos[m][a] = 100;
    }
  }
  storedSettings.txBatteryMin10 = TX_BATTERY_MIN10_DEFAULT;
  storedSettings.txBatteryMax10 = TX_BATTERY_MAX10_DEFAULT;
  storedSettings.rfPowerLevel = RF_POWER_LOW;
  rfPowerLevel = RF_POWER_LOW;
  storedSettings.debugScreenMarker = SETTINGS_DEBUG_MARKER;
  storedSettings.debugScreenEnabled = 0;
  debugScreenEnabled = 0;
  debugScreenActive = false;
  debugScreenPage = 0;
  storedSettings.featureMarker = SETTINGS_FEATURE_MARKER;
  storedSettings.alertOutputMode = ALERT_OUTPUT_OFF;
  storedSettings.alertLevelMask = ALERT_LEVEL_ALL;
  alertOutputMode = ALERT_OUTPUT_OFF;
  alertLevelMask = ALERT_LEVEL_ALL;

  activeModel = MODEL01;
  applyGlobalToWorking();
  applyModelToWorking(activeModel);
}

void saveSettings() {
  syncWorkingToStorage();
  storedSettings.magic = SETTINGS_MAGIC;
  storedSettings.version = SETTINGS_VERSION;
  EEPROM.put(0, storedSettings); // EEPROM.put updates only bytes that changed
}

void loadSettings() {
  EEPROM.get(0, storedSettings);
  if (storedSettings.magic != SETTINGS_MAGIC ||
      storedSettings.version != SETTINGS_VERSION ||
      storedSettings.activeModel >= MODEL_COUNT) {
    setDefaultSettings();
    saveSettings();
    return;
  }

  activeModel = storedSettings.activeModel;
  ensureRxLimits(activeModel);
  ensureTxLimits();
  potGaugeStyle = (storedSettings.potGaugeStyle == POT_GAUGE_CUMULATIVE)
                    ? POT_GAUGE_CUMULATIVE : POT_GAUGE_CLASSIC;

  // Backward-compatible extension: old v4.1i EEPROM images had reserved0 = 0
  // and no rfPowerLevel byte. Preserve every existing setting and default the
  // new RF-power option to LOW on the first v4.1j boot.
  const bool needsRfPowerMigration = (storedSettings.reserved0 != SETTINGS_EXT_MARKER);
  const bool needsRfPowerRepair = (storedSettings.rfPowerLevel > RF_POWER_MAX);
  const bool needsDebugScreenRepair =
      (storedSettings.debugScreenMarker != SETTINGS_DEBUG_MARKER ||
       storedSettings.debugScreenEnabled > 1);
  if (needsRfPowerMigration || needsRfPowerRepair) {
    storedSettings.reserved0 = SETTINGS_EXT_MARKER;
    storedSettings.rfPowerLevel = RF_POWER_LOW;
  }
  if (needsDebugScreenRepair) {
    storedSettings.debugScreenMarker = SETTINGS_DEBUG_MARKER;
    storedSettings.debugScreenEnabled = 0;
  }
  bool needsFeatureRepair = (storedSettings.featureMarker != SETTINGS_FEATURE_MARKER);
  if (!needsFeatureRepair) {
    if (storedSettings.alertOutputMode > ALERT_OUTPUT_VIBRO ||
        storedSettings.alertLevelMask == 0 ||
        (storedSettings.alertLevelMask & ~ALERT_LEVEL_ALL))
      needsFeatureRepair = true;
    for (uint8_t m = 0; m < MODEL_COUNT && !needsFeatureRepair; ++m) {
      if (storedSettings.throttleMode[m] > THROTTLE_FWD_ONLY) needsFeatureRepair = true;
      for (uint8_t a = 0; a < AXIS_COUNT && !needsFeatureRepair; ++a) {
        if (storedSettings.endpointNeg[m][a] < ENDPOINT_MIN_PERCENT ||
            storedSettings.endpointNeg[m][a] > ENDPOINT_MAX_PERCENT ||
            storedSettings.endpointPos[m][a] < ENDPOINT_MIN_PERCENT ||
            storedSettings.endpointPos[m][a] > ENDPOINT_MAX_PERCENT)
          needsFeatureRepair = true;
      }
    }
  }
  if (needsFeatureRepair) {
    storedSettings.featureMarker = SETTINGS_FEATURE_MARKER;
    storedSettings.alertOutputMode = ALERT_OUTPUT_OFF;
    storedSettings.alertLevelMask = ALERT_LEVEL_ALL;
    for (uint8_t m = 0; m < MODEL_COUNT; ++m) {
      storedSettings.throttleMode[m] = THROTTLE_BIDIR;
      for (uint8_t a = 0; a < AXIS_COUNT; ++a) {
        storedSettings.endpointNeg[m][a] = 100;
        storedSettings.endpointPos[m][a] = 100;
      }
    }
  }

  rfPowerLevel = storedSettings.rfPowerLevel;
  debugScreenEnabled = storedSettings.debugScreenEnabled ? 1 : 0;
  debugScreenActive = false;
  debugScreenPage = 0;
  alertOutputMode = storedSettings.alertOutputMode;
  alertLevelMask = storedSettings.alertLevelMask ? storedSettings.alertLevelMask : ALERT_LEVEL_1_BIT;

  applyGlobalToWorking();
  applyModelToWorking(activeModel);

  if (needsRfPowerMigration || needsRfPowerRepair || needsDebugScreenRepair || needsFeatureRepair)
    EEPROM.put(0, storedSettings);
}



void selectModel(uint8_t modelIndex) {
  if (modelIndex >= MODEL_COUNT || modelIndex == activeModel) return;

  // Save the current model first, then swap only model-specific parameters.
  syncWorkingToStorage();
  activeModel = modelIndex;
  storedSettings.activeModel = activeModel;
  ensureRxLimits(activeModel);
  applyModelToWorking(activeModel);

  // Each model owns a different RF address (Rx001..Rx005). No nRF reset is
  // required: changing the writing pipe takes effect on the next transmission.
  openActiveModelPipe();
  resetLinkState();

  // Never carry an active latched AUX state from one vehicle to another.
  for (uint8_t i = 0; i < AUX_COUNT; ++i) {
    auxLatched[i] = 0;
    auxOutput[i] = 0;
  }

  saveSettings();
  forceMainRedraw = true;
  urgentDisplayUpdate = true;
}

// ------------------------------------------------------------
// ATtiny85 alert control over the existing D2/PB4 one-wire line
// ------------------------------------------------------------
uint8_t alertOutputMask() {
  if (alertOutputMode == ALERT_OUTPUT_BUZZER) return 1;
  if (alertOutputMode == ALERT_OUTPUT_VIBRO) return 2;
  if (alertOutputMode == ALERT_OUTPUT_BUZZ_VIB) return 3;
  return 0;
}

void triggerInfoAlert() {
  if (alertOutputMode != ALERT_OUTPUT_OFF && (alertLevelMask & ALERT_LEVEL_1_BIT))
    alertInfoPending = 3; // three short-wire retries; ATtiny does not restart same L1 pattern
}

uint16_t batteryWarningThresholdMv(uint16_t minMv, uint16_t maxMv) {
  if (maxMv <= minMv) return minMv;
  return minMv + (uint16_t)(((uint32_t)(maxMv - minMv) * 15UL) / 100UL);
}

uint8_t evaluateContinuousAlertLevel(unsigned long now) {
  if (alertOutputMode == ALERT_OUTPUT_OFF) return 0;

  const bool txValid = txBatteryMv != 0 &&
      (unsigned long)(now - lastTxBatteryInputTime) <= TX_BATTERY_INPUT_TIMEOUT_MS;
  const bool rxValid = telemetryFresh();
  const bool afterLinkGrace = (unsigned long)(now - systemStartMs) >= ALERT_LINK_GRACE_MS;
  const unsigned long ackAge = lastAckTime ? (unsigned long)(now - lastAckTime) : 0xFFFFFFFFUL;
  const bool linkCritical = afterLinkGrace && ackAge > ALERT_RX_CRITICAL_TIMEOUT_MS;
  const bool linkWarning = afterLinkGrace && lastAckTime &&
      ackAge > LINK_LOST_TIMEOUT_MS && ackAge <= ALERT_RX_CRITICAL_TIMEOUT_MS;

  const bool critical = linkCritical ||
      (txValid && txBatteryMv <= txBatteryMinMv()) ||
      (rxValid && rxBatteryMv <= rxBatteryMinMv());

  if (critical && (alertLevelMask & ALERT_LEVEL_3_BIT)) return 3;

  const uint16_t txWarn = batteryWarningThresholdMv(txBatteryMinMv(), txBatteryMaxMv());
  const uint16_t rxWarn = batteryWarningThresholdMv(rxBatteryMinMv(), rxBatteryMaxMv());
  const bool warning = linkWarning ||
      (txValid && txBatteryMv > txBatteryMinMv() && txBatteryMv <= txWarn) ||
      (rxValid && rxBatteryMv > rxBatteryMinMv() && rxBatteryMv <= rxWarn);

  if (warning && (alertLevelMask & ALERT_LEVEL_2_BIT)) return 2;
  return 0;
}

uint8_t alertWireCommand(uint8_t level) {
  const uint8_t mask = alertOutputMask();
  if (level == 0 || mask == 0) return 0;
  // Commands 1..9 = L1/L2/L3 x BUZZ/VIB/BOTH.
  return (uint8_t)((level - 1U) * 3U + mask);
}

bool sendAlertWireCommand(uint8_t command) {
  if (command > 9) command = 0;
  if (digitalRead(PIN_TX_BATTERY) == LOW) return false;

  const uint16_t widthUs = ALERT_WIRE_BASE_US + (uint16_t)command * ALERT_WIRE_STEP_US;
  wireReplyStage = 0;
  wireMasterTxActive = 1;
  digitalWrite(PIN_TX_BATTERY, LOW);
  pinMode(PIN_TX_BATTERY, OUTPUT);
  delayMicroseconds(widthUs);
  pinMode(PIN_TX_BATTERY, INPUT_PULLUP);
  delayMicroseconds(25);
  wireMasterTxActive = 0;

  // Local commanded width; do not depend on self-edge capture.
  alertTxEchoWidthUs = widthUs;
  lastAlertTxEchoTime = millis();
  return true;
}

void serviceAlertOutput(unsigned long now) {
#if WIRE_SERIAL_DIAG
  if (wireDiagForcedCommand <= 9) {
    if (wireDiagForcedCommand != alertLastWireCommand ||
        (unsigned long)(now - alertLastWireSendMs) >= 500UL) {
      if (sendAlertWireCommand(wireDiagForcedCommand)) {
        alertLastWireCommand = wireDiagForcedCommand;
        alertLastWireSendMs = now;
      }
    }
    return;
  }
#endif
  // OFF is a hard state, not merely "no current alert". Cancel any pending L1
  // feedback and repeatedly force command 0 so a previously active B+V state
  // cannot survive one missed pulse.
  if (alertOutputMode == ALERT_OUTPUT_OFF) {
    alertInfoPending = 0;
    alertInfoHoldUntilMs = 0;
    if (alertLastWireCommand != 0 ||
        (unsigned long)(now - alertLastWireSendMs) >= ALERT_RESEND_MS) {
      if (sendAlertWireCommand(0)) {
        alertLastWireCommand = 0;
        alertLastWireSendMs = now;
      }
    }
    return;
  }

  const uint8_t continuousLevel = evaluateContinuousAlertLevel(now);

  // Continuous warnings/critical alarms always have priority over L1 feedback.
  if (continuousLevel) alertInfoPending = 0;

  if (!continuousLevel && alertInfoPending &&
      alertOutputMode != ALERT_OUTPUT_OFF && (alertLevelMask & ALERT_LEVEL_1_BIT)) {
    const uint8_t command = alertWireCommand(1);
    if (sendAlertWireCommand(command)) {
      if (alertInfoPending) --alertInfoPending;
      alertLastWireCommand = command;
      alertLastWireSendMs = now;
      // After the final redundant L1 command, let the ATtiny finish its pulse
      // before the normal STOP state is sent.
      if (alertInfoPending == 0) alertInfoHoldUntilMs = now + 120UL;
    }
    return;
  }

  if ((long)(now - alertInfoHoldUntilMs) < 0) return;

  const uint8_t command = alertWireCommand(continuousLevel);
  if (command != alertLastWireCommand ||
      (unsigned long)(now - alertLastWireSendMs) >= ALERT_RESEND_MS) {
    if (sendAlertWireCommand(command)) {
      alertLastWireCommand = command;
      alertLastWireSendMs = now;
    }
  }
}

uint8_t friendlyNameLength(uint8_t modelIndex) {
  uint8_t len = 0;
  while (len < MODEL_NAME_LEN && storedSettings.model[modelIndex].friendlyName[len]) len++;
  while (len > 0 && storedSettings.model[modelIndex].friendlyName[len - 1] == ' ') len--;
  return len;
}

void beginModelRename() {
  for (uint8_t i = 0; i < MODEL_NAME_LEN; ++i) {
    const char c = storedSettings.model[activeModel].friendlyName[i];
    renameBuffer[i] = c ? c : ' ';
  }
  renameBuffer[MODEL_NAME_LEN] = 0;
  renamePos = 0;
  uiPage = UI_MODEL_RENAME;
  urgentDisplayUpdate = true;
}

int8_t modelCharIndex(char c) {
  for (uint8_t i = 0; i < MODEL_CHARSET_LEN; ++i)
    if (MODEL_CHARSET[i] == c) return (int8_t)i;
  return 0;
}

void changeRenameChar(int8_t direction) {
  int8_t idx = modelCharIndex(renameBuffer[renamePos]);
  idx += direction;
  if (idx < 0) idx = MODEL_CHARSET_LEN - 1;
  if (idx >= MODEL_CHARSET_LEN) idx = 0;
  renameBuffer[renamePos] = MODEL_CHARSET[idx];
}

void saveModelRename() {
  int8_t last = MODEL_NAME_LEN - 1;
  while (last >= 0 && renameBuffer[last] == ' ') last--;

  char *dst = storedSettings.model[activeModel].friendlyName;
  if (last < 0) {
    makeDefaultModelName(dst, activeModel);
  } else {
    uint8_t i = 0;
    for (; i <= (uint8_t)last; ++i) dst[i] = renameBuffer[i];
    dst[i] = 0;
    for (++i; i <= MODEL_NAME_LEN; ++i) dst[i] = 0;
  }
  saveSettings();
  forceMainRedraw = true;
}

// ------------------------------------------------------------
// ADC / filtering
// ------------------------------------------------------------
// v4.0m: keep one discarded conversion after each ADC mux change, but use
// only two useful samples. This cuts the analogRead workload by roughly 4x
// versus v4.0l while still oversampling every control before each 50 Hz RF frame.
uint16_t readAnalogAverage(uint8_t pin) {
  analogRead(pin); // discard first conversion after mux change
  uint16_t sum = 0;
  for (uint8_t i = 0; i < ADC_SAMPLES; ++i)
    sum += analogRead(pin);
  return (sum + (ADC_SAMPLES / 2)) / ADC_SAMPLES;
}

void initializeFilters() {
  for (uint8_t i = 0; i < CHANNEL_COUNT; ++i) {
    const uint16_t sample = readAnalogAverage(SENSOR_PINS[i]);
    sensorValue[i] = sample;
    filterStateQ4[i] = ((int32_t)sample) << 4;
  }
}

void updateCalibrationRange() {
  if (uiPage != UI_CAL_RANGE) return;

  for (uint8_t axis = 0; axis < AXIS_COUNT; ++axis) {
    const uint16_t v = sensorValue[AXIS_TO_CHANNEL[axis]];
    if (v < calTempMin[axis]) calTempMin[axis] = v;
    if (v > calTempMax[axis]) calTempMax[axis] = v;
  }
}

void get_data() {
  for (uint8_t i = 0; i < CHANNEL_COUNT; ++i) {
    const uint16_t raw = readAnalogAverage(SENSOR_PINS[i]);
    const int32_t targetQ4 = ((int32_t)raw) << 4;
    filterStateQ4[i] += (targetQ4 - filterStateQ4[i]) / FILTER_DIVISOR;
    const uint16_t filtered = (uint16_t)((filterStateQ4[i] + 8) >> 4);
    const int16_t delta = (int16_t)filtered - (int16_t)sensorValue[i];

    if (delta >= ADC_DEADBAND || delta <= -ADC_DEADBAND)
      sensorValue[i] = filtered;
  }

  updateCalibrationRange();
}

// ------------------------------------------------------------
// Control processing : calibration -> deadband -> expo -> D/R -> trim -> reverse
// ------------------------------------------------------------
int16_t clamp1000(int32_t v) {
  if (v > 1000) return 1000;
  if (v < -1000) return -1000;
  return (int16_t)v;
}

int16_t normalizeAxis(uint8_t axis, uint16_t raw) {
  const uint16_t mn = settings.calMin[axis];
  const uint16_t ct = settings.calCenter[axis];
  const uint16_t mx = settings.calMax[axis];

  if (raw >= ct) {
    uint16_t span = mx - ct;
    if (span < 50) span = 511;
    return clamp1000(((int32_t)(raw - ct) * 1000L) / span);
  }

  uint16_t span = ct - mn;
  if (span < 50) span = 512;
  return clamp1000(-((int32_t)(ct - raw) * 1000L) / span);
}

int16_t applyDeadband(int16_t x) {
  const int16_t db = (int16_t)settings.deadband * 10; // percent -> -1000..1000 units
  if (db <= 0) return x;

  const int16_t a = (x < 0) ? -x : x;
  if (a <= db) return 0;

  const int16_t sign = (x < 0) ? -1 : 1;
  const int32_t scaled = ((int32_t)(a - db) * 1000L) / (1000 - db);
  return (int16_t)(sign * scaled);
}

int16_t applyExpo(int16_t x, uint8_t expoPercent) {
  if (expoPercent == 0) return x;

  // x is -1000..1000. Keep all intermediate values inside int32_t.
  const int32_t x2 = ((int32_t)x * x) / 1000L;
  const int32_t x3 = (x2 * x) / 1000L;
  return (int16_t)(((int32_t)(100 - expoPercent) * x +
                    (int32_t)expoPercent * x3) / 100L);
}

int16_t clamp1250(int32_t v) {
  if (v > 1250) return 1250;
  if (v < -1250) return -1250;
  return (int16_t)v;
}

int16_t processAxis(uint8_t axis, uint16_t raw) {
  int16_t x = applyDeadband(normalizeAxis(axis, raw));

  // FWD ONLY is intentionally asymmetric. The complete lower half of the
  // spring-centred throttle stick means STOP. The upper half is stretched
  // across the full ESC range. Reverse and trim are deliberately ignored.
  if (axis == 3 && storedSettings.throttleMode[activeModel] == THROTTLE_FWD_ONLY) {
    if (x <= 0) return -1000;

    x = applyExpo(x, settings.expo[axis]);
    if (x < 0) x = 0;
    if (x > 1000) x = 1000;

    const int16_t maxOut = (int16_t)storedSettings.endpointPos[activeModel][axis] * 10;
    // x: 0..1000 becomes output: -1000..maxOut. At 100% endpoint this is
    // exactly 1000..2000 us on the paired v3.19 receiver.
    return clamp1250(-1000L + ((int32_t)x * (1000L + maxOut)) / 1000L);
  }

  x = applyExpo(x, settings.expo[axis]);
  x = (int16_t)(((int32_t)x * settings.dualRate[axis]) / 100L);
  x = clamp1000((int32_t)x + (int32_t)settings.trim[axis] * 10L);

  if (settings.reverseMask & (1 << axis))
    x = -x;

  // Endpoint is applied AFTER reverse, so NEG/POS always describe the final
  // physical output direction. 100% preserves the historical 1000..2000 us.
  const uint8_t ep = (x < 0) ? storedSettings.endpointNeg[activeModel][axis]
                             : storedSettings.endpointPos[activeModel][axis];
  return clamp1250(((int32_t)x * ep) / 100L);
}

byte encodeExtendedAxisValue(int16_t value) {
  value = clamp1250(value);
  return (byte)(((int32_t)(value + 1250) * 255L + 1250L) / 2500L);
}

byte axisToByte(uint8_t axis) {
  const uint8_t ch = AXIS_TO_CHANNEL[axis];
  return encodeExtendedAxisValue(processAxis(axis, sensorValue[ch]));
}

// ------------------------------------------------------------
// Buttons / AUX
// ------------------------------------------------------------
uint8_t decodeButton(uint16_t value) {
  // MENU COMBO must be tested before JL because the estimated double-button
  // value is only slightly below the measured JL value.
  if (value >= MENU_COMBO_MIN && value <= MENU_COMBO_MAX)
    return BUTTON_MENU_COMBO;

  if (value >= BUTTON_NONE_MIN)
    return BUTTON_NONE;

  if (value >= BUTTON_LEFT_MIN && value <= BUTTON_LEFT_MAX)
    return BUTTON_LEFT;

  if (value >= BUTTON_JR_MIN && value <= BUTTON_JR_MAX)
    return BUTTON_JOYSTICK_RIGHT;

  if (value >= BUTTON_RIGHT_MIN && value <= BUTTON_RIGHT_MAX)
    return BUTTON_RIGHT;

  if (value >= BUTTON_JL_MIN && value <= BUTTON_JL_MAX)
    return BUTTON_JOYSTICK_LEFT;

  return BUTTON_NONE;
}

int8_t buttonToAux(uint8_t id) {
  if (id == BUTTON_JOYSTICK_RIGHT) return AUX_JR;
  if (id == BUTTON_JOYSTICK_LEFT)  return AUX_JL;
  if (id == BUTTON_RIGHT)          return AUX_RIGHT;
  if (id == BUTTON_LEFT)           return AUX_LEFT;
  return -1;
}

bool auxIsToggle(uint8_t aux) {
  return (settings.auxToggleMask & (1 << aux)) != 0;
}

// Reverse bits 0..3 = ROLL/PITCH/YAW/THR, bits 4..7 = JR/JL/PBR/PBL.
bool auxIsReversed(uint8_t aux) {
  return (settings.reverseMask & (1 << (4 + aux))) != 0;
}

uint8_t auxEffectiveOutput(uint8_t aux) {
  return auxOutput[aux] ^ (auxIsReversed(aux) ? 1 : 0);
}

void clearMomentaryAux() {
  for (uint8_t i = 0; i < AUX_COUNT; ++i) {
    if (!auxIsToggle(i)) auxOutput[i] = 0;
  }
}

void pressAux(uint8_t buttonId, unsigned long now) {
  const int8_t aux = buttonToAux(buttonId);
  if (aux < 0) return;

  if (auxIsToggle((uint8_t)aux)) {
    lastAuxPressId = buttonId;
    lastAuxPressTime = now;
    lastAuxPreviousLatch = auxLatched[aux];
    lastAuxPressWasToggle = true;

    auxLatched[aux] ^= 1;
    auxOutput[aux] = auxLatched[aux];
  } else {
    lastAuxPressId = buttonId;
    lastAuxPressTime = now;
    lastAuxPressWasToggle = false;
    auxOutput[aux] = 1;
  }

  urgentDisplayUpdate = true;
}

void releaseAux(uint8_t buttonId) {
  const int8_t aux = buttonToAux(buttonId);
  if (aux < 0) return;

  if (!auxIsToggle((uint8_t)aux)) {
    auxOutput[aux] = 0;
      urgentDisplayUpdate = true;
  }
}

void enterMenuCombo(unsigned long now, uint8_t previousButton) {
  // Release any momentary AUX immediately.
  clearMomentaryAux();

  // If JL or JR was momentarily decoded just before the true combination,
  // undo a fresh toggle so opening the menu does not change an AUX state.
  if (lastAuxPressWasToggle &&
      lastAuxPressId == previousButton &&
      (unsigned long)(now - lastAuxPressTime) <= COMBO_ROLLBACK_MS) {
    const int8_t aux = buttonToAux(previousButton);
    if (aux >= 0) {
      auxLatched[aux] = lastAuxPreviousLatch;
      auxOutput[aux] = auxLatched[aux];
    }
  }

  comboHoldSince = now;
  comboHoldTriggered = false;
  urgentDisplayUpdate = true;
}

// ------------------------------------------------------------
// Menu helpers
// ------------------------------------------------------------
void openMenu() {
  menuOpen = true;
  uiPage = UI_MENU_ROOT;
  rootIndex = 0;
  comboHoldTriggered = true;
  suppressAuxUntilRelease = true;
  menuIgnoreButtonsUntilRelease = true;
  menuStickLastAction = MENU_STICK_NONE;
  debugNavButton = BUTTON_NONE;
  debugNavPressSince = 0;
  debugScreenActive = false;
  debugScreenPage = 0;
  clearMomentaryAux();
  uView.clear(PAGE);
  urgentDisplayUpdate = true;
}

void closeMenu() {
  menuOpen = false;
  uiPage = UI_MAIN;
  suppressAuxUntilRelease = true;
  menuIgnoreButtonsUntilRelease = false;
  menuStickLastAction = MENU_STICK_NONE;
  debugScreenActive = false;
  debugScreenPage = 0;
  forceMainRedraw = true;
  uView.clear(PAGE);
  urgentDisplayUpdate = true;
}

void controlName(uint8_t control) {
  if (control == 0) uView.print(F("ROLL"));
  else if (control == 1) uView.print(F("PITCH"));
  else if (control == 2) uView.print(F("YAW"));
  else if (control == 3) uView.print(F("THR"));
  else if (control == 4) uView.print(PFSTR(S_JR));
  else if (control == 5) uView.print(PFSTR(S_JL));
  else if (control == 6) uView.print(PFSTR(S_PBR));
  else uView.print(PFSTR(S_PBL));
}

void rootLabel(uint8_t item) {
  switch (item) {
    case 0: uView.print(PFSTR(S_MODEL)); break;
    case 1: uView.print(PFSTR(S_EXPO)); break;
    case 2: uView.print(PFSTR(S_DR)); break;
    case 3: uView.print(PFSTR(S_ENDPOINT)); break;
    case 4: uView.print(PFSTR(S_REVERSE)); break;
    case 5: uView.print(PFSTR(S_TRIM)); break;
    case 6: uView.print(PFSTR(S_THR_MODE)); break;
    case 7: uView.print(PFSTR(S_DEADBAND)); break;
    case 8: uView.print(PFSTR(S_AUX_MODE)); break;
    case 9: uView.print(PFSTR(S_POT_DISP)); break;
    case 10: uView.print(PFSTR(S_RX_BAT)); break;
    case 11: uView.print(PFSTR(S_TX_BAT)); break;
    case 12: uView.print(PFSTR(S_RF_POWER)); break;
    case 13: uView.print(PFSTR(S_DEBUG_SCR)); break;
    case 14: uView.print(PFSTR(S_ALERTS)); break;
    case 15: uView.print(PFSTR(S_CALIB)); break;
    case 16: uView.print(F("RESET")); break;
    default: uView.print('?'); break;
  }
}

void beginAxisEdit(uint8_t kind) {
  editKind = kind;
  axisListIndex = 0;
  uiPage = UI_AXIS_LIST;
  urgentDisplayUpdate = true;
}

int16_t currentEditValue() {
  const uint8_t a = axisListIndex;
  if (editKind == EDIT_EXPO) return settings.expo[a];
  if (editKind == EDIT_DUALRATE) return settings.dualRate[a];
  if (editKind == EDIT_REVERSE) {
    if (a == 3 && storedSettings.throttleMode[activeModel] == THROTTLE_FWD_ONLY) return 0;
    return (settings.reverseMask & (1 << a)) ? 1 : 0;
  }
  return settings.trim[a];
}

void restoreEditValue() {
  const uint8_t a = axisListIndex;
  if (editKind == EDIT_EXPO) settings.expo[a] = (uint8_t)editOriginalValue;
  else if (editKind == EDIT_DUALRATE) settings.dualRate[a] = (uint8_t)editOriginalValue;
  else if (editKind == EDIT_REVERSE) {
    if (editOriginalValue) settings.reverseMask |= (1 << a);
    else settings.reverseMask &= ~(1 << a);
  } else settings.trim[a] = (int8_t)editOriginalValue;
}

void modifyAxisEdit(int8_t direction) {
  const uint8_t a = axisListIndex;

  if (editKind == EDIT_EXPO) {
    int16_t v = settings.expo[a] + direction * 5;
    if (v < 0) v = 0;
    if (v > 70) v = 70;
    settings.expo[a] = (uint8_t)v;
  } else if (editKind == EDIT_DUALRATE) {
    int16_t v = settings.dualRate[a] + direction * 5;
    if (v < 50) v = 50;
    if (v > 100) v = 100;
    settings.dualRate[a] = (uint8_t)v;
  } else if (editKind == EDIT_REVERSE) {
    if (a == 3 && storedSettings.throttleMode[activeModel] == THROTTLE_FWD_ONLY) {
      settings.reverseMask &= ~(1 << a);
      return;
    }
    if (direction > 0) settings.reverseMask |= (1 << a);
    else               settings.reverseMask &= ~(1 << a);
  } else {
    int16_t v = settings.trim[a] + direction;
    if (v < -20) v = -20;
    if (v > 20) v = 20;
    settings.trim[a] = (int8_t)v;
  }
}

void captureCalibration(uint8_t nextPage) {
  for (uint8_t a = 0; a < AXIS_COUNT; ++a) {
    const uint16_t v = sensorValue[AXIS_TO_CHANNEL[a]];
    calTempCenter[a] = v;
    calTempMin[a] = v;
    calTempMax[a] = v;
  }
  uiPage = (UiPage)nextPage;
  urgentDisplayUpdate = true;
}

void saveCalibration() {
  for (uint8_t a = 0; a < AXIS_COUNT; ++a) {
    const uint16_t c = calTempCenter[a];
    // Only accept an axis if both sides have enough travel.
    if (calTempMin[a] + 100 < c && calTempMax[a] > c + 100) {
      settings.calMin[a] = calTempMin[a];
      settings.calCenter[a] = c;
      settings.calMax[a] = calTempMax[a];
    }
  }
  saveSettings();
  uiPage = UI_MENU_ROOT;
  urgentDisplayUpdate = true;
}

void handleRootSelect() {
  switch (rootIndex) {
    case 0:
      modelMenuIndex = 0;
      uiPage = UI_MODEL_MENU;
      break;
    case 1: beginAxisEdit(EDIT_EXPO); break;
    case 2: beginAxisEdit(EDIT_DUALRATE); break;
    case 3:
      endpointAxisIndex = 0;
      uiPage = UI_ENDPOINT_LIST;
      break;
    case 4: beginAxisEdit(EDIT_REVERSE); break;
    case 5: beginAxisEdit(EDIT_TRIM); break;
    case 6:
      editOriginalValue = storedSettings.throttleMode[activeModel];
      uiPage = UI_THROTTLE_MODE_EDIT;
      break;
    case 7:
      editOriginalValue = settings.deadband;
      uiPage = UI_DEADBAND_EDIT;
      break;
    case 8:
      auxListIndex = 0;
      uiPage = UI_AUX_LIST;
      break;
    case 9:
      editOriginalValue = potGaugeStyle;
      uiPage = UI_POT_DISPLAY_EDIT;
      break;
    case 10:
      batEditTarget = 0;
      batItemIndex = 0;
      batEditMin10 = storedSettings.rxBatteryMin10[activeModel];
      batEditMax10 = storedSettings.rxBatteryMax10[activeModel];
      uiPage = UI_BAT_EDIT;
      break;
    case 11:
      batEditTarget = 1;
      batItemIndex = 0;
      batEditMin10 = storedSettings.txBatteryMin10;
      batEditMax10 = storedSettings.txBatteryMax10;
      uiPage = UI_BAT_EDIT;
      break;
    case 12:
      editOriginalValue = rfPowerLevel;
      uiPage = UI_RF_POWER_EDIT;
      break;
    case 13:
      editOriginalValue = debugScreenEnabled;
      uiPage = UI_DEBUG_SCREEN_EDIT;
      break;
    case 14:
      alertMenuIndex = 0;
      uiPage = UI_ALERT_MENU;
      break;
    case 15: captureCalibration(UI_CAL_CENTER); break;
    case 16:
      resetChoice = false;
      uiPage = UI_RESET_CONFIRM;
      break;
  }
  urgentDisplayUpdate = true;
}

void menuBackOneLevel();

void restoreAuxEditValue() {
  if (editOriginalValue) settings.auxToggleMask |= (1 << auxListIndex);
  else                   settings.auxToggleMask &= ~(1 << auxListIndex);
}

void menuButtonPress(uint8_t id) {
  // JL is the dedicated BACK/CANCEL key everywhere in the menu.
  // During calibration it also locks stick navigation until the stick
  // physically returns to center.
  if (id == BUTTON_JOYSTICK_LEFT) {
    if (uiPage == UI_CAL_CENTER || uiPage == UI_CAL_RANGE) {
      uiPage = UI_MENU_ROOT;
      menuStickLockedUntilCenter = true;
      urgentDisplayUpdate = true;
    } else {
      menuBackOneLevel();
    }
    return;
  }

  // Calibration exception: PB LEFT captures/saves without touching the stick.
  if (id == BUTTON_LEFT) {
    if (uiPage == UI_CAL_CENTER) captureCalibration(UI_CAL_RANGE);
    else if (uiPage == UI_CAL_RANGE) saveCalibration();
    urgentDisplayUpdate = true;
    return;
  }

  if (id != BUTTON_JOYSTICK_RIGHT) return;

  // JR validates / saves the current choice or edited value.
  if (uiPage == UI_MODEL_SELECT) {
    selectModel(modelSelectIndex);
    uiPage = UI_MODEL_MENU;
  }
  else if (uiPage == UI_MODEL_RENAME) {
    saveModelRename();
    uiPage = UI_MODEL_MENU;
  }
  else if (uiPage == UI_MODEL_BIND) {
    bindResult = bindActiveModelReceiver() ? 1 : 2;
    forceMainRedraw = true;
  }
  else if (uiPage == UI_AXIS_EDIT) {
    saveSettings();
    uiPage = UI_AXIS_LIST;
  }
  else if (uiPage == UI_ENDPOINT_EDIT) {
    saveSettings();
    uiPage = UI_ENDPOINT_LIST;
  }
  else if (uiPage == UI_THROTTLE_MODE_EDIT) {
    if (storedSettings.throttleMode[activeModel] == THROTTLE_FWD_ONLY) {
      settings.reverseMask &= ~(1 << 3);
      settings.trim[3] = 0;
    }
    saveSettings();
    uiPage = UI_MENU_ROOT;
  }
  else if (uiPage == UI_ALERT_OUTPUT_EDIT) {
    saveSettings();
    alertLastWireCommand = 0xFF;
    uiPage = UI_ALERT_MENU;
  }
  else if (uiPage == UI_ALERT_LEVELS) {
    if (alertLevelMask == 0) alertLevelMask = ALERT_LEVEL_1_BIT;
    saveSettings();
    alertLastWireCommand = 0xFF;
    uiPage = UI_ALERT_MENU;
  }
  else if (uiPage == UI_DEADBAND_EDIT) {
    saveSettings();
    uiPage = UI_MENU_ROOT;
  }
  else if (uiPage == UI_AUX_EDIT) {
    saveSettings();
    if (!auxIsToggle(auxListIndex)) {
      auxLatched[auxListIndex] = 0;
      auxOutput[auxListIndex] = 0;
    }
      uiPage = UI_AUX_LIST;
  }
  else if (uiPage == UI_POT_DISPLAY_EDIT) {
    saveSettings();
    uiPage = UI_MENU_ROOT;
    forceMainRedraw = true;
  }
  else if (uiPage == UI_BAT_EDIT) {
    // Commit the edit buffer only on JR validation.
    if (batEditTarget) {
      storedSettings.txBatteryMin10 = batEditMin10;
      storedSettings.txBatteryMax10 = batEditMax10;
    } else {
      storedSettings.rxBatteryMin10[activeModel] = batEditMin10;
      storedSettings.rxBatteryMax10[activeModel] = batEditMax10;
    }
    saveSettings();
    uiPage = UI_MENU_ROOT;
    forceMainRedraw = true;
  }
  else if (uiPage == UI_RF_POWER_EDIT) {
    saveSettings();
    applyRfPower();
    uiPage = UI_MENU_ROOT;
    forceMainRedraw = true;
  }
  else if (uiPage == UI_DEBUG_SCREEN_EDIT) {
    saveSettings();
    if (!debugScreenEnabled) debugScreenActive = false;
  debugScreenPage = 0;
    uiPage = UI_MENU_ROOT;
    forceMainRedraw = true;
  }
  else if (uiPage == UI_RESET_CONFIRM) {
    if (resetChoice) {
      setDefaultSettings();
      saveSettings();
      applyRfPower();
      openActiveModelPipe();
      resetLinkState();
        for (uint8_t i = 0; i < AUX_COUNT; ++i) {
        auxLatched[i] = 0;
        auxOutput[i] = 0;
      }
          forceMainRedraw = true;
    }
    uiPage = UI_MENU_ROOT;
  }

  urgentDisplayUpdate = true;
}

uint8_t readMenuStickDirection() {
  const int16_t rollDelta = (int16_t)sensorValue[CH_ROLL] -
                            (int16_t)settings.calCenter[0];
  const int16_t pitchDelta = (int16_t)sensorValue[CH_PITCH] -
                             (int16_t)settings.calCenter[1];

  const uint16_t rollAbs = (rollDelta < 0) ? (uint16_t)(-rollDelta) : (uint16_t)rollDelta;
  const uint16_t pitchAbs = (pitchDelta < 0) ? (uint16_t)(-pitchDelta) : (uint16_t)pitchDelta;

  // Take the dominant axis only; diagonal stick positions therefore generate
  // one unambiguous menu command.
  if (rollAbs >= pitchAbs) {
    if (rollAbs < MENU_STICK_THRESHOLD) return MENU_STICK_NONE;
    bool right = (rollDelta > 0);
    if (!MENU_ROLL_RIGHT_IS_HIGH) right = !right;
    // Right enters/advances. Left is deliberately ignored; JL is BACK/CANCEL.
    return right ? MENU_STICK_NEXT : MENU_STICK_NONE;
  }

  if (pitchAbs < MENU_STICK_THRESHOLD) return MENU_STICK_NONE;
  bool up = (pitchDelta > 0);
  if (!MENU_PITCH_UP_IS_HIGH) up = !up;
  return up ? MENU_STICK_PLUS : MENU_STICK_MINUS;
}

void menuEnterCurrentItem() {
  if (uiPage == UI_MENU_ROOT) {
    handleRootSelect();
  }
  else if (uiPage == UI_MODEL_MENU) {
    if (modelMenuIndex == 0) {
      modelSelectIndex = activeModel;
      uiPage = UI_MODEL_SELECT;
    } else if (modelMenuIndex == 1) {
      beginModelRename();
    } else {
      bindResult = 0;
      uiPage = UI_MODEL_BIND;
    }
  }
  else if (uiPage == UI_MODEL_RENAME) {
    // Right advances through the six character positions and wraps around.
    renamePos = (renamePos + 1) % MODEL_NAME_LEN;
  }
  else if (uiPage == UI_AXIS_LIST) {
    editOriginalValue = currentEditValue();
    uiPage = UI_AXIS_EDIT;
  }
  else if (uiPage == UI_ENDPOINT_LIST) {
    endpointOriginalNeg = storedSettings.endpointNeg[activeModel][endpointAxisIndex];
    endpointOriginalPos = storedSettings.endpointPos[activeModel][endpointAxisIndex];
    endpointSideIndex = (endpointAxisIndex == 3 &&
                         storedSettings.throttleMode[activeModel] == THROTTLE_FWD_ONLY) ? 1 : 0;
    uiPage = UI_ENDPOINT_EDIT;
  }
  else if (uiPage == UI_ENDPOINT_EDIT) {
    if (!(endpointAxisIndex == 3 &&
          storedSettings.throttleMode[activeModel] == THROTTLE_FWD_ONLY))
      endpointSideIndex ^= 1;
  }
  else if (uiPage == UI_ALERT_MENU) {
    if (alertMenuIndex == 0) {
      editOriginalValue = alertOutputMode;
      uiPage = UI_ALERT_OUTPUT_EDIT;
    } else {
      alertLevelIndex = 0;
      alertLevelsOriginalMask = alertLevelMask;
      uiPage = UI_ALERT_LEVELS;
    }
  }
  else if (uiPage == UI_ALERT_LEVELS) {
    const uint8_t bit = (uint8_t)(1U << alertLevelIndex);
    const uint8_t candidate = alertLevelMask ^ bit;
    if (candidate != 0) alertLevelMask = candidate;
  }
  else if (uiPage == UI_AUX_LIST) {
    editOriginalValue = auxIsToggle(auxListIndex) ? 1 : 0;
    uiPage = UI_AUX_EDIT;
  }
  else if (uiPage == UI_BAT_EDIT) {
    batItemIndex ^= 1;
  }
  urgentDisplayUpdate = true;
}

void menuBackOneLevel() {
  if (uiPage == UI_MENU_ROOT) {
    closeMenu();
    return;
  }

  if (uiPage == UI_MODEL_MENU) {
    uiPage = UI_MENU_ROOT;
  }
  else if (uiPage == UI_MODEL_SELECT || uiPage == UI_MODEL_RENAME ||
           uiPage == UI_MODEL_BIND) {
    uiPage = UI_MODEL_MENU;
  }
  else if (uiPage == UI_AXIS_EDIT) {
    restoreEditValue();
    uiPage = UI_AXIS_LIST;
  }
  else if (uiPage == UI_ENDPOINT_EDIT) {
    storedSettings.endpointNeg[activeModel][endpointAxisIndex] = endpointOriginalNeg;
    storedSettings.endpointPos[activeModel][endpointAxisIndex] = endpointOriginalPos;
    uiPage = UI_ENDPOINT_LIST;
  }
  else if (uiPage == UI_THROTTLE_MODE_EDIT) {
    storedSettings.throttleMode[activeModel] = (uint8_t)editOriginalValue;
    uiPage = UI_MENU_ROOT;
  }
  else if (uiPage == UI_ALERT_OUTPUT_EDIT) {
    alertOutputMode = (uint8_t)editOriginalValue;
    uiPage = UI_ALERT_MENU;
  }
  else if (uiPage == UI_ALERT_LEVELS) {
    alertLevelMask = alertLevelsOriginalMask;
    uiPage = UI_ALERT_MENU;
  }
  else if (uiPage == UI_DEADBAND_EDIT) {
    settings.deadband = (uint8_t)editOriginalValue;
    uiPage = UI_MENU_ROOT;
  }
  else if (uiPage == UI_AUX_EDIT) {
    restoreAuxEditValue();
    uiPage = UI_AUX_LIST;
  }
  else if (uiPage == UI_POT_DISPLAY_EDIT) {
    potGaugeStyle = (uint8_t)editOriginalValue;
    uiPage = UI_MENU_ROOT;
  }
  else if (uiPage == UI_BAT_EDIT) {
    // Dedicated edit buffer: BACK discards changes; persisted values stay intact.
    uiPage = UI_MENU_ROOT;
  }
  else if (uiPage == UI_RF_POWER_EDIT) {
    rfPowerLevel = (uint8_t)editOriginalValue;
    uiPage = UI_MENU_ROOT;
  }
  else if (uiPage == UI_DEBUG_SCREEN_EDIT) {
    debugScreenEnabled = (uint8_t)editOriginalValue;
    if (!debugScreenEnabled) debugScreenActive = false;
  debugScreenPage = 0;
    uiPage = UI_MENU_ROOT;
  }
  else if (uiPage == UI_AXIS_LIST || uiPage == UI_ENDPOINT_LIST ||
           uiPage == UI_AUX_LIST || uiPage == UI_ALERT_MENU ||
           uiPage == UI_CAL_CENTER || uiPage == UI_CAL_RANGE ||
           uiPage == UI_RESET_CONFIRM) {
    uiPage = UI_MENU_ROOT;
  }

  urgentDisplayUpdate = true;
}

void menuStickAction(uint8_t action) {
  if (action == MENU_STICK_NONE) return;
  triggerInfoAlert();

  // Right navigation is one-shot until the stick returns to center.
  // Left movement is ignored; JL is the only BACK/CANCEL command.
  if (action == MENU_STICK_NEXT) {
    menuEnterCurrentItem();
    return;
  }

  // Vertical: choose an item or modify the current value.
  if (uiPage == UI_MENU_ROOT) {
    if (action == MENU_STICK_PLUS)
      rootIndex = (rootIndex == 0) ? (ROOT_ITEM_COUNT - 1) : rootIndex - 1;
    else if (action == MENU_STICK_MINUS)
      rootIndex = (rootIndex + 1 >= ROOT_ITEM_COUNT) ? 0 : rootIndex + 1;
  }
  else if (uiPage == UI_MODEL_MENU) {
    if (action == MENU_STICK_PLUS)
      modelMenuIndex = (modelMenuIndex == 0) ? 2 : modelMenuIndex - 1;
    else if (action == MENU_STICK_MINUS)
      modelMenuIndex = (modelMenuIndex >= 2) ? 0 : modelMenuIndex + 1;
  }
  else if (uiPage == UI_MODEL_SELECT) {
    if (action == MENU_STICK_PLUS)
      modelSelectIndex = (modelSelectIndex == 0) ? (MODEL_COUNT - 1) : modelSelectIndex - 1;
    else if (action == MENU_STICK_MINUS)
      modelSelectIndex = (modelSelectIndex + 1 >= MODEL_COUNT) ? 0 : modelSelectIndex + 1;
  }
  else if (uiPage == UI_MODEL_RENAME) {
    if (action == MENU_STICK_PLUS) changeRenameChar(+1);
    else if (action == MENU_STICK_MINUS) changeRenameChar(-1);
  }
  else if (uiPage == UI_AXIS_LIST) {
    const uint8_t last = (editKind == EDIT_REVERSE) ? 7 : 3;
    if (action == MENU_STICK_PLUS)
      axisListIndex = (axisListIndex == 0) ? last : axisListIndex - 1;
    else if (action == MENU_STICK_MINUS)
      axisListIndex = (axisListIndex >= last) ? 0 : axisListIndex + 1;
  }
  else if (uiPage == UI_AXIS_EDIT) {
    if (action == MENU_STICK_PLUS) modifyAxisEdit(+1);
    else if (action == MENU_STICK_MINUS) modifyAxisEdit(-1);
  }
  else if (uiPage == UI_ENDPOINT_LIST) {
    if (action == MENU_STICK_PLUS)
      endpointAxisIndex = (endpointAxisIndex == 0) ? 3 : endpointAxisIndex - 1;
    else if (action == MENU_STICK_MINUS)
      endpointAxisIndex = (endpointAxisIndex >= 3) ? 0 : endpointAxisIndex + 1;
  }
  else if (uiPage == UI_ENDPOINT_EDIT) {
    uint8_t &v = endpointSideIndex ? storedSettings.endpointPos[activeModel][endpointAxisIndex]
                                   : storedSettings.endpointNeg[activeModel][endpointAxisIndex];
    if (action == MENU_STICK_PLUS && v < ENDPOINT_MAX_PERCENT) {
      v = (uint8_t)((v + 5 > ENDPOINT_MAX_PERCENT) ? ENDPOINT_MAX_PERCENT : v + 5);
    } else if (action == MENU_STICK_MINUS && v > ENDPOINT_MIN_PERCENT) {
      v = (uint8_t)((v < ENDPOINT_MIN_PERCENT + 5) ? ENDPOINT_MIN_PERCENT : v - 5);
    }
  }
  else if (uiPage == UI_THROTTLE_MODE_EDIT) {
    if (action == MENU_STICK_PLUS || action == MENU_STICK_MINUS)
      storedSettings.throttleMode[activeModel] ^= 1;
  }
  else if (uiPage == UI_ALERT_MENU) {
    if (action == MENU_STICK_PLUS || action == MENU_STICK_MINUS) alertMenuIndex ^= 1;
  }
  else if (uiPage == UI_ALERT_OUTPUT_EDIT) {
    if (action == MENU_STICK_PLUS)
      alertOutputMode = (alertOutputMode >= ALERT_OUTPUT_VIBRO) ? ALERT_OUTPUT_OFF : alertOutputMode + 1;
    else if (action == MENU_STICK_MINUS)
      alertOutputMode = (alertOutputMode == ALERT_OUTPUT_OFF) ? ALERT_OUTPUT_VIBRO : alertOutputMode - 1;
  }
  else if (uiPage == UI_ALERT_LEVELS) {
    if (action == MENU_STICK_PLUS)
      alertLevelIndex = (alertLevelIndex == 0) ? 2 : alertLevelIndex - 1;
    else if (action == MENU_STICK_MINUS)
      alertLevelIndex = (alertLevelIndex >= 2) ? 0 : alertLevelIndex + 1;
  }
  else if (uiPage == UI_DEADBAND_EDIT) {
    if (action == MENU_STICK_PLUS && settings.deadband < 10) settings.deadband++;
    else if (action == MENU_STICK_MINUS && settings.deadband > 0) settings.deadband--;
  }
  else if (uiPage == UI_AUX_LIST) {
    if (action == MENU_STICK_PLUS)
      auxListIndex = (auxListIndex == 0) ? 3 : auxListIndex - 1;
    else if (action == MENU_STICK_MINUS)
      auxListIndex = (auxListIndex >= 3) ? 0 : auxListIndex + 1;
  }
  else if (uiPage == UI_AUX_EDIT) {
    if (action == MENU_STICK_PLUS)
      settings.auxToggleMask |= (1 << auxListIndex);
    else if (action == MENU_STICK_MINUS)
      settings.auxToggleMask &= ~(1 << auxListIndex);
  }
  else if (uiPage == UI_POT_DISPLAY_EDIT) {
    if (action == MENU_STICK_PLUS || action == MENU_STICK_MINUS)
      potGaugeStyle = (potGaugeStyle == POT_GAUGE_CLASSIC)
                        ? POT_GAUGE_CUMULATIVE : POT_GAUGE_CLASSIC;
  }
  else if (uiPage == UI_BAT_EDIT) {
    uint8_t &v = batItemIndex ? batEditMax10 : batEditMin10;
    if (action == MENU_STICK_PLUS && v < 255) {
      if (batItemIndex || v + 1 < batEditMax10) ++v;
    } else if (action == MENU_STICK_MINUS && v > 10) {
      if (!batItemIndex || v - 1 > batEditMin10) --v;
    }
  }
  else if (uiPage == UI_RF_POWER_EDIT) {
    if (action == MENU_STICK_PLUS && rfPowerLevel < RF_POWER_MAX) ++rfPowerLevel;
    else if (action == MENU_STICK_MINUS && rfPowerLevel > RF_POWER_LOW) --rfPowerLevel;
  }
  else if (uiPage == UI_DEBUG_SCREEN_EDIT) {
    if (action == MENU_STICK_PLUS || action == MENU_STICK_MINUS)
      debugScreenEnabled ^= 1;
  }
  else if (uiPage == UI_RESET_CONFIRM) {
    if (action == MENU_STICK_PLUS) resetChoice = true;
    else if (action == MENU_STICK_MINUS) resetChoice = false;
  }

  urgentDisplayUpdate = true;
}

void checkMenuJoystick(unsigned long now) {
  if (!menuOpen) {
    menuStickLastAction = MENU_STICK_NONE;
    menuStickActionSince = 0;
    menuStickLockedUntilCenter = false;
    return;
  }

  // IMPORTANT: during both calibration phases all stick movement is data to
  // measure, never a menu command. This prevents FULL RANGE movements from
  // jumping back into menus or changing selections.
  if (uiPage == UI_CAL_CENTER || uiPage == UI_CAL_RANGE) {
    menuStickLastAction = MENU_STICK_NONE;
    menuStickActionSince = 0;
    menuStickLockedUntilCenter = true;
    return;
  }

  const uint8_t action = readMenuStickDirection();

  // A calibration may end while the right stick is still at an extreme. Do
  // not interpret that position on the menu that reappears. Require a real
  // return to center first.
  if (menuStickLockedUntilCenter) {
    menuStickLastAction = MENU_STICK_NONE;
    menuStickActionSince = 0;
    if (action == MENU_STICK_NONE)
      menuStickLockedUntilCenter = false;
    return;
  }

  if (action == MENU_STICK_NONE) {
    menuStickLastAction = MENU_STICK_NONE;
    menuStickActionSince = 0;
    return;
  }

  if (action != menuStickLastAction) {
    menuStickLastAction = action;
    menuStickActionSince = now;
    menuStickLastRepeat = now;
    menuStickAction(action);
    return;
  }

  // Auto-repeat uniquement sur l axe vertical. Gauche/droite sont volontairement
  // one-shot jusqu au retour au centre pour ne pas traverser plusieurs niveaux.
  if ((action == MENU_STICK_PLUS || action == MENU_STICK_MINUS) &&
      menuStickActionSince != 0 &&
      (unsigned long)(now - menuStickActionSince) >= MENU_REPEAT_DELAY_MS &&
      (unsigned long)(now - menuStickLastRepeat) >= MENU_REPEAT_INTERVAL_MS) {
    menuStickLastRepeat = now;
    menuStickAction(action);
  }
}

void check_buttons(unsigned long now) {
  sensorValue5_switch = readAnalogAverage(PIN_PB);
  const uint8_t current = decodeButton(sensorValue5_switch);

  if (current != buttonCandidate) {
    buttonCandidate = current;
    buttonCandidateSince = now;
  }

  if (buttonCandidate != buttonStable &&
      (unsigned long)(now - buttonCandidateSince) >= BUTTON_DEBOUNCE_MS) {
    const uint8_t previous = buttonStable;
    buttonStable = buttonCandidate;

    if (buttonStable != BUTTON_NONE && buttonStable != BUTTON_MENU_COMBO)
      triggerInfoAlert();

    const bool previousIsDebugNav = debugScreenEnabled && !menuOpen &&
        (previous == BUTTON_JOYSTICK_RIGHT || previous == BUTTON_JOYSTICK_LEFT);

    if (!menuOpen && previous != BUTTON_MENU_COMBO && buttonStable != BUTTON_MENU_COMBO &&
        !previousIsDebugNav)
      releaseAux(previous);

    if (!menuOpen && buttonStable == BUTTON_MENU_COMBO) {
      // Cancel a pending single-button DEBUG click: this is the JL+JR menu combo.
      debugNavButton = BUTTON_NONE;
      debugNavPressSince = 0;
      enterMenuCombo(now, previous);
    } else if (buttonStable == BUTTON_NONE) {
      // DEBUG SCREEN enabled: JR advances MAIN->RF->ALERT->MAIN; JL goes backward.
      if (!menuOpen && debugScreenEnabled &&
          (previous == BUTTON_JOYSTICK_RIGHT || previous == BUTTON_JOYSTICK_LEFT) &&
          debugNavButton == previous && debugNavPressSince != 0) {
        if (previous == BUTTON_JOYSTICK_RIGHT) {
          debugScreenPage = (uint8_t)((debugScreenPage + 1U) % 3U);
        } else {
          debugScreenPage = (debugScreenPage == 0U) ? 2U : (uint8_t)(debugScreenPage - 1U);
        }
        debugScreenActive = (debugScreenPage != 0U);
        forceMainRedraw = true;
        urgentDisplayUpdate = true;
      }
      debugNavButton = BUTTON_NONE;
      debugNavPressSince = 0;
      comboHoldSince = 0;
      comboHoldTriggered = false;
      suppressAuxUntilRelease = false;
      if (menuOpen) menuIgnoreButtonsUntilRelease = false;
    } else if (menuOpen) {
      // PB LEFT is normally an AUX, but inside CALIB it becomes the safe
      // capture/save key so the right joystick stays mechanically untouched.
      if (!menuIgnoreButtonsUntilRelease && buttonStable != BUTTON_MENU_COMBO)
        menuButtonPress(buttonStable);
    } else if (!suppressAuxUntilRelease) {
      if (debugScreenEnabled &&
          (buttonStable == BUTTON_JOYSTICK_RIGHT || buttonStable == BUTTON_JOYSTICK_LEFT)) {
        // In debug-screen mode JR/JL are UI keys outside SETTINGS, not AUX keys.
        debugNavButton = buttonStable;
        debugNavPressSince = now;
      } else {
        pressAux(buttonStable, now);
      }
    }
  }

  // The JL+JR combination opens the menu only from the main RC screen.
  if (!menuOpen && buttonStable == BUTTON_MENU_COMBO && !comboHoldTriggered) {
    if (comboHoldSince == 0) comboHoldSince = now;
    if ((unsigned long)(now - comboHoldSince) >= MENU_HOLD_MS) {
      comboHoldTriggered = true;
      openMenu();
    }
  }
}

// ------------------------------------------------------------
// Battery simulation + radio link quality
// ------------------------------------------------------------
void updateLinkQuality(bool txOk) {
  const unsigned long now = millis();
  const bool wasLost = !lastAckTime ||
                       (unsigned long)(now - lastAckTime) > LINK_LOST_TIMEOUT_MS;

  if (txOk) {
    lastAckTime = now;
    if (wasLost) {
      linkQualityAccum = 0;
      linkQualitySamples = 0;
      linkBars = 4;
      urgentDisplayUpdate = true;
    }
  }

  linkQualityAccum += txOk ? 100 : 0;
  linkQualitySamples++;

  if (!linkAckFresh()) {
    if (linkBars != 0) {
      linkBars = 0;
      urgentDisplayUpdate = true;
    }
  }

  if (linkQualitySamples >= LINK_WINDOW_SAMPLES) {
    const uint8_t successPercent =
        (uint8_t)(linkQualityAccum / linkQualitySamples);

    if (successPercent >= 98)      linkBars = 4;
    else if (successPercent >= 90) linkBars = 3;
    else if (successPercent >= 75) linkBars = 2;
    else if (successPercent >= 50) linkBars = 1;
    else                           linkBars = 0;

    if (!linkAckFresh()) linkBars = 0;

    linkQualityAccum = 0;
    linkQualitySamples = 0;
    urgentDisplayUpdate = true;
  }
}

// ------------------------------------------------------------
// OLED main screen
// ------------------------------------------------------------
void clearSmallArea(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  uView.rectFill(x, y, w, h, BLACK, NORM);
}

void drawAuxIndicator(uint8_t x, uint8_t y, uint8_t aux) {
  // 7x7 pixels. Shape shows the AUX mode; fill shows its current state:
  //   circle = instantaneous, square = toggle
  //   outline = OFF, filled = ON
  clearSmallArea(x - 4, y - 4, 9, 9);

  const uint8_t state = auxEffectiveOutput(aux);
  if (auxIsToggle(aux)) {
    if (state) uView.rectFill(x - 3, y - 3, 7, 7);
    else       uView.rect(x - 3, y - 3, 7, 7);
  } else {
    if (state) uView.circleFill(x, y, 3);
    else       uView.circle(x, y, 3);
  }
}

void update_button_status() {
  // Same logical positions as v3.x. Larger 7x7 indicators replace ON/OFF text.
  drawAuxIndicator(48, 19, AUX_JR);
  drawAuxIndicator(15, 19, AUX_JL);
  drawAuxIndicator(15, 31, AUX_LEFT);
  drawAuxIndicator(48, 31, AUX_RIGHT);
}

void drawBatteryGauge() {
  // TX battery icon: original outline, four discrete fill levels.
  // MIN and MAX come directly from the TX BAT menu.
  clearSmallArea(0, 0, 14, 10);

  const uint16_t mn = txBatteryMinMv();
  const uint16_t mx = txBatteryMaxMv();

  // Clamp the ATtiny85 measurement to the configured range before mapping.
  uint16_t v = txBatteryMv;
  if (v < mn) v = mn;
  if (v > mx) v = mx;

  uint8_t level = 0;  // 0/4 .. 4/4
  if (mx > mn) {
    level = (uint8_t)(((uint32_t)(v - mn) * 4U) / (mx - mn));
  }

  // Low-battery alarm remains tied to the configured MIN value.
  // At/below MIN the complete battery logo blinks, as before.
  const bool lowBattery = (txBatteryMv <= mn);
  if (lowBattery && (millis() & 0x0200UL)) return;

  // Original external geometry.
  uView.rect(0, 1, 11, 8);
  uView.rectFill(11, 3, 2, 4);

  // Original frame leaves a 9 x 6 pixel internal cavity: x=1..9, y=2..7.
  // Fill uses the full cavity height. At MAX the cavity is completely full.
  if (level) {
    const uint8_t fillWidth = (uint8_t)(((uint16_t)level * 9U + 2U) / 4U);
    uView.rectFill(1, 2, fillWidth, 6);
  }
}

void drawLinkBars() {
  // Top-right: 4 ascending bars. Quality is based on ACK success, not RSSI.
  // Complete link loss (>300 ms without ACK): 0 bars and the whole icon blinks.
  clearSmallArea(52, 0, 12, 10);

  const bool lost = !linkAckFresh();
  if (lost && (millis() & 0x0200UL)) return;

  const uint8_t bars = lost ? 0 : linkBars;
  for (uint8_t i = 0; i < 4; ++i) {
    const uint8_t h = 2 + i * 2;
    const uint8_t x = 53 + i * 3;
    const uint8_t y = 9 - h;
    if (i < bars) uView.rectFill(x, y, 2, h);
    else          uView.rect(x, y, 2, h);
  }
}

void drawActiveModelName() {
  // Central top zone: x=14..51 (38 px). Six 5x7-font characters fit here.
  // v4.1c: the model name is deliberately always steady. Link and RX-battery
  // alarms are shown by their own dedicated indicators instead.
  clearSmallArea(14, 0, 38, 9);

  uint8_t len = friendlyNameLength(activeModel);
  if (len == 0) len = 3;
  const uint8_t textWidth = len * 6;
  const uint8_t x = 14 + ((38 - textWidth) / 2);
  uView.setCursor(x, 0);

  if (friendlyNameLength(activeModel) == 0) {
    char fallback[MODEL_NAME_LEN + 1];
    makeDefaultModelName(fallback, activeModel);
    uView.print(fallback);
  } else {
    for (uint8_t i = 0; i < len; ++i)
      uView.print(storedSettings.model[activeModel].friendlyName[i]);
  }
}

void drawRxBatteryBar() {
  // RX model battery bar.
  // v4.1c alarm behavior separates RF-link loss from RX-battery telemetry:
  // - no ACK for >300 ms: EMPTY bar stays STEADY; only the RF-link icon blinks;
  // - ACK link present but no fresh battery telemetry: EMPTY bar blinks;
  // - ACK link present and RX voltage at/below BAT MIN: EMPTY bar blinks;
  // - otherwise the normal proportional fill is displayed.
  // The model name remains steady at all times.
  const uint8_t x = 15, y = 10;
  const uint8_t outerW = 36, outerH = 4, innerW = 34;

  clearSmallArea(x, y, outerW, outerH);

  const bool ackFresh = linkAckFresh();
  const bool freshTelemetry = telemetryFresh();
  const uint16_t mn = rxBatteryMinMv();
  const uint16_t mx = rxBatteryMaxMv();

  // Complete RX/link loss: the battery value is no longer meaningful.
  // Show an empty, steady frame and leave the blinking alarm to drawLinkBars().
  if (!ackFresh) {
    uView.rect(x, y, outerW, outerH);
    return;
  }

  // Link is alive, so a missing/stale battery payload is specifically a
  // telemetry problem. Low battery is also indicated here.
  const bool rxBatteryAlarm = !freshTelemetry || (rxBatteryMv <= mn);

  // During the OFF half-cycle, erase the whole RX-battery bar.
  // During the ON half-cycle, show its EMPTY frame only.
  if (rxBatteryAlarm) {
    if (millis() & 0x0200UL) return;
    uView.rect(x, y, outerW, outerH);
    return;
  }

  uView.rect(x, y, outerW, outerH);

  uint8_t fill;
  if (rxBatteryMv >= mx) fill = innerW;
  else fill = (uint8_t)(((uint32_t)(rxBatteryMv - mn) * innerW) / (mx - mn));

  if (fill) uView.rectFill(x + 1, y + 1, fill, 2);
}

// Two horizontal sliders share one routine: YAW and ROLL.
void drawSliderH(uint8_t x, uint8_t y, uint16_t value) {
  const uint8_t ticks = 30;
  for (uint8_t p = 1; p <= ticks + 1; p += 2) uView.lineV(x + p, y + 5, 3);
  for (uint8_t p = 1; p <= ticks + 1; p += 10) uView.lineV(x + p, y + 3, 2);

  if (value > 1023U) value = 1023U;
  const uint8_t pos = (uint8_t)(((uint32_t)value * ticks + 511U) / 1023U);
  uView.lineH(x + pos, y, 3);
  uView.pixel(x + pos + 1, y + 1);
}

// The two vertical sliders use the same geometry. mirror=false is the left
// THROTTLE scale, mirror=true is the right PITCH scale.
void drawSliderV(uint8_t edgeX, uint8_t y, uint16_t value, bool mirror) {
  const uint8_t ticks = 20;
  for (uint8_t p = 1; p <= ticks + 1; p += 2) {
    if (mirror) uView.lineH((int16_t)edgeX - 2, y + p, 3);
    else        uView.lineH(edgeX, y + p, 3);
  }
  for (uint8_t p = 1; p <= ticks + 1; p += 10) {
    if (mirror) uView.lineH((int16_t)edgeX - 4, y + p, 2);
    else        uView.lineH(edgeX + 3, y + p, 2);
  }

  if (value > 1023U) value = 1023U;
  const uint8_t pos = (uint8_t)(((uint32_t)(1023U - value) * ticks + 511U) / 1023U);
  if (mirror) {
    uView.lineV((int16_t)edgeX - 7, y + pos, 3);
    uView.pixel(edgeX - 6, y + pos + 1);
  } else {
    uView.lineV(edgeX + 7, y + pos, 3);
    uView.pixel(edgeX + 6, y + pos + 1);
  }
}

// 270-degree POT gauge. Geometry is the optimized v3.14.5 gauge, but as
// plain functions: no MicroViewWidget, no heap allocation, no float maths.
static const int8_t potOuterXHalf[46] PROGMEM = {
  -7,-7,-8,-8,-8,-9,-9,-9,-9,-10,-10,-10,-10,-10,-10,-10,
  -10,-10,-10,-10,-10,-10,-9,-9,-9,-9,-8,-8,-8,-7,-7,-7,
  -6,-6,-5,-5,-5,-4,-4,-3,-3,-2,-2,-1,-1,0
};
static const int8_t potOuterYHalf[46] PROGMEM = {
  7,7,6,6,5,5,5,4,4,3,3,2,2,1,1,0,
  -1,-1,-2,-2,-3,-3,-4,-4,-5,-5,-5,-6,-6,-7,-7,-7,
  -8,-8,-8,-9,-9,-9,-9,-10,-10,-10,-10,-10,-10,-10
};
static const int8_t potInnerXHalf[46] PROGMEM = {
  -4,-4,-5,-5,-5,-5,-5,-5,-6,-6,-6,-6,-6,-6,-6,-6,
  -6,-6,-6,-6,-6,-6,-6,-5,-5,-5,-5,-5,-5,-4,-4,-4,
  -4,-4,-3,-3,-3,-2,-2,-2,-2,-1,-1,-1,0,0
};
static const int8_t potInnerYHalf[46] PROGMEM = {
  4,4,4,4,3,3,3,2,2,2,2,1,1,1,0,0,
  0,-1,-1,-1,-2,-2,-2,-2,-3,-3,-3,-4,-4,-4,-4,-4,
  -5,-5,-5,-5,-5,-5,-6,-6,-6,-6,-6,-6,-6,-6
};

void getPotBandPoints(uint8_t index, int8_t &ix, int8_t &iy,
                      int8_t &ox, int8_t &oy) {
  if (index > 90) index = 90;
  uint8_t j = index;
  bool mirror = false;
  if (index > 45) { j = 90 - index; mirror = true; }
  ox = (int8_t)pgm_read_byte(&potOuterXHalf[j]);
  oy = (int8_t)pgm_read_byte(&potOuterYHalf[j]);
  ix = (int8_t)pgm_read_byte(&potInnerXHalf[j]);
  iy = (int8_t)pgm_read_byte(&potInnerYHalf[j]);
  if (mirror) { ox = -ox; ix = -ix; }
}

void drawPotBand(uint8_t index, uint8_t color) {
  int8_t ix, iy, ox, oy;
  getPotBandPoints(index, ix, iy, ox, oy);
  uView.line(32 + ix, 28 + iy, 32 + ox, 28 + oy, color, NORM);

  if (index < 90) {
    int8_t nix, niy, nox, noy;
    getPotBandPoints(index + 1, nix, niy, nox, noy);
    uView.line(32 + (ix + nix) / 2, 28 + (iy + niy) / 2,
               32 + (ox + nox) / 2, 28 + (oy + noy) / 2,
               color, NORM);
  }
}

void drawPotGauge(uint16_t value) {
  if (value > 1023U) value = 1023U;
  const uint8_t index = (uint8_t)(((uint32_t)value * 90U + 511U) / 1023U);

  uView.circle(32, 28, 10);
  uView.circle(32, 28, 6);
  drawPotBand(0, WHITE);
  drawPotBand(45, WHITE);
  drawPotBand(90, WHITE);

  if (potGaugeStyle == POT_GAUGE_CUMULATIVE) {
    for (uint8_t i = 0; i <= index; ++i) drawPotBand(i, WHITE);
  } else {
    const uint8_t first = (index > 4) ? index - 4 : 0;
    uint8_t last = index + 4;
    if (last > 90) last = 90;
    for (uint8_t i = first; i <= last; ++i) drawPotBand(i, WHITE);
  }

  const bool coversMid = (potGaugeStyle == POT_GAUGE_CUMULATIVE)
                           ? (index >= 45)
                           : (index >= 41 && index <= 49);
  if (value == 512U) {
    drawPotBand(44, BLACK);
    drawPotBand(45, BLACK);
    drawPotBand(46, BLACK);
    uView.pixel(31, 16); uView.pixel(32, 16); uView.pixel(33, 16);
  } else if (coversMid) {
    drawPotBand(45, BLACK);
  } else {
    drawPotBand(45, WHITE);
  }
  drawPotBand(0, WHITE);
  drawPotBand(90, WHITE);
}

void drawMainScreen() {
  // v4.0 redraws the complete 64x48 framebuffer each refresh. This is both
  // simpler and safer than XOR/widget state while remaining tiny at 20 Hz.
  uView.clear(PAGE);

  // Display geometry follows the physical sticks only.
  // Radio channel mapping remains CH3=YAW and CH4=THROTTLE.
  drawSliderV(0, 16, sensorValue[CH_THROTTLE], false);
  drawSliderV(63, 16, sensorValue[CH_PITCH], true);
  drawSliderH(0, 40, sensorValue[CH_YAW]);
  drawSliderH(32, 40, sensorValue[CH_ROLL]);
  drawPotGauge(sensorValue[CH_POT]);

  drawBatteryGauge();
  drawActiveModelName();
  drawLinkBars();
  drawRxBatteryBar();
  update_button_status();
  uView.display();
  forceMainRedraw = false;
}

// Debug values use exactly one decimal digit, without floating point.
void printVoltageMv(uint16_t mv, bool valid) {
  if (!valid || mv == 0) {
    uView.print(F("--.-V"));
    return;
  }
  const uint16_t deciV = (uint16_t)((mv + 50U) / 100U);
  uView.print(deciV / 10U);
  uView.print('.');
  uView.print(deciV % 10U);
  uView.print('V');
}

void printTimeUsMs1Value(uint16_t us) {
  const uint16_t deciMs = (uint16_t)((us + 50U) / 100U);
  uView.print(deciMs / 10U);
  uView.print('.');
  uView.print(deciMs % 10U);
}

void printTimeUsMs1(uint16_t us) {
  printTimeUsMs1Value(us);
  // The integrated font contains uppercase letters only. Using "MS" avoids
  // the former "??" shown when lowercase "ms" was requested.
  uView.print(F("MS"));
}

void printAgeSeconds1(unsigned long ageMs) {
  if (ageMs > 9900UL) {
    uView.print(F(">9.9S"));
    return;
  }
  const uint16_t deciS = (uint16_t)((ageMs + 50UL) / 100UL);
  uView.print(deciS / 10U);
  uView.print('.');
  uView.print(deciS % 10U);
  uView.print('S');
}

void drawDebugScreen() {
  uView.clear(PAGE);

  uView.setCursor(8, 0); uView.print(F("RF DEBUG"));

  uView.setCursor(0, 8); uView.print(F("TX "));
  const bool txValid = txBatteryMv != 0 &&
      (unsigned long)(millis() - lastTxBatteryInputTime) <= TX_BATTERY_INPUT_TIMEOUT_MS;
  printVoltageMv(txBatteryMv, txValid);

  uView.setCursor(0, 16); uView.print(F("RX "));
  printVoltageMv(rxBatteryMv, telemetryFresh());

  // Compact 64-pixel layout: separate latency and retry count clearly.
  // A normal '/' glyph would add 6 px and could overflow at L16.8 / R10.
  // Draw a narrow 3-pixel slash manually, then place R immediately after it.
  // Typical display: "L1.5MS/R0". Worst case "L16.8MS/R10" still fits.
  uView.setCursor(0, 24); uView.print('L');
  printTimeUsMs1Value(latencyLastUs);
  uView.print(F("MS"));

  const uint16_t latencyDeciMs = (uint16_t)((latencyLastUs + 50U) / 100U);
  const uint8_t latencyChars = (latencyDeciMs >= 100U) ? 4U : 3U; // 16.8 vs 1.5
  const uint8_t slashX = (uint8_t)(6U * (1U + latencyChars + 2U)); // L + value + MS
  uView.line((int16_t)slashX + 2, 24, slashX, 30, WHITE, NORM);
  uView.setCursor((uint8_t)(slashX + 4U), 24);
  uView.print('R');
  uView.print(retryLast);

  uView.setCursor(0, 32); uView.print(F("A "));
  printTimeUsMs1(latencyAvgUs);

  uView.setCursor(0, 40); uView.print(F("M "));
  printTimeUsMs1(latencyMaxUs);

  uView.display();
  forceMainRedraw = false;
}

uint8_t alertCommandLevel(uint8_t command) {
  if (command < 1 || command > 9) return 0;
  return (uint8_t)(((command - 1U) / 3U) + 1U);
}

void printAlertOutputShort() {
  if (alertOutputMode == ALERT_OUTPUT_BUZZER) uView.print(F("BUZZ"));
  else if (alertOutputMode == ALERT_OUTPUT_BUZZ_VIB) uView.print(F("B+V"));
  else if (alertOutputMode == ALERT_OUTPUT_VIBRO) uView.print(F("VIB"));
  else uView.print(F("OFF"));
}

// Temporary third screen: raw timing diagnostics for D2/PB4.
// C=requested command/fresh ACK, P=actual PB0/PB1 state returned by ATtiny.
void drawAlertDebugScreen() {
  uView.clear(PAGE);
  const unsigned long now = millis();
  const bool ackFresh = lastAlertAckTime != 0 &&
      (unsigned long)(now - lastAlertAckTime) <= 1000UL;
  const bool pinFresh = lastAlertPinStateTime != 0 &&
      (unsigned long)(now - lastAlertPinStateTime) <= 1000UL;
  const bool txFresh = lastAlertTxEchoTime != 0 &&
      (unsigned long)(now - lastAlertTxEchoTime) <= 1000UL;
  const bool atFresh = lastAlertRemoteMeasureTime != 0 &&
      (unsigned long)(now - lastAlertRemoteMeasureTime) <= 1000UL;

  uView.setCursor(7, 0); uView.print(F("WIRE DBG"));

  uView.setCursor(0, 8);
  uView.print(F("C"));
  if (alertLastWireCommand <= 9) uView.print(alertLastWireCommand); else uView.print('-');
  uView.print('/');
  if (ackFresh && alertAckCommand <= 9) uView.print(alertAckCommand); else uView.print('-');
  uView.print(F(" P"));
  if (pinFresh && alertPinState <= 3U) {
    uView.print((alertPinState & 0x01U) ? '1' : '0');
    uView.print((alertPinState & 0x02U) ? '1' : '0');
  } else uView.print(F("--"));

  uView.setCursor(0, 16); uView.print(F("TX "));
  if (txFresh) printTimeUsMs1(alertTxEchoWidthUs); else uView.print(F("--.-MS"));

  uView.setCursor(0, 24); uView.print(F("AT "));
  if (atFresh) printTimeUsMs1(alertRemoteMeasureUs); else uView.print(F("--.-MS"));

  uView.setCursor(0, 32); uView.print(F("AK "));
  if (ackFresh) printTimeUsMs1(alertAckWidthUs); else uView.print(F("--.-MS"));

  uView.setCursor(0, 40); uView.print(F("AGE "));
  if (lastAlertAckTime == 0) uView.print(F("--.-S"));
  else printAgeSeconds1(now - lastAlertAckTime);

  uView.display();
  forceMainRedraw = false;
}

// ------------------------------------------------------------
// OLED menu screens
// ------------------------------------------------------------
void drawRootMenu() {
  uView.clear(PAGE);
  uView.setCursor(8, 0);
  uView.print(F("SETTINGS"));

  const uint8_t start = (rootIndex / 4) * 4;

  for (uint8_t row = 0; row < 4; ++row) {
    const uint8_t item = start + row;
    if (item >= ROOT_ITEM_COUNT) continue;

    uView.setCursor(0, 10 + row * 10);
    uView.print(item == rootIndex ? '>' : ' ');
    rootLabel(item);
  }
}

void printModelNumber(uint8_t modelIndex) {
  uView.print('0');
  uView.print(modelIndex + 1);
}

void drawModelMenu() {
  uView.clear(PAGE);
  uView.setCursor(11, 0); uView.print(PFSTR(S_MODEL)); printModelNumber(activeModel);
  uView.setCursor(0, 12); uView.print(modelMenuIndex == 0 ? F(">SELECT") : F(" SELECT"));
  uView.setCursor(0, 24); uView.print(modelMenuIndex == 1 ? F(">RENAME") : F(" RENAME"));
  uView.setCursor(0, 36); uView.print(modelMenuIndex == 2 ? F(">BIND RX") : F(" BIND RX"));
}

void drawModelSelect() {
  uView.clear(PAGE);
  uView.setCursor(14, 0); uView.print(F("SELECT"));

  // With 5 models, keep four rows visible: 01..04 then 02..05.
  const uint8_t start = (modelSelectIndex < 4) ? 0 : 1;
  for (uint8_t row = 0; row < 4; ++row) {
    const uint8_t m = start + row;
    uView.setCursor(0, 10 + row * 10);
    uView.print(m == modelSelectIndex ? '>' : ' ');
    printModelNumber(m);
    uView.print(' ');
    const uint8_t len = friendlyNameLength(m);
    for (uint8_t i = 0; i < len; ++i) uView.print(storedSettings.model[m].friendlyName[i]);
  }
}

void drawModelRename() {
  uView.clear(PAGE);
  uView.setCursor(14, 0); uView.print(F("RENAME"));
  uView.setCursor(11, 10); uView.print(PFSTR(S_MODEL)); printModelNumber(activeModel);

  // Six characters, centered in the 64-pixel display.
  const uint8_t nameX = 14;
  uView.setCursor(nameX, 23);
  for (uint8_t i = 0; i < MODEL_NAME_LEN; ++i) uView.print(renameBuffer[i]);

  // Cursor under the currently edited character.
  uView.lineH(nameX + renamePos * 6, 33, 5);
  uView.setCursor(2, 40); uView.print(F("U/D R> JR"));
}

void drawModelBind() {
  uView.clear(PAGE);
  uView.setCursor(8, 0); uView.print(F("BIND RX"));
  uView.setCursor(8, 11); uView.print(PFSTR(S_MODEL)); uView.print(' '); printModelNumber(activeModel);

  uView.setCursor(5, 24);
  if (bindResult == 1) uView.print(F("BIND OK"));
  else if (bindResult == 2) uView.print(F("NO RX"));
  else uView.print(F("POWER RX"));

  uView.setCursor(0, 40); uView.print(F("JR:BIND JL:<"));
}

void drawAxisList() {
  uView.clear(PAGE);
  uView.setCursor(0, 0);
  if (editKind == EDIT_EXPO) uView.print(PFSTR(S_EXPO));
  else if (editKind == EDIT_DUALRATE) uView.print(PFSTR(S_DR));
  else if (editKind == EDIT_REVERSE) uView.print(PFSTR(S_REVERSE));
  else uView.print(PFSTR(S_TRIM));

  uint8_t start = 0;
  if (editKind == EDIT_REVERSE && axisListIndex >= 4) start = 4;

  for (uint8_t row = 0; row < 4; ++row) {
    const uint8_t item = start + row;
    uView.setCursor(0, 10 + row * 10);
    uView.print(item == axisListIndex ? '>' : ' ');
    controlName(item);
  }
}

void drawAxisEdit() {
  uView.clear(PAGE);
  uView.setCursor(0, 0);
  if (editKind == EDIT_EXPO) uView.print(F("EXPO "));
  else if (editKind == EDIT_DUALRATE) uView.print(F("D/R "));
  else if (editKind == EDIT_REVERSE) uView.print(F("REV "));
  else uView.print(F("TRIM "));
  if (editKind == EDIT_REVERSE) controlName(axisListIndex);
  else controlName(axisListIndex);

  uView.setCursor(0, 16);
  uView.print(F("VAL "));
  if (editKind == EDIT_REVERSE) {
    if (axisListIndex == 3 && storedSettings.throttleMode[activeModel] == THROTTLE_FWD_ONLY)
      uView.print(F("LOCK"));
    else
      uView.print((settings.reverseMask & (1 << axisListIndex)) ? F("ON") : F("OFF"));
  } else {
    uView.print(currentEditValue());
    if (editKind != EDIT_TRIM) uView.print('%');
  }

  uView.setCursor(0, 30); uView.print(PFSTR(S_UP_DOWN));
  uView.setCursor(0, 40); uView.print(PFSTR(S_OK_BACK));
}

void drawEndpointList() {
  uView.clear(PAGE);
  uView.setCursor(4, 0); uView.print(PFSTR(S_ENDPOINT));
  for (uint8_t row = 0; row < 4; ++row) {
    uView.setCursor(0, 10 + row * 10);
    uView.print(row == endpointAxisIndex ? '>' : ' ');
    controlName(row);
  }
}

void drawEndpointEdit() {
  uView.clear(PAGE);
  uView.setCursor(0, 0); uView.print(F("EP ")); controlName(endpointAxisIndex);

  const bool fwdThrottle = endpointAxisIndex == 3 &&
      storedSettings.throttleMode[activeModel] == THROTTLE_FWD_ONLY;
  if (fwdThrottle) {
    uView.setCursor(0, 14); uView.print(F(" MIN FIX"));
    uView.setCursor(0, 26); uView.print(F(">MAX "));
    uView.print(storedSettings.endpointPos[activeModel][endpointAxisIndex]); uView.print('%');
  } else {
    uView.setCursor(0, 14);
    uView.print(endpointSideIndex == 0 ? F(">NEG ") : F(" NEG "));
    uView.print(storedSettings.endpointNeg[activeModel][endpointAxisIndex]); uView.print('%');
    uView.setCursor(0, 26);
    uView.print(endpointSideIndex == 1 ? F(">POS ") : F(" POS "));
    uView.print(storedSettings.endpointPos[activeModel][endpointAxisIndex]); uView.print('%');
  }
  uView.setCursor(0, 40); uView.print(F("U/D R> JR:OK"));
}

void drawThrottleModeEdit() {
  uView.clear(PAGE);
  uView.setCursor(4, 0); uView.print(PFSTR(S_THR_MODE));
  uView.setCursor(8, 18);
  uView.print(storedSettings.throttleMode[activeModel] == THROTTLE_FWD_ONLY
                ? F("FWD ONLY") : F("BIDIR"));
  uView.setCursor(0, 30); uView.print(PFSTR(S_UD_CHG));
  uView.setCursor(0, 40); uView.print(PFSTR(S_OK_BACK));
}

void printAlertOutputMode() {
  if (alertOutputMode == ALERT_OUTPUT_BUZZER) uView.print(F("BUZZ"));
  else if (alertOutputMode == ALERT_OUTPUT_BUZZ_VIB) uView.print(F("B+V"));
  else if (alertOutputMode == ALERT_OUTPUT_VIBRO) uView.print(F("VIB"));
  else uView.print(F("OFF"));
}

void drawAlertMenu() {
  uView.clear(PAGE);
  uView.setCursor(8, 0); uView.print(PFSTR(S_ALERTS));
  uView.setCursor(0, 14); uView.print(alertMenuIndex == 0 ? F(">OUTPUT ") : F(" OUTPUT "));
  printAlertOutputMode();
  uView.setCursor(0, 27); uView.print(alertMenuIndex == 1 ? F(">LEVELS") : F(" LEVELS"));
}

void drawAlertOutputEdit() {
  uView.clear(PAGE);
  uView.setCursor(5, 0); uView.print(F("AL OUTPUT"));
  uView.setCursor(14, 18); printAlertOutputMode();
  uView.setCursor(0, 30); uView.print(PFSTR(S_UD_CHG));
  uView.setCursor(0, 40); uView.print(PFSTR(S_OK_BACK));
}

void drawAlertLevels() {
  uView.clear(PAGE);
  uView.setCursor(7, 0); uView.print(F("AL LEVEL"));
  for (uint8_t i = 0; i < 3; ++i) {
    uView.setCursor(0, 11 + i * 10);
    uView.print(i == alertLevelIndex ? '>' : ' ');
    uView.print('L'); uView.print(i + 1); uView.print(' ');
    uView.print((alertLevelMask & (1U << i)) ? F("ON") : F("OFF"));
  }
  uView.setCursor(0, 41); uView.print(F("R:TGL JR:OK"));
}

void drawDeadbandEdit() {
  uView.clear(PAGE);
  uView.setCursor(4, 0); uView.print(PFSTR(S_DEADBAND));
  uView.setCursor(12, 18); uView.print(settings.deadband); uView.print('%');
  uView.setCursor(0, 30); uView.print(PFSTR(S_UP_DOWN));
  uView.setCursor(0, 40); uView.print(PFSTR(S_OK_BACK));
}

void auxName(uint8_t aux) {
  controlName(aux + 4);
}

void drawAuxList() {
  uView.clear(PAGE);
  uView.setCursor(6, 0); uView.print(PFSTR(S_AUX_MODE));

  for (uint8_t row = 0; row < 4; ++row) {
    uView.setCursor(0, 10 + row * 10);
    uView.print(row == auxListIndex ? '>' : ' ');
    auxName(row);
    uView.print(' ');
    uView.print(auxIsToggle(row) ? F("TGL") : F("INST"));
  }
}

void drawAuxEdit() {
  uView.clear(PAGE);
  uView.setCursor(4, 0); uView.print(F("AUX ")); auxName(auxListIndex);
  uView.setCursor(12, 18);
  uView.print(auxIsToggle(auxListIndex) ? F("TOGGLE") : F("INSTANT"));
  uView.setCursor(0, 30); uView.print(F("U:TGL D:IN"));
  uView.setCursor(0, 40); uView.print(PFSTR(S_OK_BACK));
}

void drawPotDisplayEdit() {
  uView.clear(PAGE);
  uView.setCursor(4, 0); uView.print(PFSTR(S_POT_DISP));
  uView.setCursor(5, 18);
  uView.print(potGaugeStyle == POT_GAUGE_CUMULATIVE ? F("CUMUL") : F("CLASSIC"));
  uView.setCursor(0, 30); uView.print(PFSTR(S_UD_CHG));
  uView.setCursor(0, 40); uView.print(PFSTR(S_OK_BACK));
}

void printBattery10(uint8_t v) {
  uView.print(v / 10);
  uView.print('.');
  uView.print(v % 10);
  uView.print('V');
}

void drawBatteryEdit() {
  uView.clear(PAGE);
  uView.setCursor(10, 0);
  uView.print(batEditTarget ? PFSTR(S_TX_BAT) : PFSTR(S_RX_BAT));

  // Display the dedicated edit buffer, not the persisted values.
  const uint8_t mn = batEditMin10;
  const uint8_t mx = batEditMax10;

  uView.setCursor(0, 14); uView.print(batItemIndex ? F(" MIN ") : F(">MIN "));
  printBattery10(mn);
  uView.setCursor(0, 26); uView.print(batItemIndex ? F(">MAX ") : F(" MAX "));
  printBattery10(mx);
  uView.setCursor(0, 38); uView.print(F("U/D +/- R>"));
}

void drawRfPowerEdit() {
  uView.clear(PAGE);
  uView.setCursor(5, 0); uView.print(PFSTR(S_RF_POWER));
  uView.setCursor(18, 18);
  if (rfPowerLevel == RF_POWER_MAX) uView.print(F("MAX"));
  else if (rfPowerLevel == RF_POWER_MID) uView.print(F("MID"));
  else uView.print(F("LOW"));
  uView.setCursor(0, 30); uView.print(PFSTR(S_UD_CHG));
  uView.setCursor(0, 40); uView.print(PFSTR(S_OK_BACK));
}

void drawDebugScreenEdit() {
  uView.clear(PAGE);
  uView.setCursor(2, 0); uView.print(PFSTR(S_DEBUG_SCR));
  uView.setCursor(18, 18);
  uView.print(debugScreenEnabled ? F("ON") : F("OFF"));
  uView.setCursor(0, 30); uView.print(PFSTR(S_UD_CHG));
  uView.setCursor(0, 40); uView.print(PFSTR(S_OK_BACK));
}


void drawCalibration() {
  uView.clear(PAGE);
  uView.setCursor(8, 0); uView.print(PFSTR(S_CALIB));
  if (uiPage == UI_CAL_CENTER) {
    uView.setCursor(0, 14); uView.print(F("CENTER"));
    uView.setCursor(0, 25); uView.print(F("ALL STICKS"));
  } else {
    uView.setCursor(0, 14); uView.print(F("MOVE ALL"));
    uView.setCursor(0, 25); uView.print(F("FULL RANGE"));
  }
  uView.setCursor(0, 40); uView.print(PFSTR(S_CAL_BACK));
}

void drawResetConfirm() {
  uView.clear(PAGE);
  uView.setCursor(8, 0); uView.print(F("RESET ?"));
  uView.setCursor(12, 18);
  uView.print(resetChoice ? F("YES") : F("NO"));
  uView.setCursor(0, 30); uView.print(PFSTR(S_UD_CHG));
  uView.setCursor(0, 40); uView.print(PFSTR(S_OK_BACK));
}

void drawMenuScreen() {
  switch (uiPage) {
    case UI_MENU_ROOT: drawRootMenu(); break;
    case UI_MODEL_MENU: drawModelMenu(); break;
    case UI_MODEL_SELECT: drawModelSelect(); break;
    case UI_MODEL_RENAME: drawModelRename(); break;
    case UI_MODEL_BIND: drawModelBind(); break;
    case UI_AXIS_LIST: drawAxisList(); break;
    case UI_AXIS_EDIT: drawAxisEdit(); break;
    case UI_ENDPOINT_LIST: drawEndpointList(); break;
    case UI_ENDPOINT_EDIT: drawEndpointEdit(); break;
    case UI_THROTTLE_MODE_EDIT: drawThrottleModeEdit(); break;
    case UI_ALERT_MENU: drawAlertMenu(); break;
    case UI_ALERT_OUTPUT_EDIT: drawAlertOutputEdit(); break;
    case UI_ALERT_LEVELS: drawAlertLevels(); break;
    case UI_DEADBAND_EDIT: drawDeadbandEdit(); break;
    case UI_AUX_LIST: drawAuxList(); break;
    case UI_AUX_EDIT: drawAuxEdit(); break;
    case UI_POT_DISPLAY_EDIT: drawPotDisplayEdit(); break;
    case UI_BAT_EDIT: drawBatteryEdit(); break;
    case UI_RF_POWER_EDIT: drawRfPowerEdit(); break;
    case UI_DEBUG_SCREEN_EDIT: drawDebugScreenEdit(); break;
    case UI_CAL_CENTER:
    case UI_CAL_RANGE: drawCalibration(); break;
    case UI_RESET_CONFIRM: drawResetConfirm(); break;
    default: break;
  }
  uView.display();
}

void refresh_lcd_screen() {
  if (menuOpen) drawMenuScreen();
  else if (debugScreenEnabled && debugScreenPage == 1U) drawDebugScreen();
  else if (debugScreenEnabled && debugScreenPage == 2U) drawAlertDebugScreen();
  else drawMainScreen();
}

// ------------------------------------------------------------
// Radio
// ------------------------------------------------------------
void send_data() {
  Data.ch0 = (byte)(((uint32_t)sensorValue[CH_POT] * 100UL + 511UL) / 1023UL);

  // While the settings/calibration menu is open, keep the four proportional
  // channels at the same neutral value already used by the receiver failsafe.
  // This prevents stick movements made during calibration from driving the model.
  if (menuOpen) {
    const byte neutralCode = encodeExtendedAxisValue(0);
    Data.ch1 = neutralCode;
    Data.ch2 = neutralCode;
    Data.ch3 = neutralCode;
    Data.ch4 = (storedSettings.throttleMode[activeModel] == THROTTLE_FWD_ONLY)
                 ? encodeExtendedAxisValue(-1000) : neutralCode;
  } else {
    Data.ch1 = axisToByte(0); // ROLL
    Data.ch2 = axisToByte(1); // PITCH
    // Final protocol mapping: ch3 = YAW, ch4 = THROTTLE.
    Data.ch3 = axisToByte(2); // YAW
    Data.ch4 = axisToByte(3); // THROTTLE
  }
  // ch5 now carries TX battery VOLTAGE in 0.1 V units (not a percentage).
  // Example: 3.8 V -> 38. The local UI keeps the full millivolt value.
  Data.ch5 = (byte)(((txBatteryMv + 50U) / 100U) & 0x7FU);
  if (storedSettings.throttleMode[activeModel] == THROTTLE_FWD_ONLY)
    Data.ch5 |= 0x80U;

  Data.ch6 = auxEffectiveOutput(AUX_JR);
  Data.ch7 = auxEffectiveOutput(AUX_JL);
  Data.ch8 = auxEffectiveOutput(AUX_RIGHT);
  Data.ch9 = auxEffectiveOutput(AUX_LEFT);

  const uint32_t txStartUs = micros();
  const bool txOk = radio.write(&Data, sizeof(Data));
  const uint32_t txElapsedUs32 = micros() - txStartUs;

  // Actual retransmissions used by the just-completed TX.  Read directly from
  // OBSERVE_TX.ARC_CNT so this also compiles with older RF24 libraries that do
  // not implement radio.getARC().
  retryLast = readNrfArcCountCompat();
  const uint16_t txElapsedUs = (txElapsedUs32 > 65535UL)
                                 ? 65535U : (uint16_t)txElapsedUs32;
  updateLatencyStats(txElapsedUs);
  updateLinkQuality(txOk);

  if (txOk && radio.isAckPayloadAvailable()) {
    uint16_t mv;
    radio.read(&mv, sizeof(mv));
    if (mv) {
      rxBatteryMv = mv;
      lastTelemetryTime = millis();
      // Do NOT force an OLED redraw for every ACK payload. At 50 Hz that would
      // couple the UI workload directly to the RF loop. The normal 20 Hz OLED
      // refresh is more than fast enough for battery telemetry.
    }
  }
}

// ------------------------------------------------------------
// Setup / loop
// ------------------------------------------------------------
void setup() {
#if WIRE_SERIAL_DIAG
  Serial.begin(115200);
  delay(100);
  Serial.println(F("=== MicroView TX 4.1.15 RF RETRY + UNIT DISPLAY FIX ==="));
  Serial.println(F("RF disabled: D0/D1 reserved for Serial during this test."));
  Serial.println(F("Type 0..9 to force a one-wire command, N for normal mode."));
#endif
  uView.begin();
  uView.clear(PAGE);
  uView.display();
  loadSettings();

  // D2 battery input from ATtiny85. Idle is HIGH; LOW pulse width = mV.
  pinMode(PIN_TX_BATTERY, INPUT_PULLUP);
  attachInterrupt(0, txBatteryPulseISR, CHANGE); // INT0 = D2 on ATmega328P

  initializeFilters();

  forceMainRedraw = true;
  refresh_lcd_screen();

#if !WIRE_SERIAL_DIAG
  radio.begin();
  // Bench-validated RF settings. Data rate/channel stay fixed; PA level is
  // selected globally from SETTINGS > RF POWER and restored from EEPROM.
  applyRfPower();
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(RF_CHANNEL);
  radio.setRetries(2, 10);
  radio.setAutoAck(true);
  radio.enableAckPayload();
  openActiveModelPipe();
  radio.stopListening();
#else
  Serial.println(F("[INIT] D2 one-wire interrupt attached; waiting for ATtiny battery pulses."));
#endif

  const unsigned long now = millis();
  systemStartMs = now;
  lastSensorTime = now;
  lastButtonTime = now;
  lastDisplayTime = now;
  lastTxBatteryInputTime = now;
  nextSendUs = micros() + SEND_INTERVAL_US;
}

// RF deadline test. Signed subtraction is rollover-safe for micros() as long
// as deadlines are much closer than 2^31 us, which is trivially true here.
static bool rfDeadlineDue(uint32_t nowUs) {
  return (int32_t)(nowUs - nextSendUs) >= 0;
}

// v4.0o periodic scheduler: keep an absolute 20 ms clock so small execution
// delays do not accumulate into a 21 ms period. After a normal periodic frame,
// advance exactly one slot. If the following slot is already in the past, a
// complete frame was missed: resynchronise to now + 20 ms without any catch-up
// burst.
static void scheduleNextRfPeriodic() {
  nextSendUs += SEND_INTERVAL_US;

  const uint32_t nowUs = micros();
  if ((int32_t)(nowUs - nextSendUs) >= 0) {
    nextSendUs = nowUs + SEND_INTERVAL_US;
  }
}

// v4.1: no asynchronous RF sends. AUX/menu changes are included in the next
// periodic 50 Hz frame (maximum command latency about 20 ms).

void loop() {
  unsigned long now = millis();
  bool rfSentThisLoop = false;

  updateTxBatteryInput(now);
#if WIRE_SERIAL_DIAG
  serviceWireSerialDebug(now);
#endif

  // 1) RF first. All commands use the same deterministic 50 Hz stream.
  // Periodic frames follow an absolute 20 ms clock. Small delays
  // are recovered on the next interval; a fully missed slot is skipped cleanly
  // rather than generating a catch-up burst.
  uint32_t nowUs = micros();
  if (rfDeadlineDue(nowUs)) {
#if !WIRE_SERIAL_DIAG
    send_data();
#endif
    scheduleNextRfPeriodic();
    rfSentThisLoop = true;
  }

  // Alert commands are emitted just after an RF frame. With protocol v4 they
  // last 1..10 ms, leaving margin before the next 20 ms RF deadline.
  if (rfSentThisLoop) serviceAlertOutput(millis());

  // 2) Acquire controls after servicing RF.
  now = millis();
  if ((unsigned long)(now - lastSensorTime) >= SENSOR_INTERVAL_MS) {
    lastSensorTime = now;
    get_data();
  }

  if ((unsigned long)(now - lastButtonTime) >= BUTTON_INTERVAL_MS) {
    lastButtonTime = now;
    check_buttons(now);
  }

  if (menuOpen) {
    checkMenuJoystick(now);
  }

  // AUX/menu state changes are transmitted by the next periodic 50 Hz frame.

  // 3) OLED only just AFTER an RF transmission. If a redraw becomes due between
  // RF frames, it waits until the next transmitted frame. This keeps a redraw
  // from starting immediately before a radio deadline. ACK telemetry therefore
  // remains visually refreshed at about 20 Hz without driving the RF cadence.
  now = millis();
  if (rfSentThisLoop &&
      (urgentDisplayUpdate ||
       (unsigned long)(now - lastDisplayTime) >= DISPLAY_INTERVAL_MS)) {
    lastDisplayTime = now;
    refresh_lcd_screen();
    urgentDisplayUpdate = false;
  }
}
