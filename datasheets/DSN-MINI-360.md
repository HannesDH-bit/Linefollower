# DSN-MINI-360 step-down converter

De DSN-MINI-360 is een kleine, instelbare buckconverter van een webshop. Er bestaat geen officiële datasheet van de module. De regelchip op deze modules is volgens de gangbare productbeschrijvingen een **MP2307** van Monolithic Power Systems, of een kloon daarvan. Controleer de opdruk op jouw module.

| gegeven | waarde | bron |
|---|---|---|
| ingangsspanning | 4,75 – 23 V | MP2307-datasheet (*Recommended Operating Conditions*) |
| uitgangsspanning | 1,0 – 17 V, instelbaar met trimpotentiometer | productbeschrijvingen |
| uitgangsstroom | 3 A piek, ongeveer 1,8 A continu (module koelt slecht) | MP2307: 3 A; module: productbeschrijvingen |
| schakelfrequentie | 340 kHz | MP2307-datasheet |
| afmetingen | ongeveer 17 × 11 × 3,8 mm | productbeschrijvingen |
| aansluitingen | IN+, IN−, OUT+, OUT− (IN− en OUT− zijn intern verbonden) | module |

MP2307-datasheet: <https://www.monolithicpower.com/en/mp2307.html>. Een automatische download was niet mogelijk; download de PDF handmatig en zet hem in deze map als `MP2307.pdf`.

## Belangrijk voor deze robot
- **Instelling:** OUT = 6,0 V (voeding van de DRV8833 en de N20-motoren). Stel in **zonder** motoren aangesloten, meet met de multimeter en draai aan de trimpotentiometer.
- **Een buckconverter kan alleen omlaag regelen.** Is de ingangsspanning lager dan de ingestelde uitgang, dan geeft de converter ongeveer de ingangsspanning door. Daarom wijst de meting van 3,5 V op **zowel** de ingang als de uitgang op een probleem **vóór** de converter: de batterij, de batterijhouder, de schakelaar of de bedrading. Een defecte converter is minder waarschijnlijk. Zie [gebruiksaanwijzing](../gebruiksaanwijzing/readme.md#voeding-controleren-en-foutzoeken).
- **Dropout:** de MP2307 heeft een kleine spanningsval nodig. Bij een 2S-pakket onder ongeveer 7 V kan de 6,0 V gaan zakken.
- **Kortsluiting op OUT** (bijvoorbeeld met de meetpen) kan de module beschadigen. Dat is eerder al gebeurd met een module.
