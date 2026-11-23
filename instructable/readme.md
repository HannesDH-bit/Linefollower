# Instructable

Een instructable is een stappenplan, zonder verdere uitleg, waarmee je de robot kunt nabouwen vanuit de bill of materials en met de technische tekeningen. Ook de stappen om de microcontroller te compileren en te uploaden staan erin.

### stap 1
Bestel alle componenten uit de [bill of materials](../bill%20of%20materials/readme.md).

### stap 2
Print de onderplaat in PLA ([technische tekeningen/mechanisch](../technische%20tekeningen/mechanisch/readme.md), `Onderplaat_Linefollower2.stl`). *Printinstellingen invullen: laaghoogte, infill.*

### stap 3
Monteer de twee N20-motoren met de motorbeugels op de onderplaat en druk de rubberwielen op de D-assen.

### stap 4
Monteer het kogelwiel.

### stap 5
Monteer de QTR-8A vooraan, met de sensoren naar beneden. Controleer met een schuifmaat dat de afstand tot de vloer ongeveer 3 mm is (max. 6 mm). Laat de 3.3V BYPASS-jumper open.

### stap 6
Soldeer de vrouwelijke headers voor de Arduino Nano, de DRV8833 en de HC-06 op de perfboard, volgens het [aansluitschema](../technische%20tekeningen/elektronisch/readme.md).

### stap 7
Soldeer de schroefklemmen voor de batterij, de motoren en de DSN-MINI-360.

### stap 8
Soldeer de spanningsdeler voor de HC-06 RX: D8 → 1 kΩ → knooppunt → 2 kΩ → GND; HC-06 RXD op het knooppunt.

### stap 9
Soldeer de drukknop tussen +5 V en D10, met 1 kΩ van D10 naar GND.

### stap 10
Soldeer de LED met 220 Ω tussen D12 en GND (kathode naar GND).

### stap 11
Leg de bedrading: QTR-8A OUT1…OUT8 naar A0…A7, LEDON naar D13; DRV8833 IN1 naar D6, IN2 naar D5, IN3 naar D3, IN4 naar D2; motoren naar OUT1/OUT2 en OUT3/OUT4.

### stap 12
Bedraad de batterijhouder in **serie**. Verbind de + via de hoofdschakelaar met de Nano VIN en de DSN-MINI-360 IN+, en verbind alle GND's met elkaar.

### stap 13
Laat de DRV8833 VM nog los. Plaats de geladen cellen, zet de schakelaar aan en meet over de houder: 7,4–8,4 V.

### stap 14
Stel met de trimpotentiometer de uitgang van de DSN-MINI-360 in op **6,0 V**. Schakel uit en verbind OUT+ met DRV8833 VM.

### stap 15
Installeer de [Arduino IDE](https://www.arduino.cc/en/software) en, indien nodig, de CH340-driver.

### stap 16
Open `code/finaal/linefollower/linefollower.ino`.

### stap 17
Kies *Tools → Board → Arduino Nano*, *Processor → ATmega328P (Old Bootloader)* en de juiste COM-poort.

### stap 18
Klik *Upload*.

### stap 19
Open de seriële monitor (115200 baud) en typ `help`.

### stap 20
Koppel de HC-06 met Windows en open PuTTY op de Bluetooth-COM-poort, 9600 baud ([gebruiksaanwijzing](../gebruiksaanwijzing/readme.md#verbinding-maken)).

### stap 21
Stel de motorrichting in met `test left 120` / `test right 120` en `swap` / `invleft` / `invright`.

### stap 22
Kalibreer: `calibrate auto 5`, en daarna `save`.

### stap 23
Regel de PID af volgens de [gebruiksaanwijzing](../gebruiksaanwijzing/readme.md#settings) en sla op met `save`.

### stap 24
Zet de robot op de lijn en druk op de start/stop-knop.
