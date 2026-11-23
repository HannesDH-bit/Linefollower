## Bill of materials

Alleen de onderdelen die effectief in één robot gebruikt worden. Reserveonderdelen (2 extra cellen, 1 extra motor, 1 extra DRV8833, enz.) zijn niet opgenomen. Bij onderdelen die per verpakking gekocht werden, is de prijs per stuk = verpakkingsprijs / aantal per verpakking.

| volgnummer | naam | omschrijving | nieuw/recup | kostprijs/stuk | aantal | subtotaal |
|---:|---|---|---|---:|---:|---:|
| 1 | N20 gearmotor | N20 mini metal gearmotor, 6 V, 1000 rpm ([specificaties](../datasheets/N20-motor.md)) | nieuw | €4,14 | 2 | €8,28 |
| 2 | 18650 batterijhouder | batterijhouder voor 2 × 18650-cellen, in serie bedraad | nieuw | €3,39 | 1 | €3,39 |
| 3 | DMEGC INR18650-25P | Li-ion 18650, 2500 mAh, 20 A, zonder beveiliging (per 4 gekocht) ([specificaties](../datasheets/DMEGC-INR18650-25P.md)) | nieuw | €1,65 | 2 | €3,30 |
| 4 | Pololu QTR-8A | QTR-8A Reflectance Sensor Array, Pololu #960 ([datasheet](../datasheets/QTR-8A_QTR-8RC.pdf)) | nieuw | €15,75 | 1 | €15,75 |
| 5 | Arduino Nano V3.0 (kloon) | ATmega328P, 5 V, 16 MHz, CH340 ([datasheet](../datasheets/Arduino_Nano_datasheet.pdf)) | recup (eigen voorraad) | / | 1 | / |
| 6 | DRV8833 | 2-kanaals DC-motordriver, H-brug, 2,7–10,8 V, 1,5 A (per 2 gekocht) ([datasheet](../datasheets/DRV8833.pdf)) | nieuw | €1,74 | 1 | €1,74 |
| 7 | HC-06 | Bluetooth seriële module (SPP) ([datasheet](../datasheets/HC-06.pdf)) | nieuw | €3,44 | 1 | €3,44 |
| 8 | DSN-MINI-360 | regelbare DC-DC step-downconverter, ingesteld op 6,0 V (per 6 gekocht) ([specificaties](../datasheets/DSN-MINI-360.md)) | nieuw | €1,17 | 1 | €1,17 |
| 9 | rubberwiel | D-gat rubberwiel voor N20 D-as (per 10 gekocht) | nieuw | €0,46 | 2 | €0,92 |
| 10 | N20 motorbeugel | montagebeugel voor N20-motor (per 5 gekocht) | nieuw | €0,33 | 2 | €0,66 |
| 11 | perfboard | dubbelzijdige prototypeprint, 5 × 7 cm (per 5 gekocht) | nieuw | €0,55 | 1 | €0,55 |
| 12 | universeel kogelwiel | robot caster / universeel steunwiel | nieuw | €1,84 | 1 | €1,84 |
| 13 | 2-polige schroefklem | PCB-schroefklem, steek 5,08 mm, 10 A / 300 V (per 20 gekocht) | nieuw | €0,32 ¹ | 3 | €0,97 |
| 14 | start/stop-drukknop | monostabiele drukknop ([info](../datasheets/schakelaar-en-drukknop.md)) | recup (eigen voorraad) | / | 1 | / |
| 15 | witte LED | indicatie-/power-LED | recup (eigen voorraad) | / | 1 | / |
| 16 | weerstand 220 Ω | stroombegrenzing LED | recup (eigen voorraad) | / | 1 | / |
| 17 | weerstand 1 kΩ | pull-down drukknop + HC-06-spanningsdeler ² | recup (eigen voorraad) | / | 4 | / |
| 18 | weerstand 2 kΩ | HC-06-spanningsdeler | recup (eigen voorraad) | / | 1 | / |
| 19 | tuimelschakelaar | Goobay SPDT, On-On, max. 125 V / 3 A, 8 × 5 × 7 mm, productcode 1281200 ([info](../datasheets/schakelaar-en-drukknop.md)) | recup (eigen voorraad) | / | 1 | / |
| 20 | PLA-chassis | zelf ontworpen en 3D-geprint PLA-chassis | recup (school) | / | 1 | / |
| | **TOTAAL** | **materiaalkost van één linefollower, exclusief verzending** | | | **29 onderdelen** | **€42,01** |

¹ De stukprijs is afgerond tot €0,32. Het subtotaal €0,97 komt uit de onafgeronde stukprijs (ongeveer €0,323), vandaar 1 cent verschil met 3 × €0,32. De som van de subtotalen is €42,01.

² **Te controleren:** in het schema zitten 2 weerstanden van 1 kΩ (R1 deler, R3 pull-down). Pas het aantal aan als er niet effectief 4 gebruikt worden.

### Nog aan te vullen
- **Lader** voor de 18650-cellen (feedback van de evaluatie). Vul het type en de prijs in van de lader die je effectief gebruikt, of vermeld dat het een recup/schoollader is.
- Kleine onderdelen die op de robot zitten, zoals headers, draad en schroeven, als ze gebruikt worden.

### Bescherming van de 6 V-motoren
Het 2S-pakket levert 7,2–8,4 V. De motoren krijgen hun voeding **niet** rechtstreeks van de batterij, maar via de DSN-MINI-360, ingesteld op **6,0 V**. PWM 255 komt dus overeen met 6 V op de motor. Daarnaast begrenst de parameter `max` in de firmware de PWM per motor (standaard 255; zet hem lager als de converter ooit overbrugd wordt: 6/8,4 × 255 ≈ 182).
