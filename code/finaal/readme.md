# Finale code

Map: [linefollower/](linefollower/), te openen met de Arduino IDE via `linefollower.ino`.
Board: **Arduino Nano**, processor **ATmega328P (Old Bootloader)**. Geen externe bibliotheken nodig: `SoftwareSerial` en `EEPROM` zitten in de AVR-core.

Gecompileerd en gecontroleerd met avr-gcc 7.3.0 / AVR-core 1.8.8, zonder warnings:
20,3 kB flash (66 %), 890 byte statisch RAM (43 %). **Nog niet op de robot getest.**

## Opbouw

| bestand | functie |
|---|---|
| [config.h](linefollower/config.h) | hardwareconfiguratie: alle pinnen, de sensorvolgorde, PWM-pinnen en de knop (interrupt of polling) |
| [settings.h/.cpp](linefollower/settings.h) | alle parameters met eenheden, standaardwaarden, bereikcontrole, EEPROM (magic + versie + CRC8, enkel bij `save`) |
| [sensors.h/.cpp](linefollower/sensors.h) | uitlezen, kalibratie (wit/zwart of automatisch min/max), normalisatie 0…1000, lijnpositie in mm, detectie van kruispunten en geen lijn |
| [controller.h/.cpp](linefollower/controller.h) | PID (anti-windup, gefilterde D), vertraging in bochten, toestanden bij lijnverlies |
| [motors.h/.cpp](linefollower/motors.h) | DRV8833: vooruit/achteruit/rem, nooit `analogWrite()` op D2, links/rechts-balans, swap/invert |
| [SerialCommand.h/.cpp](linefollower/SerialCommand.h) | commandoparser uit de HOGENT-template, verbeterd |
| [linefollower.ino](linefollower/linefollower.ino) | hoofdlus met vaste cyclustijd, commando's, drukknop, LED, debug |

## Regelkring

```
elke cycleTime µs (niet-blokkerend, micros()):
  8× analogRead → normaliseren (0 = wit, 1000 = zwart, per sensor gekalibreerd)
  → positie [mm] (parabool door de zwartste sensor + buren; −33,3 … +33,3 mm)
  → toestand: volgen / kruispunt / onderbreking / herstel / verloren
  → PID: u = kp·e + ki·∫e dt + kd·de/dt   (dt = werkelijk gemeten cyclustijd)
  → basis = power · (1 − slow·|e|/33,3)
  → links/rechts volgens 'diff' (zoals de template), verschil behouden bij begrenzing
  → motoren
```

**Eenheden:** e in mm, u in PWM. kp in PWM/mm, ki in PWM/(mm·s), kd in PWM/(mm/s). Omdat de PID met seconden rekent, blijven kp/ki/kd geldig als je de cyclustijd verandert.

### Waarom eerdere Kp-wijzigingen "niets deden" (te controleren in je oude code)
Als de fout in mm uitgedrukt wordt (zoals in de template, max. ±33 mm), dan geeft `KP=0.027` maximaal 0,027 × 33 ≈ **1 PWM** stuurcorrectie. Dat is te weinig om een bocht te nemen, en het verschil tussen 0,02 en 0,05 is niet merkbaar. De nieuwe standaard is `kp = 2.5`, ongeveer 80 PWM bij de maximale fout.
Andere mogelijke oorzaken, die de nieuwe code allemaal afvangt of zichtbaar maakt:
- De parser van de template reageerde alleen op `\r`, verwerkte backspace niet en liet de buffer na 32 tekens overlopen. Een getypt commando werd dan stil genegeerd. Nu volgt altijd `OK …` of `ERR …`.
- `set kd` in de template deelt door de cyclustijd: de getoonde en de gebruikte waarde verschillen.
- De motoren liepen op verschillende PWM-frequenties (D3: 490 Hz, D5/D6: 976 Hz). Nu draaien ze allebei op 976 Hz.
- Verkeerd teken van de correctie of een omgekeerde sensorvolgorde: controleer met `raw` en `debug on` (zie [gebruiksaanwijzing](../../gebruiksaanwijzing/readme.md)).
- Een te lage voedingsspanning (3,5 V gemeten) maakt de motoren traag en zwak, ongeacht de PID-waarden.

## Lijnverlies, onderbrekingen en kruispunten
| situatie | detectie | reactie |
|---|---|---|
| kruispunt | ≥ 6 sensoren zwart | koers aanhouden (laatste fout), rechtdoor |
| korte onderbreking | geen zwart, lijn lag laatst binnen ±16,7 mm | rechtdoor met de laatste correctie, I-term bevroren |
| uit de bocht | geen zwart, lijn lag laatst aan de zijkant | maximaal bijsturen naar die kant op halve snelheid |
| verloren | langer dan `timeout` ms (standaard 400) geen lijn | motoren remmen, LED knippert traag, melding `STOP lijn verloren` |

## Start/stop en interrupt
In de huidige bedrading zit de knop op D10. Dat is geen INT0/INT1-pin, en de pin-change interrupt van D10 kan niet samen met SoftwareSerial (beide claimen `PCINT0_vect`; bevestigd met een linktest). De finale code leest de knop daarom **elke lus met 50 ms debounce**. Omdat de lus in microseconden draait, merk je in de praktijk geen verschil.
Wordt een externe interrupt officieel geëist, wissel dan twee draden (knop naar D2, IN4 naar D10) en pas `config.h` aan. De code schakelt dan automatisch over op `attachInterrupt()` (INT0). Zie [technische tekeningen](../../technische%20tekeningen/elektronisch/readme.md#aandachtspunten-en-openstaande-controles). De [interrupt-PoC](../proof%20of%20concepts/interrupt/readme.md) toont de pin-change interrupt op D10 zonder SoftwareSerial.

## Commando's
Zie de [gebruiksaanwijzing](../../gebruiksaanwijzing/readme.md#commandos).
