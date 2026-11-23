// =============================================================================
// Linefollower - finale code
// HOGENT Elektromechanica - syntheseproject
// Arduino Nano (ATmega328P, 16 MHz) - QTR-8A - DRV8833 - 2x N20 6 V - HC-06
// =============================================================================
// Bestanden:
//   config.h          hardware / pinnen
//   settings.*        parameters + EEPROM (versie + checksum)
//   sensors.*         QTR-8A uitlezen, kalibratie, normalisatie, lijnpositie
//   controller.*      PID, snelheid in bochten, lijnverlies
//   motors.*          DRV8833
//   SerialCommand.*   commandoparser (template, verbeterd)
// Commando's: zie gebruiksaanwijzing/readme.md of typ "help".
// Dezelfde commando's werken via USB (115200 baud) en Bluetooth (9600 baud).
// =============================================================================
#include <SoftwareSerial.h>
#include "config.h"
#include "SerialCommand.h"
#include "settings.h"
#include "sensors.h"
#include "motors.h"
#include "controller.h"

SoftwareSerial bluetooth(PIN_BT_RX, PIN_BT_TX);   // RX, TX
SerialCommand usbCmd(Serial);
SerialCommand btCmd(bluetooth);

// type van de parametertabel (bovenaan: de Arduino IDE plaatst automatisch
// functieprototypes voor de eerste functie)
enum PType : uint8_t { T_U32, T_I16, T_U16, T_U8, T_F };
struct PDef { const char *name; uint8_t type; void *ptr; float minv, maxv; };

// ------------------------------------------------------------- toestand -----
static bool running = false;
static bool lostStop = false;          // gestopt omdat de lijn verloren was
static bool debugOn = false;
static Stream *debugPort = &Serial;
static bool eepromOk = false;

static uint32_t previous;              // [us] begin vorige cyclus
static uint32_t calcTimeMax;           // [us] langste rekentijd sinds laatste "show"
static uint32_t cycleMax;              // [us] langste gemeten cyclus sinds laatste "show"
static uint16_t overruns;              // aantal cycli waarbij de rekentijd > cyclustijd
static uint32_t lastDebug;

// ---------------------------------------------------------- start / stop ----
static void startRobot(Stream &out)
{
  if (running) return;
  controllerReset();
  running = true;
  lostStop = false;
  out.println(F("OK start"));
}

static void stopRobot(Stream &out)
{
  running = false;
  motorsStop();
  out.println(F("OK stop"));
}

// ------------------------------------------------------------ drukknop ------
#if BUTTON_USES_INTERRUPT
static volatile bool buttonFlag = false;
static volatile uint32_t buttonLast = 0;
static void buttonIsr()
{
  uint32_t now = millis();
  if (now - buttonLast > 250) { buttonFlag = true; buttonLast = now; }   // debounce
}
#endif

static bool buttonPressed()
{
#if BUTTON_USES_INTERRUPT
  if (buttonFlag) { buttonFlag = false; return true; }
  return false;
#else
  // Polling met debounce (zie config.h: externe interrupt op D10 is niet
  // mogelijk samen met SoftwareSerial).
  static bool stable = LOW, lastRead = LOW;
  static uint32_t changed = 0;
  bool r = digitalRead(PIN_BUTTON);
  uint32_t now = millis();
  if (r != lastRead) { lastRead = r; changed = now; }
  if (now - changed >= BUTTON_DEBOUNCE_MS && r != stable)
  {
    stable = r;
    if (stable == HIGH) return true;        // actief hoog (pull-down, knop naar +5 V)
  }
  return false;
#endif
}

