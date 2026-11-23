# N20-BT21 DC-gearmotor 6 V, 1000 rpm

Voor deze webshopmotor is geen officiële datasheet beschikbaar. Hieronder staan de gegevens die bekend zijn, met per gegeven de status en de bron.

| gegeven | waarde | status / bron |
|---|---|---|
| type | geborstelde DC-motor met metalen tandwielkast (N20-formaat, 12 × 10 mm motorhuis) | bevestigd (aankoop) |
| nominale spanning | 6 V | aankoop / productpagina |
| toerental onbelast | 1000 rpm bij 6 V | aankoop / productpagina (gekozen uitvoering) |
| as | 3 mm D-as | bevestigd (D-gat wielen passen) |
| overbrengingsverhouding | **onbekend** | niet op productpagina; niet afleiden uit andere projecten |
| blokkeerstroom (stall) | **onbekend** | te meten (zie hieronder) |
| stroom onbelast | **onbekend** | te meten |

## Waarom de blokkeerstroom belangrijk is
- DRV8833: maximaal 1,5 A RMS en 2 A piek per H-brug ([DRV8833.pdf](DRV8833.pdf), *Recommended Operating Conditions*). Daarboven schakelt de chip af met overstroombeveiliging.
- DSN-MINI-360: maximaal ongeveer 1,8 A continu voor beide motoren samen (zie [DSN-MINI-360.md](DSN-MINI-360.md)).
- Pololu raadt bij geborstelde motoren aan om continu niet meer dan ongeveer 25 % van de blokkeerstroom te gebruiken ([Pololu 20D gearmotors](https://www.pololu.com/category/167/20d-mm-gearmotors)).

## Meetprocedure (nog uit te voeren)
1. Zet een labovoeding op 6,0 V, met de stroombegrenzing op 1 A.
2. Meet de stroom onbelast (wiel vrij).
3. Houd de as kort (< 2 s) vast en lees de stroom af. Dit is de blokkeerstroom; ligt die boven 1 A, dan grijpt de begrenzing in.
4. Tel het aantal omwentelingen van het wiel in 10 s bij 6 V. Zo vind je het werkelijke toerental onbelast.

Noteer de resultaten hier, met datum.
