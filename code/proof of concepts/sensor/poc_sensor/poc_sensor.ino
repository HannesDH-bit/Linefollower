// =============================================================================
// Proof of concept sensoren - QTR-8A (Pololu #960)
// Toont aan dat de 8 analoge sensoren onafhankelijk uitgelezen worden
// (geen kalibratie, normalisatie of interpolatie).
// =============================================================================
// Bedrading: OUT1..OUT8 -> A0..A7, LEDON -> D13, VCC -> 5V, GND -> GND,
//            3.3V BYPASS open (werking op 5 V).
// Seriele monitor of seriele plotter: 115200 baud.
// Elke 100 ms: 8 ruwe ADC-waarden (0..1023), daarna dezelfde meting met de
// IR-leds UIT (= enkel omgevingslicht).
//
// AD-bereik: de ADC gebruikt de standaard 5 V referentie. De QTR-8A geeft
// ~0 V boven wit en tot bijna 5 V boven zwart, dus het volledige bereik van de
// 10-bit ADC (0..1023) wordt benut. Een kleinere referentie (bv. INTERNAL 1,1 V)
// zou zwart laten verzadigen.
// =============================================================================

const uint8_t SENSORS[8] = { A0, A1, A2, A3, A4, A5, A6, A7 };
const uint8_t LEDON = 13;

void setup()
{
  pinMode(LEDON, OUTPUT);
  Serial.begin(115200);
  Serial.println(F("s1 s2 s3 s4 s5 s6 s7 s8 | leds uit: s1..s8"));
}

void loop()
{
  digitalWrite(LEDON, HIGH);
  delayMicroseconds(200);
  for (uint8_t i = 0; i < 8; i++) { Serial.print(analogRead(SENSORS[i])); Serial.print(' '); }

  digitalWrite(LEDON, LOW);
  delayMicroseconds(200);
  Serial.print(F("| "));
  for (uint8_t i = 0; i < 8; i++) { Serial.print(analogRead(SENSORS[i])); Serial.print(' '); }
  Serial.println();
  digitalWrite(LEDON, HIGH);

  delay(100);
}
