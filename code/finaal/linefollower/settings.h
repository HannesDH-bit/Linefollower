// =============================================================================
// settings.h - alle instelbare parameters + EEPROM-opslag
// =============================================================================
// Eenheden (belangrijk om Kp/Ki/Kd te kunnen interpreteren):
//   lijnfout e        : millimeter  (-33,3 mm ... +33,3 mm, + = lijn rechts)
//   PID-uitgang u     : PWM-eenheden (0..255 per motor)
//   kp                : PWM per mm
//   ki                : PWM per (mm * s)
//   kd                : PWM per (mm / s)
//   cyclustijd        : microseconden
// Omdat de PID met echte seconden rekent, hoeven kp/ki/kd NIET aangepast te
// worden als de cyclustijd verandert (anders dan in de basistemplate).
// =============================================================================
#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include "config.h"

#define SETTINGS_MAGIC    0x4C46      // "LF"
#define SETTINGS_VERSION  1           // verhogen als de struct verandert

struct Params
{
  // --- template-parameters
  uint32_t cycleTime;     // [us]   regelcyclus
  int16_t  power;         // [0..255] basissnelheid (PWM)
  float    diff;          // [0..1] verdeling stuurcorrectie: deel dat bij het buitenwiel opgeteld wordt
  float    kp;            // [PWM/mm]
  float    ki;            // [PWM/(mm*s)]
  float    kd;            // [PWM/(mm/s)]
  // --- uitbreidingen
  int16_t  maxPwm;        // [0..255] absolute PWM-limiet per motor
  float    slow;          // [0..1] vertraging in bochten: power * (1 - slow * |e|/emax)
  float    dFilter;       // [0..1) laagdoorlaatfilter D-term (0 = geen filter)
  float    iLimit;        // [PWM] maximale bijdrage van de I-term (anti-windup)
  int16_t  balance;       // [-50..50] % snelheidscorrectie links/rechts (+ = linkermotor sneller)
  uint16_t lostTimeout;   // [ms] hoe lang zoeken na lijnverlies voor de robot stopt
  int16_t  threshold;     // [0..1000] genormaliseerde waarde vanaf waar een sensor "zwart" ziet
  uint8_t  swapMotors;    // 0: linkermotor = kanaal A (OUT1/OUT2), 1: linkermotor = kanaal B (OUT3/OUT4)
  uint8_t  invLeft;       // 1: draairichting linkermotor omkeren
  uint8_t  invRight;      // 1: draairichting rechtermotor omkeren
  uint8_t  allowReverse;  // 1: binnenwiel mag achteruit draaien in scherpe bochten
  uint8_t  fastDecay;     // 1: fast decay waar mogelijk (0 = slow decay waar mogelijk)
  uint8_t  ambient;       // 1: omgevingslichtcompensatie (LEDON uit/aan meting)
  uint8_t  echo;          // 1: ontvangen commando terugsturen
  uint16_t debugInterval; // [ms] interval van de debuglijnen
  // --- kalibratie (ruwe ADC-waarden 0..1023)
  int16_t  white[NUM_SENSORS];
  int16_t  black[NUM_SENSORS];
};

extern Params params;

void settingsDefaults();                  // veilige standaardwaarden in RAM
bool settingsLoad();                      // true = geldige EEPROM-gegevens geladen
void settingsSave();                      // schrijft enkel gewijzigde bytes
bool settingsValid(const Params &p);      // bereikcontrole
bool settingsStoredInfo(Params &out, uint16_t &version);  // lees EEPROM zonder te laden

#endif
