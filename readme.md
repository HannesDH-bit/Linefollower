# LineFollower

Autonome linefollower voor het syntheseproject Elektromechanica – automatisering (HOGENT). De robot volgt een zwarte lijn (15 mm) op een witte ondergrond met 8 reflectiesensoren en een PID-regeling. Instellingen worden draadloos aangepast via Bluetooth en in EEPROM bewaard.

![aansluitschema](technische%20tekeningen/elektronisch/aansluitschema_planB.png)

## specifications

microcontroller: Arduino Nano V3.0 kloon (ATmega328P, 5 V, 16 MHz, CH340)

motors: 2 × N20-BT21 geborstelde DC-gearmotor, 6 V, 1000 rpm

h-bridge: DRV8833 dual H-brug (2,7–10,8 V, 1,5 A per kanaal)

sensors: Pololu QTR-8A (#960), 8 analoge reflectiesensoren, pitch 9,525 mm

batteries: 2 × DMEGC INR18650-25P in serie (2S, 7,2 V nominaal, 2500 mAh), motoren via DSN-MINI-360 step-down op 6,0 V

wireless communication: HC-06 Bluetooth Classic (SPP), 9600 baud

distance sensor - motors: *te meten*

weight: *te wegen*

speed: 0,152 m/s gemeten bij de evaluatie (3 toeren, 4144 mm, met de vorige code). Nieuwe meting volgt.

## inhoud

| map | inhoud |
|---|---|
| [bill of materials](bill%20of%20materials/readme.md) | onderdelen en kostprijs |
| [code/proof of concepts](code/proof%20of%20concepts/readme.md) | [H-brug](code/proof%20of%20concepts/hbridge/readme.md), [sensoren](code/proof%20of%20concepts/sensor/readme.md), [interrupt](code/proof%20of%20concepts/interrupt/readme.md), [draadloos](code/proof%20of%20concepts/wireless%20communication/readme.md) |
| [code/basis](code/basis/readme.md) | basisprogramma (template: cyclus, debug, start/stop, EEPROM, Bluetooth) |
| [code/finaal](code/finaal/readme.md) | finale firmware |
| [datasheets](datasheets/readme.md) | datasheets en specificaties |
| [gebruiksaanwijzing](gebruiksaanwijzing/readme.md) | batterij, voeding, Bluetooth, commando's, kalibratie, afregelen |
| [instructable](instructable/readme.md) | stappenplan om de robot na te bouwen |
| [parcours](parcours/readme.md) | parcours en meetresultaten |
| [technische tekeningen](technische%20tekeningen/elektronisch/readme.md) | elektronisch schema en [mechanische tekeningen](technische%20tekeningen/mechanisch/readme.md) |

## status

| onderdeel | status |
|---|---|
| prototype (plan B, perfboard) | gebouwd; 3 toeren gereden |
| voeding | **open probleem:** 3,5 V gemeten op IN en OUT van de DSN-MINI-360 (zie [gebruiksaanwijzing](gebruiksaanwijzing/readme.md#voeding-controleren-en-foutzoeken)) |
| finale code | nieuw geschreven en gecompileerd, nog te testen op de robot |
| EasyEDA-schema | onvolledig (symbolen zonder verbindingen); aansluitschema als SVG beschikbaar |
| mechanische tekeningen | CAD-bestanden nog toe te voegen |
| plan A (PCB) | nog niet gestart |
