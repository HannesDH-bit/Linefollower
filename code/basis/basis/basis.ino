// =============================================================================
// Basisprogramma (HOGENT-template) aangepast aan deze linefollower
// =============================================================================
// Wat samenwerkt:
//   * instelbare parameters met SerialCommand: cyclustijd, debug on/off, start/stop
//   * parameters in EEPROM (magic + versie; enkel bij "save")
//   * start/stop met de drukknop
//   * draadloze communicatie: zelfde commando's via USB (115200) en HC-06 (9600)
// Met de cyclus wordt een knipperende LED gemaakt: tijdens "run" wisselt de
// LED elke 'blink' cycli van toestand (knipperfrequentie = 1 / (2*blink*cycle)).
//
// Commando's:  run | start | stop | debug | debug on | debug off |
//              set cycle <us> | set blink <cycli> | save
//
// Drukknop: D10 heeft geen INT0/INT1 en SoftwareSerial bezet alle
// pin-change vectoren -> op D10 wordt de knop gepolld met debounce.
// Na de aanbevolen omwisseling (knop op D2) wordt automatisch de externe
// interrupt INT0 gebruikt. Zie code/finaal/readme.md.
// =============================================================================
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include "SerialCommand.h"

#define PIN_LED     12
#define PIN_BUTTON  10        // 2 na de omwisseling knop <-> IN4
#define PIN_BT_RX   7
#define PIN_BT_TX   8

SoftwareSerial bluetooth(PIN_BT_RX, PIN_BT_TX);
SerialCommand usbCmd(Serial);
SerialCommand btCmd(bluetooth);

struct Params
{
  uint16_t magic;
  uint8_t  version;
  uint32_t cycleTime;     // [us]
  uint16_t blink;         // [cycli]
} params;

const uint16_t MAGIC = 0x4C42;   // "LB"
const uint8_t VERSION = 1;

bool running = false;
bool debugOn = false;
Stream *debugPort = &Serial;
uint32_t previous, calcMax;
uint16_t cycleCount;

#if (PIN_BUTTON == 2) || (PIN_BUTTON == 3)
volatile bool buttonFlag = false;
volatile uint32_t buttonLast = 0;
void buttonIsr()
{
  uint32_t now = millis();
  if (now - buttonLast > 250) { buttonFlag = true; buttonLast = now; }
}
bool buttonPressed() { if (buttonFlag) { buttonFlag = false; return true; } return false; }
#else
bool buttonPressed()
{
  static bool stable = LOW, lastRead = LOW;
  static uint32_t changed = 0;
  bool r = digitalRead(PIN_BUTTON);
  if (r != lastRead) { lastRead = r; changed = millis(); }
  if (millis() - changed >= 50 && r != stable) { stable = r; return stable == HIGH; }
  return false;
}
#endif

void loadParams()
{
  EEPROM.get(0, params);
  if (params.magic != MAGIC || params.version != VERSION ||
      params.cycleTime < 100 || params.cycleTime > 1000000UL || params.blink == 0)
  {
    params.magic = MAGIC; params.version = VERSION;
    params.cycleTime = 10000;   // 10 ms
    params.blink = 25;          // 25 x 10 ms = 250 ms aan / 250 ms uit = 2 Hz
    Serial.println(F("EEPROM leeg/ongeldig -> standaardwaarden"));
  }
}

void onRun()   { running = true;  SerialCommand::reply().println(F("OK run")); }
void onStop()  { running = false; digitalWrite(PIN_LED, LOW); SerialCommand::reply().println(F("OK stop")); }
void onSave()  { EEPROM.put(0, params); SerialCommand::reply().println(F("OK bewaard")); }

void onSet()
{
  Stream &out = SerialCommand::reply();
  char *p = SerialCommand::activeCommand()->next();
  char *v = SerialCommand::activeCommand()->next();
  long value = v ? atol(v) : -1;
  if (p && strcmp(p, "cycle") == 0 && value >= 100 && value <= 1000000L) params.cycleTime = value;
  else if (p && strcmp(p, "blink") == 0 && value >= 1 && value <= 10000) params.blink = value;
  else { out.println(F("ERR gebruik: set cycle <100..1000000> | set blink <1..10000>")); return; }
  out.print(F("OK ")); out.print(p); out.print(F(" = ")); out.println(value);
}

void onDebug()
{
  Stream &out = SerialCommand::reply();
  char *arg = SerialCommand::activeCommand()->next();
  if (arg && strcmp(arg, "on") == 0)  { debugOn = true; debugPort = &out; out.println(F("OK debug on")); return; }
  if (arg && strcmp(arg, "off") == 0) { debugOn = false; out.println(F("OK debug off")); return; }
  out.print(F("cycle time [us]: ")); out.println(params.cycleTime);
  out.print(F("blink [cycli]: "));   out.println(params.blink);
  out.print(F("running: "));         out.println(running);
  out.print(F("calculation time max [us]: ")); out.println(calcMax);
  calcMax = 0;
}

void onUnknown(char *cmd)
{
  SerialCommand::reply().print(F("ERR onbekend commando: "));
  SerialCommand::reply().println(cmd);
}

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
#if (PIN_BUTTON == 2) || (PIN_BUTTON == 3)
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), buttonIsr, RISING);
#endif
  Serial.begin(115200);
  bluetooth.begin(9600);
  loadParams();

  SerialCommand::addCommand("run", onRun);
  SerialCommand::addCommand("start", onRun);
  SerialCommand::addCommand("stop", onStop);
  SerialCommand::addCommand("set", onSet);
  SerialCommand::addCommand("debug", onDebug);
  SerialCommand::addCommand("save", onSave);
  SerialCommand::setDefaultHandler(onUnknown);

  Serial.println(F("ready"));
  bluetooth.println(F("ready"));
  previous = micros();
}

void loop()
{
  usbCmd.readSerial();
  btCmd.readSerial();
  if (buttonPressed())
  {
    running = !running;
    if (!running) digitalWrite(PIN_LED, LOW);
    Serial.println(running ? F("run (knop)") : F("stop (knop)"));
  }

  uint32_t now = micros();
  if (now - previous >= params.cycleTime)
  {
    previous = now;
    // ---- code die cyclisch uitgevoerd moet worden ----
    if (running && ++cycleCount >= params.blink)
    {
      cycleCount = 0;
      digitalWrite(PIN_LED, !digitalRead(PIN_LED));
      if (debugOn) debugPort->println(digitalRead(PIN_LED) ? F("led aan") : F("led uit"));
    }
    // ---------------------------------------------------
    uint32_t calc = micros() - now;
    if (calc > calcMax) calcMax = calc;
  }
}
