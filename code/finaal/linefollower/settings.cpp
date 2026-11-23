#include "settings.h"
#include <EEPROM.h>

Params params;

// Layout in EEPROM:  [magic:2][version:1][Params][checksum:1]
struct Header { uint16_t magic; uint8_t version; };

static const int ADDR_HEADER = 0;
static const int ADDR_PARAMS = sizeof(Header);
static const int ADDR_CRC    = ADDR_PARAMS + sizeof(Params);

static uint8_t crc8(const uint8_t *data, size_t len)
{
  uint8_t crc = 0;
  while (len--)
  {
    crc ^= *data++;
    for (uint8_t i = 0; i < 8; i++) crc = (crc & 0x80) ? (crc << 1) ^ 0x07 : (crc << 1);
  }
  return crc;
}

void settingsDefaults()
{
  // Bewust voorzichtige startwaarden - GEEN bewezen optimum. Afregelen volgens
  // gebruiksaanwijzing/readme.md.
  params.cycleTime     = 2500;    // 400 Hz
  params.power         = 90;
  params.diff          = 0.5f;
  params.kp            = 2.5f;    // 33 mm fout -> ~80 PWM correctie
  params.ki            = 0.0f;
  params.kd            = 0.05f;
  params.maxPwm        = 255;     // motoren krijgen 6,0 V van de DSN-MINI-360
  params.slow          = 0.3f;
  params.dFilter       = 0.5f;
  params.iLimit        = 60.0f;
  params.balance       = 0;
  params.lostTimeout   = 400;
  params.threshold     = 500;
  params.swapMotors    = 0;
  params.invLeft       = 0;
  params.invRight      = 0;
  params.allowReverse  = 1;
  params.fastDecay     = 0;
  params.ambient       = 0;
  params.echo          = 1;
  params.debugInterval = 100;
  for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    params.white[i] = 50;         // historische meting wit  - altijd opnieuw kalibreren!
    params.black[i] = 700;        // historische meting zwart
  }
}

bool settingsValid(const Params &p)
{
  if (p.cycleTime < 500 || p.cycleTime > 50000) return false;
  if (p.power < 0 || p.power > 255) return false;
  if (p.maxPwm < 0 || p.maxPwm > 255) return false;
  if (!(p.diff >= 0 && p.diff <= 1)) return false;      // vangt ook NaN
  if (!(p.slow >= 0 && p.slow <= 1)) return false;
  if (!(p.dFilter >= 0 && p.dFilter < 1)) return false;
  if (!(p.kp >= 0 && p.kp < 1000)) return false;
  if (!(p.ki >= 0 && p.ki < 1000)) return false;
  if (!(p.kd >= 0 && p.kd < 1000)) return false;
  if (!(p.iLimit >= 0 && p.iLimit <= 255)) return false;
  if (p.balance < -50 || p.balance > 50) return false;
  if (p.threshold < 50 || p.threshold > 950) return false;
  if (p.debugInterval < 20 || p.debugInterval > 10000) return false;
  for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    if (p.white[i] < 0 || p.white[i] > 1023) return false;
    if (p.black[i] < 0 || p.black[i] > 1023) return false;
  }
  return true;
}

bool settingsStoredInfo(Params &out, uint16_t &version)
{
  Header h;
  EEPROM.get(ADDR_HEADER, h);
  version = h.version;
  if (h.magic != SETTINGS_MAGIC || h.version != SETTINGS_VERSION) return false;
  EEPROM.get(ADDR_PARAMS, out);
  if (EEPROM.read(ADDR_CRC) != crc8((const uint8_t *)&out, sizeof(Params))) return false;
  return settingsValid(out);
}

bool settingsLoad()
{
  Params tmp;
  uint16_t version;
  if (!settingsStoredInfo(tmp, version))
  {
    settingsDefaults();
    return false;
  }
  params = tmp;
  return true;
}

void settingsSave()
{
  Header h = { SETTINGS_MAGIC, SETTINGS_VERSION };
  EEPROM.put(ADDR_HEADER, h);                  // put() gebruikt update(): enkel gewijzigde bytes
  EEPROM.put(ADDR_PARAMS, params);
  EEPROM.update(ADDR_CRC, crc8((const uint8_t *)&params, sizeof(Params)));
}