// ----------------------------------------------------------------- LED ------
//  continu aan            : voeding aan, klaar (geen zwart onder de sensoren)
//  snel knipperen (10 Hz) : stilstaand en een sensor ziet zwart
//  rijden                 : aan zolang de lijn gezien wordt, uit bij lijnverlies
//  traag knipperen (1 Hz) : gestopt omdat de lijn verloren was
static void updateLed()
{
  uint32_t ms = millis();
  bool on;
  if (running) on = (sensors.type != LINE_NONE);
  else if (lostStop) on = (ms / 500) & 1;
  else if (sensors.type != LINE_NONE) on = (ms / 50) & 1;
  else on = true;
  digitalWrite(PIN_LED, on);
}

// ---------------------------------------------------- parametertabel --------

static const char n_cycle[] PROGMEM = "cycle";
static const char n_power[] PROGMEM = "power";
static const char n_max[] PROGMEM = "max";
static const char n_diff[] PROGMEM = "diff";
static const char n_slow[] PROGMEM = "slow";
static const char n_kp[] PROGMEM = "kp";
static const char n_ki[] PROGMEM = "ki";
static const char n_kd[] PROGMEM = "kd";
static const char n_dfilter[] PROGMEM = "dfilter";
static const char n_ilimit[] PROGMEM = "ilimit";
static const char n_balance[] PROGMEM = "balance";
static const char n_timeout[] PROGMEM = "timeout";
static const char n_threshold[] PROGMEM = "threshold";
static const char n_swap[] PROGMEM = "swap";
static const char n_invleft[] PROGMEM = "invleft";
static const char n_invright[] PROGMEM = "invright";
static const char n_reverse[] PROGMEM = "reverse";
static const char n_fastdecay[] PROGMEM = "fastdecay";
static const char n_ambient[] PROGMEM = "ambient";
static const char n_echo[] PROGMEM = "echo";
static const char n_dbgint[] PROGMEM = "dbgint";

static const PDef PARAMS[] PROGMEM = {
  { n_cycle,     T_U32, &params.cycleTime,     500, 50000 },
  { n_power,     T_I16, &params.power,         0,   255 },
  { n_max,       T_I16, &params.maxPwm,        0,   255 },
  { n_diff,      T_F,   &params.diff,          0,   1 },
  { n_slow,      T_F,   &params.slow,          0,   1 },
  { n_kp,        T_F,   &params.kp,            0,   999 },
  { n_ki,        T_F,   &params.ki,            0,   999 },
  { n_kd,        T_F,   &params.kd,            0,   999 },
  { n_dfilter,   T_F,   &params.dFilter,       0,   0.99f },
  { n_ilimit,    T_F,   &params.iLimit,        0,   255 },
  { n_balance,   T_I16, &params.balance,       -50, 50 },
  { n_timeout,   T_U16, &params.lostTimeout,   0,   5000 },
  { n_threshold, T_I16, &params.threshold,     50,  950 },
  { n_swap,      T_U8,  &params.swapMotors,    0,   1 },
  { n_invleft,   T_U8,  &params.invLeft,       0,   1 },
  { n_invright,  T_U8,  &params.invRight,      0,   1 },
  { n_reverse,   T_U8,  &params.allowReverse,  0,   1 },
  { n_fastdecay, T_U8,  &params.fastDecay,     0,   1 },
  { n_ambient,   T_U8,  &params.ambient,       0,   1 },
  { n_echo,      T_U8,  &params.echo,          0,   1 },
  { n_dbgint,    T_U16, &params.debugInterval, 20,  10000 },
};
static const uint8_t NUM_PARAMS = sizeof(PARAMS) / sizeof(PARAMS[0]);

static int8_t findParam(const char *name, PDef &out)
{
  if (name == NULL) return -1;
  for (uint8_t i = 0; i < NUM_PARAMS; i++)
  {
    memcpy_P(&out, &PARAMS[i], sizeof(PDef));
    if (strcmp_P(name, out.name) == 0) return i;
  }
  return -1;
}

