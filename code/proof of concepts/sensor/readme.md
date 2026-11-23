# Sensoren proof of concept

Minimale hard- en software die aantoont dat minimaal 6 sensoren onafhankelijk van elkaar kunnen uitgelezen worden (geen calibratie, normalisatie of interpolatie). Hierbij wordt een zo groot mogelijk bereik van de AD-converter benut.

Code: [poc_sensor/poc_sensor.ino](poc_sensor/poc_sensor.ino)

## Schakeling
| QTR-8A (Pololu #960) | Arduino Nano |
|---|---|
| OUT1…OUT8 | A0…A7 |
| LEDON | D13 |
| VCC | 5V |
| GND | GND |
| 3.3V BYPASS | open (werking op 5 V) |

## Werking
- Elke 100 ms worden de 8 ruwe ADC-waarden (0…1023) getoond, eerst met de IR-leds AAN en daarna met de IR-leds UIT (alleen omgevingslicht).
- De QTR-8A geeft een lage spanning bij veel reflectie (wit) en een hoge spanning bij weinig reflectie (zwart) ([datasheet](../../../datasheets/QTR-8A_QTR-8RC.pdf)).
- **AD-bereik:** de ADC gebruikt de 5 V-referentie. Wit geeft ongeveer 50 en zwart ongeveer 700–1000 (historische metingen), dus het grootste deel van het 10-bit bereik wordt gebruikt. Een lagere referentie zou zwart laten verzadigen.
- Met de *Seriële plotter* van de Arduino IDE zie je de 8 waarden als grafiek.

## Stappenplan
1. Monteer de sensor op ongeveer 3 mm boven de ondergrond, zoals in de robot.
2. Upload de code en open de monitor of plotter op 115200 baud.
3. Leg de robot op wit: alle waarden zijn laag.
4. Schuif een zwarte lijn van 15 mm van links naar rechts onder de array. De waarden stijgen sensor na sensor; zo bewijs je dat elke sensor onafhankelijk werkt en in welke volgorde ze aangesloten zijn.
5. Vergelijk de waarden met de leds aan en uit. Het verschil toont hoeveel omgevingslicht (zon, TL) meespeelt.

## Resultaten (in te vullen)
| | s1 | s2 | s3 | s4 | s5 | s6 | s7 | s8 |
|---|---|---|---|---|---|---|---|---|
| wit | | | | | | | | |
| zwart | | | | | | | | |
| wit, leds uit | | | | | | | | |
