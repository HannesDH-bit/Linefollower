// =============================================================================
// config.h  -  hardwareconfiguratie van de linefollower (Arduino Nano, ATmega328P)
// =============================================================================
// Alles wat met de fysieke bedrading te maken heeft staat in dit ene bestand.
// Pas je de bedrading aan, pas dan ENKEL dit bestand aan.
//
// Bedrading zoals gebouwd (zie technische tekeningen/elektronisch/readme.md):
//
//   QTR-8A  OUT1..OUT8 -> A0..A7      LEDON -> D13
//   DRV8833 IN1 -> D6 (PWM)   IN2 -> D5 (PWM)   => motor op OUT1/OUT2
//           IN3 -> D3 (PWM)   IN4 -> D2 (GEEN PWM) => motor op OUT3/OUT4
//   HC-06   TX -> D7 (SoftwareSerial RX)   RX <- D8 via 1k/2k spanningsdeler
//   Start/stopknop -> D10 (externe pull-down 1k, knop naar +5 V)
//   LED (220 ohm) -> D12
//   D0/D1 blijven vrij voor USB (programmeren + debug)
// =============================================================================
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ---------------------------------------------------------------- versie ----
#define FIRMWARE_VERSION   "1.0"

// --------------------------------------------------------- communicatie ----
#define USB_BAUDRATE       115200UL   // USB-serieel (Serial Monitor / PuTTY via USB)
#define BT_BAUDRATE        9600UL     // HC-06 (bevestigd op 9600 baud)
#define PIN_BT_RX          7          // Arduino ontvangt  <- HC-06 TX
#define PIN_BT_TX          8          // Arduino zendt     -> HC-06 RX (via spanningsdeler)

// ---------------------------------------------------------------- sensor ----
#define NUM_SENSORS        8
#define SENSOR_PITCH_MM    9.525f     // QTR-8A: 0,375 inch tussen de sensoren
#define PIN_SENSOR_LEDON   13         // QTR-8A LEDON (ook de on-board LED van de Nano)

// Volgorde van de analoge ingangen van LINKS naar RECHTS, gezien in rijrichting.
// Controleer dit met het commando "raw": schuif de lijn onder de linkerkant
// van de robot -> de positie moet NEGATIEF worden.
// Staat de array omgekeerd gemonteerd, keer deze lijst dan om.
static const uint8_t SENSOR_PINS[NUM_SENSORS] = { A0, A1, A2, A3, A4, A5, A6, A7 };

// ---------------------------------------------------------------- motoren ---
// DRV8833 kanaal A: IN1/IN2 -> OUT1/OUT2
#define PIN_A_IN1          6          // PWM (Timer0)
#define PIN_A_IN2          5          // PWM (Timer0)
// DRV8833 kanaal B: IN3/IN4 -> OUT3/OUT4
#define PIN_B_IN3          3          // PWM (Timer2)
#define PIN_B_IN4          2          // GEEN PWM!  (wordt enkel digitaal geschakeld)

// Welke pinnen PWM kunnen. De motorcode gebruikt NOOIT analogWrite() op een
// pin waarvoor hier false staat.
#define PIN_A_IN1_PWM      true
#define PIN_A_IN2_PWM      true
#define PIN_B_IN3_PWM      true
#define PIN_B_IN4_PWM      false

// Welke kant van de robot op welk kanaal zit, en de draairichting, zijn
// INSTELBAAR via commando's (set swap / set invleft / set invright) en worden
// in EEPROM bewaard. Zo hoeft er niet opnieuw geflasht te worden.

// ------------------------------------------------------------ bediening -----
#define PIN_LED            12         // witte LED met 220 ohm
#define PIN_BUTTON         10         // start/stop (actief HOOG, externe pull-down)
#define BUTTON_DEBOUNCE_MS 50

// Externe interrupt:
//  - Op de Nano hebben enkel D2 (INT0) en D3 (INT1) een "externe interrupt".
//  - D10 heeft enkel een pin-change interrupt (PCINT2 / vector PCINT0_vect), maar
//    de SoftwareSerial-bibliotheek (nodig voor de HC-06 op D7/D8) claimt ALLE
//    pin-change vectoren. Een eigen PCINT-ISR op D10 geeft dan een linkfout.
//  - Daarom: knop op D10  -> knop wordt elke lus gepolld met debounce.
//            knop op D2/D3 -> echte externe interrupt (attachInterrupt).
//  - Aanbevolen hardwarewijziging (2 draadjes wisselen, zie readme):
//        knop  D10 -> D2      DRV8833 IN4  D2 -> D10
//    en dan hieronder PIN_BUTTON 2, PIN_B_IN4 10, PIN_B_IN4_PWM true.
#if (PIN_BUTTON == 2) || (PIN_BUTTON == 3)
  #define BUTTON_USES_INTERRUPT 1
#else
  #define BUTTON_USES_INTERRUPT 0
#endif

#if (PIN_BUTTON == PIN_B_IN4) || (PIN_BUTTON == PIN_B_IN3) || (PIN_BUTTON == PIN_A_IN1) || (PIN_BUTTON == PIN_A_IN2)
  #error "PIN_BUTTON valt samen met een motorpin - controleer config.h"
#endif

#endif