static void printParamValue(Stream &out, const PDef &d)
{
  switch (d.type)
  {
    case T_U32: out.print(*(uint32_t *)d.ptr); break;
    case T_I16: out.print(*(int16_t *)d.ptr); break;
    case T_U16: out.print(*(uint16_t *)d.ptr); break;
    case T_U8:  out.print(*(uint8_t *)d.ptr); break;
    case T_F:   out.print(*(float *)d.ptr, 4); break;
  }
}

static void applyEcho()
{
  usbCmd.setEcho(params.echo);
  btCmd.setEcho(params.echo);
}

// Controleer of een tekst een volledig geldig getal is ("1.5", "-3", "on"/"off")
static bool parseNumber(const char *s, float &v)
{
  if (s == NULL) return false;
  if (strcmp(s, "on") == 0)  { v = 1; return true; }
  if (strcmp(s, "off") == 0) { v = 0; return true; }
  char *end;
  v = strtod(s, &end);
  return end != s && *end == '\0';
}

static void setParam(const char *name, const char *value, Stream &out)
{
  PDef d;
  if (findParam(name, d) < 0)
  {
    out.print(F("ERR onbekende parameter: "));
    out.println(name ? name : "(geen)");
    return;
  }
  float v;
  if (!parseNumber(value, v))
  {
    out.print(F("ERR ongeldige waarde voor "));
    out.println(name);
    return;
  }
  if (v < d.minv || v > d.maxv)
  {
    out.print(F("ERR "));
    out.print(name);
    out.print(F(" moet tussen "));
    out.print(d.minv, 2);
    out.print(F(" en "));
    out.print(d.maxv, 2);
    out.println(F(" liggen"));
    return;
  }
  switch (d.type)
  {
    case T_U32: *(uint32_t *)d.ptr = (uint32_t)(v + 0.5f); break;
    case T_I16: *(int16_t *)d.ptr  = (int16_t)(v < 0 ? v - 0.5f : v + 0.5f); break;
    case T_U16: *(uint16_t *)d.ptr = (uint16_t)(v + 0.5f); break;
    case T_U8:  *(uint8_t *)d.ptr  = (uint8_t)(v + 0.5f); break;
    case T_F:   *(float *)d.ptr    = v; break;
  }
  if (d.ptr == &params.echo) applyEcho();
  out.print(F("OK "));
  out.print(name);
  out.print(F(" = "));
  printParamValue(out, d);
  out.println(F("   (typ 'save' om te bewaren)"));
}

// ----------------------------------------------------------- weergave -------
static void printArray(Stream &out, const __FlashStringHelper *label, const int16_t *a)
{
  out.print(label);
  for (uint8_t i = 0; i < NUM_SENSORS; i++) { out.print(a[i]); out.print(' '); }
  out.println();
}

static void printAllParams(Stream &out, const Params &p)
{
  // De pointers in de tabel wijzen naar 'params'; voor een andere struct
  // (bv. de EEPROM-inhoud) wordt de offset omgerekend.
  PDef d;
  for (uint8_t i = 0; i < NUM_PARAMS; i++)
  {
    memcpy_P(&d, &PARAMS[i], sizeof(PDef));
    d.ptr = (uint8_t *)&p + ((uint8_t *)d.ptr - (uint8_t *)&params);
    out.print(F("  "));
    out.print((const __FlashStringHelper *)d.name);
    out.print(F(" = "));
    printParamValue(out, d);
    out.println();
  }
  printArray(out, F("  wit  : "), p.white);
  printArray(out, F("  zwart: "), p.black);
}

static void cmdShow()
{
  Stream &out = SerialCommand::reply();
  out.print(F("Linefollower v" FIRMWARE_VERSION " - "));
  out.println(running ? F("RIJDT") : F("gestopt"));
  printAllParams(out, params);
  out.print(F("  EEPROM bij opstart: "));
  out.println(eepromOk ? F("geldig geladen") : F("leeg/ongeldig -> standaardwaarden"));
  out.print(F("  knop: D"));
  out.print(PIN_BUTTON);
  out.println(BUTTON_USES_INTERRUPT ? F(" (externe interrupt)") : F(" (polling + debounce)"));
  out.print(F("  rekentijd max [us]: "));
  out.print(calcTimeMax);
  out.print(F("  cyclus max [us]: "));
  out.print(cycleMax);
  out.print(F("  overschrijdingen: "));
  out.println(overruns);
  calcTimeMax = cycleMax = 0;
  overruns = 0;
}

