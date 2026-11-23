// =============================================================================
// sensors.h - QTR-8A: uitlezen, kalibreren, normaliseren, lijnpositie
// =============================================================================
// QTR-8A (Pololu #960): veel reflectie (wit) -> lage spanning,
//                       weinig reflectie (zwart) -> hoge spanning.
// Normalisatie per sensor:  0 = wit (gekalibreerd), 1000 = zwart (gekalibreerd)
// =============================================================================
#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include "config.h"

#define POSITION_MAX_MM  (3.5f * SENSOR_PITCH_MM)   // 33,3 mm: buitenste sensor

enum LineType : uint8_t
{
  LINE_NONE = 0,      // geen enkele sensor ziet zwart
  LINE_NORMAL,        // gewone lijn
  LINE_CROSSING       // (bijna) alle sensoren zwart: kruispunt / dwarse lijn
};

struct SensorData
{
  int16_t  raw[NUM_SENSORS];
  int16_t  norm[NUM_SENSORS];
  float    position;      // [mm] - = lijn links van het midden, + = rechts
  uint8_t  blackCount;    // aantal sensoren boven de drempel
  LineType type;
};

extern SensorData sensors;

void sensorsBegin();
void sensorsRead(bool ambientCompensation);   // ruwe waarden + normalisatie + positie
void sensorsUpdatePosition(int16_t threshold);

// Kalibratie
void calibrateAverage(int16_t *target, Stream &out);   // 32 metingen middelen (wit of zwart)
void calibrateSweepStart();                            // min/max-zoektocht starten
void calibrateSweepSample();                           // tijdens sweep oproepen
void calibrateSweepFinish(Stream &out);                // min -> wit, max -> zwart
uint8_t calibrationCheck(Stream &out);                 // aantal sensoren met te klein bereik

#define CAL_MIN_RANGE  150   // minimaal verschil zwart-wit (ADC-eenheden) per sensor

#endif
