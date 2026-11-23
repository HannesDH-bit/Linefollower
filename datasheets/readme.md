# Datasheets

Datasheets en specificaties van alle belangrijke componenten uit de [bill of materials](../bill%20of%20materials/readme.md). Standaardweerstanden en de LED zijn niet opgenomen.

| onderdeel | bestand | wat is belangrijk voor deze robot |
|---|---|---|
| Arduino Nano (ATmega328P) | [ATmega328P.pdf](ATmega328P.pdf) | 16 MHz, 2 KB RAM, 32 KB flash (30 KB vrij na de bootloader), 1 KB EEPROM. Externe interrupts INT0/INT1 enkel op D2/D3; pin-change interrupts op alle pinnen (hfst. *External Interrupts*). PWM: D3/D11 (Timer2), D5/D6 (Timer0), D9/D10 (Timer1). |
| Arduino Nano: bord | [Arduino_Nano_datasheet.pdf](Arduino_Nano_datasheet.pdf), [Arduino_Nano_pinout.pdf](Arduino_Nano_pinout.pdf) | VIN 7–12 V aanbevolen (hier: 2S-pakket 6,0–8,4 V; onder ongeveer 7 V zakt de 5 V-rail). A6/A7 zijn enkel analoge ingangen. Officiële Arduino-documentatie; de kloon heeft dezelfde pinout. |
| Arduino Nano kloon: USB-chip | [CH340.pdf](CH340.pdf) | USB-serieel converter van de kloon; Windows-driver CH340 nodig. Uploaden met *ATmega328P (Old Bootloader)*. |
| DRV8833 dual H-brug | [DRV8833.pdf](DRV8833.pdf) | VM 2,7–10,8 V; 1,5 A RMS / 2 A piek per kanaal. Waarheidstabel xIN1/xIN2 (coast/vooruit/achteruit/rem) en PWM-tabel (fast/slow decay), gebruikt in [motors.h](../code/finaal/linefollower/motors.h). Pinout van de module (IN1–IN4, OUT1–OUT4, VCC/VM, GND, eventueel EEP/nSLEEP en ULT/nFAULT) controleren op de print zelf. |
| Pololu QTR-8A (#960) | [QTR-8A_QTR-8RC.pdf](QTR-8A_QTR-8RC.pdf) | 8 analoge uitgangen, sensorafstand 9,525 mm (0,375"), optimale afstand tot de ondergrond ongeveer 3 mm (max. ongeveer 6 mm). Wit geeft een lage spanning, zwart een hoge. LEDON hoog = IR-leds aan. 3,3 V BYPASS open laten bij 5 V. |
| HC-06 Bluetooth | [HC-06.pdf](HC-06.pdf) | Bluetooth Classic SPP, slave. UART-niveau 3,3 V, dus RX via spanningsdeler. Standaard 9600 baud (bevestigd). AT-commando's verschillen van de HC-05. Datasheet via Olimex; de exacte herkomst van de module is onbekend. |
| N20-BT21 gearmotor 6 V 1000 rpm | [N20-motor.md](N20-motor.md) | Geen officiële datasheet: bekende gegevens + meetprocedure voor de blokkeerstroom en de overbrengingsverhouding. |
| DMEGC INR18650-25P | [DMEGC-INR18650-25P.md](DMEGC-INR18650-25P.md) | 2500 mAh, 20 A, **zonder beveiliging**; 2S: 6,0–8,4 V. |
| DSN-MINI-360 step-down | [DSN-MINI-360.md](DSN-MINI-360.md) | MP2307-buck, ingestelde uitgang 6,0 V voor de motoren; regelt alleen omlaag. |
| Tuimelschakelaar en drukknop | [schakelaar-en-drukknop.md](schakelaar-en-drukknop.md) | Goobay SPDT 3 A (hoofdschakelaar); drukknop met 1 kΩ pull-down op D10. |