static void cmdHelp()
{
  Stream &out = SerialCommand::reply();
  out.println(F("start | run | stop | show | debug [on|off] | raw | help"));
  out.println(F("set <param> <waarde>   of   <param>=<waarde>"));
  out.println(F("calibrate white | black | auto [s] | check"));
  out.println(F("save | load | defaults | eeprom"));
  out.println(F("test left|right|a|b <pwm -255..255> [ms]"));
  out.print(F("param: "));
  PDef d;
  for (uint8_t i = 0; i < NUM_PARAMS; i++)
  {
    memcpy_P(&d, &PARAMS[i], sizeof(PDef));
    out.print((const __FlashStringHelper *)d.name);
    out.print(' ');
  }
  out.println();
}

// --------------------------------------------------------- commando's -------
static void cmdSet()
{
  SerialCommand *c = SerialCommand::activeCommand();
  char *name = c->next();
  char *value = c->next();
  setParam(name, value, SerialCommand::reply());
}

static void cmdStart() { startRobot(SerialCommand::reply()); }
static void cmdStop()  { stopRobot(SerialCommand::reply()); lostStop = false; }

static void cmdDebug()
{
  Stream &out = SerialCommand::reply();
  char *arg = SerialCommand::activeCommand()->next();
  if (arg == NULL) { cmdShow(); return; }          // zoals in de template: "debug" toont alles
  if (strcmp(arg, "on") == 0)
  {
    debugOn = true;
    debugPort = &out;
    out.println(F("OK debug on  (e P I D u L R toestand)"));
    if (&out == &bluetooth)
      out.println(F("WARN debug via Bluetooth blokkeert ~1 ms per teken; verhoog dbgint tijdens het rijden"));
  }
  else if (strcmp(arg, "off") == 0)
  {
    debugOn = false;
    out.println(F("OK debug off"));
  }
  else out.println(F("ERR gebruik: debug on | debug off"));
}

static void cmdRaw()
{
  Stream &out = SerialCommand::reply();
  sensorsRead(params.ambient);
  printArray(out, F("raw : "), sensors.raw);
  printArray(out, F("norm: "), sensors.norm);
  out.print(F("positie [mm]: "));
  out.print(sensors.position, 1);
  out.print(F("  zwart: "));
  out.print(sensors.blackCount);
  out.print(F("  type: "));
  out.println(sensors.type == LINE_NONE ? F("geen lijn") : sensors.type == LINE_CROSSING ? F("kruispunt") : F("lijn"));
}

static void cmdCalibrate()
{
  Stream &out = SerialCommand::reply();
  char *arg = SerialCommand::activeCommand()->next();
  if (running) stopRobot(out);
  motorsStop();

  if (arg && strcmp(arg, "white") == 0)
  {
    out.print(F("kalibratie wit (alle sensoren boven WIT)... "));
    calibrateAverage(params.white, out);
    calibrationCheck(out);
  }
  else if (arg && strcmp(arg, "black") == 0)
  {
    out.print(F("kalibratie zwart (alle sensoren boven ZWART)... "));
    calibrateAverage(params.black, out);
    calibrationCheck(out);
  }
  else if (arg && strcmp(arg, "auto") == 0)
  {
    float s = 5;
    char *sec = SerialCommand::activeCommand()->next();
    if (sec && (!parseNumber(sec, s) || s < 1 || s > 30)) { out.println(F("ERR duur 1..30 s")); return; }
    out.print(F("kalibratie auto: beweeg de sensorarray "));
    out.print((int)s);
    out.println(F(" s lang heen en weer over de lijn (elke sensor over zwart EN wit)"));
    calibrateSweepStart();
    uint32_t t0 = millis();
    while (millis() - t0 < (uint32_t)(s * 1000))
    {
      calibrateSweepSample();
      digitalWrite(PIN_LED, (millis() / 125) & 1);
      delay(2);
    }
    calibrateSweepFinish(out);
    calibrationCheck(out);
  }
  else if (arg && strcmp(arg, "check") == 0)
  {
    calibrationCheck(out);
    return;
  }
  else
  {
    out.println(F("ERR gebruik: calibrate white | black | auto [s] | check"));
    return;
  }
  out.println(F("(typ 'save' om de kalibratie te bewaren)"));
}

