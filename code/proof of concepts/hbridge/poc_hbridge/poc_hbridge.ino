// =============================================================================
// Proof of concept H-brug (DRV8833) - 2 motoren onafhankelijk, traploos in
// snelheid en draairichting.
// =============================================================================
// Bedrading (zoals in de robot):
//   kanaal A: IN1 = D6 (PWM), IN2 = D5 (PWM)  -> OUT1/OUT2
//   kanaal B: IN3 = D3 (PWM), IN4 = D2 (GEEN PWM!) -> OUT3/OUT4
// Seriele monitor: 115200 baud, regeleinde naar keuze.
//   a <-255..255>   kanaal A instellen   (bv. "a 150", "a -80")
//   b <-255..255>   kanaal B instellen
//   s               beide stoppen (rem)
//   demo            automatische ramp: A vooruit/achteruit, daarna B
// Positief = "IN1/IN3-richting". Noteer bij elke test welke kant het wiel
// draait; daarmee stel je in de finale code swap/invleft/invright in.
// =============================================================================

const uint8_t A_IN1 = 6, A_IN2 = 5;   // beide PWM
const uint8_t B_IN3 = 3, B_IN4 = 2;   // D2 kan geen PWM

// Kanaal A: beide pinnen PWM -> slow decay in beide richtingen
void setA(int s)
{
  s = constrain(s, -255, 255);
  if (s >= 0) { analogWrite(A_IN1, 255); analogWrite(A_IN2, 255 - s); }   // vooruit (s=0 -> rem)
  else        { analogWrite(A_IN1, 255 + s); analogWrite(A_IN2, 255); }   // achteruit
}

// Kanaal B: enkel IN3 kan PWM.
//  s > 0: IN4 = 0, IN3 = PWM           (fast decay)
//  s < 0: IN4 = 1, IN3 = PWM inverted  (slow decay)
void setB(int s)
{
  s = constrain(s, -255, 255);
  if (s > 0)       { digitalWrite(B_IN4, LOW);  analogWrite(B_IN3, s); }
  else if (s < 0)  { digitalWrite(B_IN4, HIGH); analogWrite(B_IN3, 255 + s); }
  else             { digitalWrite(B_IN4, HIGH); analogWrite(B_IN3, 255); }  // rem
}

void ramp(void (*set)(int), const char *name)
{
  Serial.print(name); Serial.println(F(": 0 -> 255 -> 0 -> -255 -> 0"));
  for (int s = 0; s <= 255; s += 5)   { set(s); delay(20); }
  for (int s = 255; s >= -255; s -= 5) { set(s); delay(20); }
  for (int s = -255; s <= 0; s += 5)  { set(s); delay(20); }
  set(0);
}

void setup()
{
  pinMode(A_IN1, OUTPUT); pinMode(A_IN2, OUTPUT);
  pinMode(B_IN3, OUTPUT); pinMode(B_IN4, OUTPUT);
  TCCR2A |= _BV(WGM21);        // Timer2 (D3) naar fast PWM = zelfde 976 Hz als D5/D6
  setA(0); setB(0);
  Serial.begin(115200);
  Serial.println(F("PoC H-brug klaar. Commando's: a <pwm>, b <pwm>, s, demo"));
}

void loop()
{
  static char buf[20];
  static uint8_t n = 0;
  while (Serial.available())
  {
    char c = Serial.read();
    if (c == '\r' || c == '\n')
    {
      buf[n] = '\0';
      if (n == 0) continue;
      n = 0;
      int v = atoi(buf + 1);
      if (buf[0] == 'a')      { setA(v); Serial.print(F("A = ")); Serial.println(v); }
      else if (buf[0] == 'b') { setB(v); Serial.print(F("B = ")); Serial.println(v); }
      else if (buf[0] == 's') { setA(0); setB(0); Serial.println(F("stop")); }
      else if (strcmp(buf, "demo") == 0) { ramp(setA, "A"); ramp(setB, "B"); Serial.println(F("demo klaar")); }
      else Serial.println(F("onbekend"));
    }
    else if (c == 8 || c == 127) { if (n) n--; }
    else if (n < sizeof(buf) - 1) buf[n++] = tolower(c);
  }
}
