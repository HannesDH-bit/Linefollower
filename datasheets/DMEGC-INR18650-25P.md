# DMEGC INR18650-25P (Li-ion 18650-cel)

Er is geen officiële DMEGC-datasheet als PDF gevonden. Gegevens volgens de verkoper (NKON): <https://nkon.nl/en/dmegc-inr18650-25p-2500mah-20a.html>

| gegeven | waarde | bron |
|---|---|---|
| chemie | Li-ion (INR, NMC) | NKON |
| capaciteit | 2500 mAh | NKON / aankoop |
| max. continue ontlaadstroom | 20 A | NKON / aankoop |
| nominale spanning | 3,6 V | gangbaar voor INR-cellen; te bevestigen in de officiële datasheet |
| laadspanning | 4,2 V (max.) | gangbaar voor Li-ion; te bevestigen |
| ontlaadeindspanning | ongeveer 2,5 V absoluut minimum; gebruik in de praktijk ≥ 3,0 V | gangbaar; te bevestigen |
| beveiliging | **geen** beveiligingscircuit (*unprotected*, flat top) | NKON |
| afmetingen | Ø 18,25 mm × 64,95 mm, 47 g | NKON |

De distributeur BMZ vermeldt dat deze cel alleen met een beveiligingsprint (BMS) gebruikt mag worden: <https://b2b.bmz-group.com/en/products/18650-2-5ah-618388>.

## Gebruik in de robot
- **2S in serie:** 7,2 V nominaal, 8,4 V volgeladen, minimaal 6,0 V (3,0 V per cel).
- **Geen BMS aanwezig in de robot:** er is geen onderspannings- of overlaadbeveiliging. De firmware kan de batterijspanning niet meten, want A0–A7 zijn allemaal door de sensoren bezet. Meet daarom vóór elke rit met de multimeter en stop onder 3,3 V per cel.
- **Laden:** gebruik altijd een lader voor losse Li-ion-cellen (CC/CV, 4,2 V). Laad de cellen niet in de houder, tenzij met een 2S-balanslader. Een lader staat nog **niet** in de BOM (feedback van de evaluatie): vul aan met de lader die je effectief gebruikt.
