# Start/stop-interrupt proof of concept

Minimale hard- en software die de correcte werking van een start/stop-drukknop aantoont, met een hardware-interrupt.

Code: [poc_interrupt/poc_interrupt.ino](poc_interrupt/poc_interrupt.ino)

## Schakeling
- drukknop tussen +5 V en D10
- 1 kΩ pull-down van D10 naar GND (losgelaten = LOW, ingedrukt = HIGH)
- LED + 220 Ω op D12

## Werking
- D10 is geen INT0/INT1-pin (dat zijn alleen D2 en D3, en die worden door de motor gebruikt). D10 (= PB2) heeft wel een **pin-change interrupt**: PCINT2 in groep 0, vector `PCINT0_vect`, geactiveerd met de registers `PCICR` en `PCMSK0` ([ATmega328P datasheet](../../../datasheets/ATmega328P.pdf), hoofdstuk *External Interrupts*).
- Een pin-change interrupt reageert op elke flank. De ISR kijkt of de pin HIGH is (= ingedrukt) en negeert flanken binnen 50 ms na de vorige flank (debounce).
- De hoofdlus doet bewust een `delay(500)`. Toch wordt elke druk geregistreerd, wat aantoont dat de interrupt werkt, los van de hoofdlus.

## Beperking: samen met Bluetooth
De `SoftwareSerial`-bibliotheek, nodig voor de HC-06, definieert zelf **alle** pin-change vectoren. Deze ISR samen met SoftwareSerial geeft een linkfout (`multiple definition of __vector_3`, getest). Daarom:
- leest de finale code de knop op D10 met polling + debounce, en
- is de voorgestelde oplossing om de knop naar D2 (INT0) te verplaatsen en IN4 naar D10. Zie [finale code](../../finaal/readme.md#startstop-en-interrupt).

## Stappenplan
1. Upload de code en open de seriële monitor op 115200 baud.
2. Druk op de knop: de monitor toont `start` en de LED gaat aan.
3. Druk opnieuw: `stop`, LED uit.
4. Druk 20 keer snel na elkaar en controleer dat elke druk precies één keer meetelt (geen dender).
