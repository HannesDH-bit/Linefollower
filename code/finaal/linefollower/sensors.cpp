#include "sensors.h"
#include "settings.h"

SensorData sensors;

static int16_t sweepMin[NUM_SENSORS];
static int16_t sweepMax[NUM_SENSORS];

void sensorsBegin()
{
  pinMode(PIN_SENSOR_LEDON, OUTPUT);
  digitalWrite(PIN_SENSOR_LEDON, HIGH);       // IR-leds aan
  sensors.position = 0;
  sensors.type = LINE_NONE;
}

static void readRaw(int16_t *dest)
{
  for (uint8_t i = 0; i < NUM_SENSORS; i++) dest[i] = analogRead(SENSOR_PINS[i]);
}

void sensorsRead(bool ambientCompensation)
{
  if (ambientCompensation)
  {
    // Meting met IR-leds uit = enkel omgevingslicht. Het verschil (uit - aan)
    // is het deel dat door de eigen IR-led veroorzaakt wordt. Omgerekend naar
    // dezelfde schaal als zonder compensatie (hoog = zwart).
    // VOORSTEL - nog te testen onder zon-/TL-licht.
    int16_t off[NUM_SENSORS];
    digitalWrite(PIN_SENSOR_LEDON, LOW);
    delayMicroseconds(200);
    readRaw(off);
    digitalWrite(PIN_SENSOR_LEDON, HIGH);
    delayMicroseconds(200);
    readRaw(sensors.raw);
    for (uint8_t i = 0; i < NUM_SENSORS; i++)
    {
      int16_t v = 1023 - (off[i] - sensors.raw[i]);
      sensors.raw[i] = constrain(v, 0, 1023);
    }
  }
  else
  {
    readRaw(sensors.raw);
  }

  for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    int16_t w = params.white[i];
    int16_t b = params.black[i];
    if (b - w < 1) { sensors.norm[i] = 0; continue; }          // ongeldige kalibratie
    int32_t n = ((int32_t)(sensors.raw[i] - w) * 1000L) / (b - w);
    sensors.norm[i] = (int16_t)constrain(n, 0L, 1000L);
  }

  sensorsUpdatePosition(params.threshold);
}

// Lijnpositie: zwartste sensor zoeken en een parabool door die sensor en
// zijn twee buren leggen (zelfde methode als de HOGENT-template, maar met
// zwart = 1000). Geeft sub-sensor-resolutie; een lijn van 15 mm valt altijd
// over 1 a 2 sensoren (pitch 9,525 mm).
void sensorsUpdatePosition(int16_t threshold)
{
  uint8_t idx = 0;
  uint8_t count = 0;
  for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    if (sensors.norm[i] > sensors.norm[idx]) idx = i;
    if (sensors.norm[i] >= threshold) count++;
  }
  sensors.blackCount = count;

  if (sensors.norm[idx] < threshold) { sensors.type = LINE_NONE; return; }   // positie ongewijzigd
  sensors.type = (count >= NUM_SENSORS - 2) ? LINE_CROSSING : LINE_NORMAL;

  float offset = 0;
  if (idx > 0 && idx < NUM_SENSORS - 1)
  {
    float s0 = sensors.norm[idx - 1];
    float s1 = sensors.norm[idx];
    float s2 = sensors.norm[idx + 1];
    float denom = s0 - 2.0f * s1 + s2;                // < 0 bij een echte top
    if (denom < -1.0f) offset = 0.5f * (s0 - s2) / denom;
    else offset = (s2 > s0) ? 0.5f : (s2 < s0 ? -0.5f : 0);   // vlakke top (2 sensoren verzadigd)
    offset = constrain(offset, -0.5f, 0.5f);
  }
  else if (idx == 0)                 // buitenste sensor: enkel naar binnen interpoleren
  {
    // Lijn ligt op of voorbij sensor 1; buiten de array kan niet gemeten worden.
    float a = sensors.norm[0], b = sensors.norm[1];
    offset = b / (a + b);           // 0 .. 0,5
  }
  else
  {
    float a = sensors.norm[NUM_SENSORS - 1], b = sensors.norm[NUM_SENSORS - 2];
    offset = -b / (a + b);          // -0,5 .. 0
  }

  sensors.position = ((float)idx + offset - 3.5f) * SENSOR_PITCH_MM;
}

// ------------------------------------------------------------- kalibratie ---
void calibrateAverage(int16_t *target, Stream &out)
{
  int32_t sum[NUM_SENSORS] = { 0 };
  int16_t tmp[NUM_SENSORS];
  for (uint8_t n = 0; n < 32; n++)
  {
    readRaw(tmp);
    for (uint8_t i = 0; i < NUM_SENSORS; i++) sum[i] += tmp[i];
    delay(2);
  }
  for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    target[i] = sum[i] / 32;
    out.print(target[i]);
    out.print(' ');
  }
  out.println();
}

void calibrateSweepStart()
{
  for (uint8_t i = 0; i < NUM_SENSORS; i++) { sweepMin[i] = 1023; sweepMax[i] = 0; }
}

void calibrateSweepSample()
{
  int16_t tmp[NUM_SENSORS];
  readRaw(tmp);
  for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    if (tmp[i] < sweepMin[i]) sweepMin[i] = tmp[i];
    if (tmp[i] > sweepMax[i]) sweepMax[i] = tmp[i];
  }
}

void calibrateSweepFinish(Stream &out)
{
  for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    params.white[i] = sweepMin[i];
    params.black[i] = sweepMax[i];
  }
  out.print(F("wit : ")); for (uint8_t i = 0; i < NUM_SENSORS; i++) { out.print(params.white[i]); out.print(' '); } out.println();
  out.print(F("zwart: ")); for (uint8_t i = 0; i < NUM_SENSORS; i++) { out.print(params.black[i]); out.print(' '); } out.println();
}

uint8_t calibrationCheck(Stream &out)
{
  uint8_t bad = 0;
  for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    int16_t range = params.black[i] - params.white[i];
    if (range < CAL_MIN_RANGE)
    {
      out.print(F("WARN sensor "));
      out.print(i + 1);
      out.print(F(": bereik zwart-wit = "));
      out.print(range);
      out.print(F(" (< "));
      out.print(CAL_MIN_RANGE);
      out.println(F("), is deze sensor over zwart EN wit bewogen?"));
      bad++;
    }
  }
  if (bad == 0) out.println(F("OK kalibratie: alle sensoren hebben voldoende bereik"));
  return bad;
}
