// =============================================================================
// Proof of concept start/stop met een hardware-interrupt
// =============================================================================
// Bedrading (zoals in de robot):
//   drukknop tussen +5 V en D10, 1 kOhm pull-down van D10 naar GND
//   LED + 220 Ohm op D12
//
// D10 is geen INT0/INT1-pin (dat zijn enkel D2 en D3, en die worden door de
// linkermotor gebruikt). D10 = PB2 heeft wel een pin-change interrupt:
// PCINT2, groep PCIE0, vector PCINT0_vect (ATmega328P datasheet, hoofdstuk
// "External Interrupts", registers PCICR en PCMSK0).
// Een pin-change interrupt reageert op ELKE flank; in de ISR wordt gekeken of
// de pin hoog is (= knop ingedrukt) en wordt gedebounced op tijd.
//
// LET OP: deze PoC gebruikt bewust GEEN SoftwareSerial. SoftwareSerial
// definieert zelf alle PCINT-vectoren, waardoor deze ISR samen met de HC-06
// niet kan. Zie code/finaal/readme.md voor de oplossing in de finale code.
//
// Seriele monitor 115200 baud: elke druk toont "start" of "stop".
// =============================================================================

const uint8_t BUTTON = 10;
const uint8_t LED = 12;
const uint16_t DEBOUNCE_MS = 50;

volatile bool running = false;
volatile bool changed = false;
volatile uint32_t lastEdge = 0;

ISR(PCINT0_vect)
{
  uint32_t now = millis();
  if (digitalRead(BUTTON) == HIGH && now - lastEdge > DEBOUNCE_MS)
  {
    running = !running;
    changed = true;
  }
  lastEdge = now;              // elke flank (ook dender) herstart de debouncetijd
}

void setup()
{
  pinMode(BUTTON, INPUT);      // externe pull-down
  pinMode(LED, OUTPUT);
  Serial.begin(115200);

  PCMSK0 |= _BV(PCINT2);       // D10 = PB2 = PCINT2
  PCIFR  |= _BV(PCIF0);        // eventuele oude vlag wissen
  PCICR  |= _BV(PCIE0);        // pin-change interrupts groep 0 (D8..D13) aan

  Serial.println(F("PoC interrupt: druk op de knop op D10"));
}

void loop()
{
  // De hoofdlus doet bewust iets "trages" om aan te tonen dat de knop via de
  // interrupt toch onmiddellijk geregistreerd wordt.
  digitalWrite(LED, running);
  if (changed)
  {
    changed = false;
    Serial.println(running ? F("start") : F("stop"));
  }
  delay(500);
}
