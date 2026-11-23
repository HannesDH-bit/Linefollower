# H-brug proof of concept

Minimale hard- en software die aantoont dat 2 motoren onafhankelijk van elkaar kunnen draaien, en (traploos) regelbaar zijn in snelheid en draairichting.

Code: [poc_hbridge/poc_hbridge.ino](poc_hbridge/poc_hbridge.ino)

## Schakeling
| DRV8833 | Arduino Nano / voeding |
|---|---|
| IN1 | D6 (PWM) |
| IN2 | D5 (PWM) |
| IN3 | D3 (PWM) |
| IN4 | D2 (**geen PWM**) |
| VM (VCC) | VMOT = 6,0 V uit de DSN-MINI-360 |
| GND | gemeenschappelijke GND |
| OUT1/OUT2 | motor kanaal A |
| OUT3/OUT4 | motor kanaal B |

Volledig schema: [technische tekeningen/elektronisch](../../../technische%20tekeningen/elektronisch/readme.md).

## Hoe het werkt
DRV8833 (zie [datasheet](../../../datasheets/DRV8833.pdf), tabellen *H-Bridge Logic* en *PWM Control*):
- **Kanaal A:** beide ingangen zijn PWM. Vooruit: IN1 = 1, IN2 = PWM (255 − s). Achteruit: IN1 = PWM (255 − s), IN2 = 1. Dat is *slow decay*, met een vrij lineair verband tussen PWM en toerental.
- **Kanaal B:** alleen IN3 is PWM (D2 kan geen PWM; `analogWrite()` op D2 geeft enkel aan/uit).
  - s > 0: IN4 = 0, IN3 = PWM s (*fast decay*)
  - s < 0: IN4 = 1, IN3 = PWM (255 + s) (*slow decay*)

  Zo is kanaal B toch traploos regelbaar in **beide** richtingen.
- Timer2 (D3) wordt naar fast PWM gezet, zodat alle motorpinnen op 976 Hz werken.

## Stappenplan
1. **Controleer eerst de voeding:** VMOT moet 6,0 V zijn (zie [gebruiksaanwijzing](../../../gebruiksaanwijzing/readme.md#voeding-controleren-en-foutzoeken)). Zet de robot op een blokje, met de wielen vrij.
2. Upload de code en open de seriële monitor op 115200 baud.
3. `a 150`: motor A draait. Noteer welk wiel (links/rechts) en welke kant (vooruit/achteruit).
4. `a -150`: dezelfde motor draait de andere kant op.
5. `a 60`, `a 255`: de snelheid verandert traploos. Zoek de laagste waarde waarbij de motor nog start (dode zone).
6. Herhaal met `b …`, en stop met `s`.
7. `demo`: automatische ramp van 0 naar 255, naar −255, terug naar 0, voor A en daarna B.

## Resultaten (in te vullen)
| test | resultaat |
|---|---|
| `a 150` draait wiel | links / rechts, vooruit / achteruit |
| `b 150` draait wiel | links / rechts, vooruit / achteruit |
| laagste startwaarde A / B | … / … |
| omwentelingen in 10 s bij 200 (A / B) | … / … (eerder: rechts 30, links 29) |

Deze resultaten bepalen de instellingen `swap`, `invleft`, `invright` en `balance` van de finale code.