static void cmdSave()
{
  Stream &out = SerialCommand::reply();
  if (!settingsValid(params)) { out.println(F("ERR parameters ongeldig, niet bewaard")); return; }
  settingsSave();
  out.println(F("OK bewaard in EEPROM"));
}

static void cmdLoad()
{
  Stream &out = SerialCommand::reply();
  if (settingsLoad()) out.println(F("OK geladen uit EEPROM"));
  else out.println(F("ERR EEPROM leeg/ongeldig -> standaardwaarden geladen"));
  applyEcho();
}

static void cmdDefaults()
{
  settingsDefaults();
  applyEcho();
  SerialCommand::reply().println(F("OK standaardwaarden geladen (niet bewaard; kalibratie opnieuw doen!)"));
}

static void cmdEeprom()
{
  Stream &out = SerialCommand::reply();
  Params stored;
  uint16_t version;
  if (!settingsStoredInfo(stored, version))
  {
    out.print(F("EEPROM leeg of ongeldig (versie "));
    out.print(version);
    out.println(F(")"));
    return;
  }
  out.print(F("EEPROM inhoud (versie "));
  out.print(version);
  out.println(F("):"));
  printAllParams(out, stored);
}

static void cmdTest()
{
  Stream &out = SerialCommand::reply();
  SerialCommand *c = SerialCommand::activeCommand();
  char *which = c->next();
  char *pwmS = c->next();
  char *msS = c->next();
  float pwm, ms = 1000;
  if (which == NULL || !parseNumber(pwmS, pwm) || pwm < -255 || pwm > 255 ||
      (msS && (!parseNumber(msS, ms) || ms < 0 || ms > 10000)))
  {
    out.println(F("ERR gebruik: test left|right|a|b <pwm -255..255> [ms]"));
    return;
  }
  if (running) stopRobot(out);
  int16_t p = (int16_t)pwm;
  if (strcmp(which, "left") == 0)       motorsSet(p, 0);
  else if (strcmp(which, "right") == 0) motorsSet(0, p);
  else if (strcmp(which, "both") == 0)  motorsSet(p, p);
  else if (strcmp(which, "a") == 0)     motorChannelRaw(0, p);   // OUT1/OUT2, zonder swap/invert
  else if (strcmp(which, "b") == 0)     motorChannelRaw(1, p);   // OUT3/OUT4, zonder swap/invert
  else { out.println(F("ERR kies left, right, both, a of b")); return; }
  out.println(F("motor draait..."));
  delay((uint32_t)ms);
  motorsStop();
  out.println(F("OK test klaar"));
}

// "kp=2.5", "power 120", ... zonder 'set' ervoor
static void cmdUnknown(char *token)
{
  Stream &out = SerialCommand::reply();
  PDef d;
  if (findParam(token, d) >= 0)
  {
    char *value = SerialCommand::activeCommand()->next();
    if (value == NULL)                       // enkel de naam: waarde tonen
    {
      out.print(token);
      out.print(F(" = "));
      printParamValue(out, d);
      out.println();
    }
    else setParam(token, value, out);
    return;
  }
  out.print(F("ERR onbekend commando: \""));
  out.print(token);
  out.println(F("\"  (typ 'help')"));
}

