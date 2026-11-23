// =============================================================================
// Proof of concept draadloze communicatie - HC-06 (Bluetooth Classic, SPP)
// Duplex communicatie tussen Arduino Nano en laptop (PuTTY op de Bluetooth-COM-poort)
// =============================================================================
// Bedrading:
//   HC-06 TX  -> D7  (SoftwareSerial RX)
//   HC-06 RX  <- D8  via spanningsdeler: D8 - 1 kOhm - knooppunt - 2 kOhm - GND,
//                     HC-06 RX op het knooppunt (5 V -> 3,3 V)
//   HC-06 VCC -> 5V,  GND -> GND
//   LED + 220 Ohm op D12
// HC-06: 9600 baud (bevestigd). USB: 115200 baud.
//
// Werking:
//   * alles wat via Bluetooth binnenkomt wordt naar USB doorgestuurd en omgekeerd
//   * commando's via Bluetooth (afsluiten met Enter):
//       ping      -> antwoordt "pong <millis>"
//       led on    -> LED aan      led off -> LED uit
//   * elke 5 s stuurt de Arduino zelf "alive <n>" naar Bluetooth (toont zenden
//     zonder dat er eerst iets ontvangen is)
// =============================================================================
#include <SoftwareSerial.h>

SoftwareSerial bt(7, 8);   // RX, TX
const uint8_t LED = 12;

char line[24];
uint8_t len = 0;

void handle(const char *cmd)
{
  if (strcmp(cmd, "ping") == 0)        { bt.print(F("pong ")); bt.println(millis()); }
  else if (strcmp(cmd, "led on") == 0) { digitalWrite(LED, HIGH); bt.println(F("OK led on")); }
  else if (strcmp(cmd, "led off") == 0){ digitalWrite(LED, LOW);  bt.println(F("OK led off")); }
  else if (cmd[0])                     { bt.print(F("ERR onbekend: ")); bt.println(cmd); }
}

void setup()
{
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  bt.begin(9600);
  Serial.println(F("PoC Bluetooth klaar"));
  bt.println(F("PoC Bluetooth klaar - typ ping, led on, led off"));
}

void loop()
{
  while (bt.available())
  {
    char c = bt.read();
    Serial.write(c);                                   // BT -> USB (monitor)
    if (c == '\r' || c == '\n') { line[len] = '\0'; handle(line); len = 0; }
    else if (c == 8 || c == 127) { if (len) len--; }   // backspace uit PuTTY
    else if (len < sizeof(line) - 1 && isprint(c)) line[len++] = tolower(c);
  }
  while (Serial.available()) bt.write(Serial.read());  // USB -> BT

  static uint32_t last = 0;
  static uint16_t n = 0;
  if (millis() - last >= 5000) { last = millis(); bt.print(F("alive ")); bt.println(n++); }
}
