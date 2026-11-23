# Hoofdschakelaar en start/stop-drukknop

## Hoofdschakelaar: Goobay tuimelschakelaar SPDT (productcode 1281200)
| gegeven | waarde | bron |
|---|---|---|
| type | SPDT, On-On (2 standen, 3 aansluitingen) | BOM / verpakking |
| max. belasting | 125 V AC / 3 A | BOM / verpakking |
| afmetingen | 8 × 5 × 7 mm | BOM / verpakking |

Gebruik: de gemeenschappelijke middelste pin gaat naar de batterij +. Eén buitenste pin gaat naar het voedingsnet (Nano VIN en DSN-MINI-360 IN+); de andere buitenste pin blijft vrij. Zo werkt de On-On-schakelaar als aan/uit.
Stroom: de motoren en de logica trekken samen ruim minder dan 3 A, binnen de specificatie. Zoek de exacte datasheet bij Goobay (artikelnummer 1281200) en voeg die hier toe.

## Start/stop-drukknop
| gegeven | waarde | bron |
|---|---|---|
| type | monostabiele drukknop (normaal open) | BOM (eigen voorraad) |
| fabrikant / type | **onbekend** (eigen voorraad) | te controleren |

Aansluiting: tussen +5 V en D10, met 1 kΩ pull-down van D10 naar GND. Ingedrukt geeft dat 5 V (HIGH), losgelaten 0 V (LOW). De stroom bij indrukken is 5 V / 1 kΩ = 5 mA.