// ------------------------------------------------------------- debug --------
static void printDebug()
{
  Stream &out = *debugPort;
  out.print(F("e=")); out.print(ctrl.error, 1);
  out.print(F(" P=")); out.print(ctrl.p, 1);
  out.print(F(" I=")); out.print(ctrl.i, 1);
  out.print(F(" D=")); out.print(ctrl.d, 1);
  out.print(F(" u=")); out.print(ctrl.u, 1);
  out.print(F(" L=")); out.print(ctrl.left);
  out.print(F(" R=")); out.print(ctrl.right);
  out.print(' ');
  if (running) out.print(lineStateName(ctrl.state));
  else { out.print(F("pos=")); out.print(sensors.position, 1); out.print(F(" n=")); out.print(sensors.blackCount); }
  out.print(F(" ct=")); out.println(calcTimeMax);
}

// ------------------------------------------------------------- setup --------
void setup()
{
  motorsBegin();                       // eerst: motoren gegarandeerd stil
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);
  pinMode(PIN_BUTTON, INPUT);          // externe pull-down aanwezig
#if BUTTON_USES_INTERRUPT
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), buttonIsr, RISING);
#endif

  Serial.begin(USB_BAUDRATE);
  bluetooth.begin(BT_BAUDRATE);

  eepromOk = settingsLoad();
  sensorsBegin();

  SerialCommand::addCommand("set", cmdSet);
  SerialCommand::addCommand("start", cmdStart);
  SerialCommand::addCommand("run", cmdStart);
  SerialCommand::addCommand("stop", cmdStop);
  SerialCommand::addCommand("show", cmdShow);
  SerialCommand::addCommand("debug", cmdDebug);
  SerialCommand::addCommand("help", cmdHelp);
  SerialCommand::addCommand("raw", cmdRaw);
  SerialCommand::addCommand("calibrate", cmdCalibrate);
  SerialCommand::addCommand("save", cmdSave);
  SerialCommand::addCommand("load", cmdLoad);
  SerialCommand::addCommand("defaults", cmdDefaults);
  SerialCommand::addCommand("eeprom", cmdEeprom);
  SerialCommand::addCommand("test", cmdTest);
  SerialCommand::setDefaultHandler(cmdUnknown);
  applyEcho();

  const __FlashStringHelper *msg = eepromOk ? F("ready (EEPROM geladen)") : F("ready (EEPROM leeg/ongeldig: standaardwaarden - kalibreer en 'save')");
  Serial.println(msg);
  bluetooth.println(msg);

  previous = micros();
}

// -------------------------------------------------------------- loop --------
void loop()
{
  usbCmd.readSerial();
  btCmd.readSerial();

  if (buttonPressed())
  {
    if (running) { stopRobot(Serial); bluetooth.println(F("stop (knop)")); }
    else         { startRobot(Serial); bluetooth.println(F("start (knop)")); }
  }

  uint32_t now = micros();
  uint32_t elapsed = now - previous;
  if (elapsed >= params.cycleTime)
  {
    previous = now;
    if (elapsed > cycleMax) cycleMax = elapsed;

    sensorsRead(params.ambient);
    if (running)
    {
      float dt = elapsed * 1e-6f;                 // werkelijke cyclustijd in seconden
      if (!controllerStep(dt))
      {
        running = false;
        lostStop = true;
        motorsStop();
        Serial.println(F("STOP lijn verloren"));
        bluetooth.println(F("STOP lijn verloren"));
      }
    }
    updateLed();

    uint32_t calc = micros() - now;
    if (calc > calcTimeMax) calcTimeMax = calc;
    if (calc > params.cycleTime) overruns++;
  }

  if (debugOn && millis() - lastDebug >= params.debugInterval)
  {
    lastDebug = millis();
    printDebug();
  }
}
