# Mechanische tekeningen

## Constructie (bevestigd)
- 3D-geprint PLA-chassis (onderplaat)
- 2× N20-motor in N20-motorbeugels, D-as rubberwielen
- universeel kogelwiel als derde steunpunt
- Pololu QTR-8A vooraan, onderaan het chassis
- elektronica op perfboard (5 × 7 cm), 2S 18650-houder

## Bestanden
De CAD-bestanden staan nog **niet** in de repository. Voeg toe:

| bestand | inhoud |
|---|---|
| `onderplaat final (1).ipt` | Inventor-model van de onderplaat |
| `onderplaat final (1).dxf` | 2D-tekening van de onderplaat |
| `Onderplaat_Linefollower.stl` | printbestand, versie 1 |
| `Onderplaat_Linefollower2.stl` | printbestand, versie 2 (welke is de finale versie?) |

Maak vanuit Inventor ook een **PDF-tekening met maatvoering** (*Drawing > Export > PDF*), zodat de afmetingen zonder CAD-software leesbaar zijn.

## Belangrijke maten (in te vullen uit CAD / te meten)
| maat | waarde | waarom belangrijk |
|---|---|---|
| afstand sensor – ondergrond | doel ongeveer 3 mm (max. ongeveer 6 mm, QTR-8A-datasheet) | signaalsterkte en contrast |
| afstand sensorrij – wielas | *te meten* | grotere afstand: vroeger reageren, maar gevoeliger voor slingeren |
| spoorbreedte (wiel tot wiel) | *te meten* | draaicirkel; min. bochtstraal parcours 10 cm |
| wieldiameter | *te meten* | snelheid = rpm × π × d |
| totale breedte / hoogte | *te meten* | de chronobrug is 12 cm breed en 12 cm hoog (opdracht controleren) |
| massa | *te wegen* | |

Snelheidsschatting: met 1000 rpm onbelast en een wiel van bijvoorbeeld 30 mm is de theoretische topsnelheid 1000/60 × π × 0,030 ≈ 1,57 m/s (onbelast, bij 6 V). Vul de werkelijke wieldiameter in.
