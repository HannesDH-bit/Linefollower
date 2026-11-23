# Gebruiksaanwijzing

### opladen / vervangen batterijen
- De robot gebruikt **2 × DMEGC INR18650-25P in serie** (2S): 8,4 V volgeladen, 7,2 V nominaal. Laad niet verder af dan 6,6 V voor het pakket (3,3 V per cel).
- De cellen hebben **geen beveiliging** en de robot heeft geen BMS. Controleer daarom vóór elke rit de spanning met de multimeter.
- **Laden:** zet de hoofdschakelaar uit en haal de cellen uit de houder. Laad ze in een Li-ion-lader voor losse cellen (CC/CV, 4,2 V). Laad nooit onbewaakt. Gebruik altijd twee cellen met een gelijke spanning (verschil < 0,05 V).
- **Plaatsen:** let op de polariteit die in de houder aangeduid staat. Meet na het plaatsen over de volledige houder: 7,4–8,4 V. Meet je ongeveer 3,7–4,2 V, dan staan de cellen niet in serie of maakt één cel geen contact.
- Gebruik **nooit** 8,4 V rechtstreeks op de motoren; die zijn voor 6 V. De DSN-MINI-360 moet op 6,0 V ingesteld zijn.

### voeding controleren en foutzoeken
Bij de laatste meting stond er **3,5 V op zowel de ingang als de uitgang** van de DSN-MINI-360. Een buckconverter kan alleen omlaag regelen. Als de ingang 3,5 V is, geeft hij die ongeveer door. **De fout zit dus vóór de converter.** Laat de robot zo niet rijden.

Meet stap voor stap (multimeter op DC V, zwarte pen op batterij −). Raak nooit met één meetpen twee punten tegelijk: een kortsluiting op OUT heeft al eens een converter vernield.

| stap | meetpunt | verwacht | als dat niet zo is |
|---|---|---|---|
| 1 | elke cel apart, buiten de houder | 3,6–4,2 V | cel leeg (laden) of defect (< 2,5 V: niet meer gebruiken) |
| 2 | houder: + van cel 1 tot − van cel 2 | som van beide cellen, 7,2–8,4 V | ongeveer 3,7 V: houder **parallel** bedraad of brug tussen de cellen ontbreekt; 0 V: slecht contact of cel verkeerd om |
| 3 | draden van de houder op de perfboard | zelfde als stap 2 | draad, soldeerpunt of schroefklem |
| 4 | na de hoofdschakelaar (schakelaar aan) | zelfde als stap 2 | middelste en buitenste pin verwisseld, of schakelaar defect |
| 5 | DSN-MINI-360 IN+ | zelfde als stap 2 | bedrading tussen schakelaar en converter |
| 6 | DSN-MINI-360 OUT+, **zonder** DRV8833/motoren | 6,0 V (bijregelen met de trimpotentiometer) | OUT ≈ IN en niet regelbaar: converter defect |
| 7 | OUT+ met motoren draaiend (`test both 200`) | > 5,7 V | zakt sterk: batterij leeg, slecht contact, of de motoren trekken te veel stroom |
| 8 | Nano 5V-pin | 4,8–5,1 V | VIN te laag (< 7 V) |

Noteer de meetresultaten in [technische tekeningen/elektronisch](../technische%20tekeningen/elektronisch/readme.md).

### draadloze communicatie
#### verbinding maken
1. Zet de robot aan. De LED van de HC-06 knippert.
2. Windows: *Instellingen → Bluetooth en apparaten → apparaat toevoegen → Bluetooth → HC-06*, pincode `1234` (of `0000`).
3. *Meer Bluetooth-instellingen → COM-poorten*: gebruik de **uitgaande** poort van HC-06 (op deze laptop: **COM4**).
4. PuTTY:
   - *Session*: Connection type **Serial**, Serial line **COM4**, Speed **9600**
   - *Terminal*: **Local echo: Force on**, **Local line editing: Force on**. Zo zie je wat je typt, en wordt een regel pas bij Enter verstuurd.
   - optioneel: *Terminal → Implicit CR in every LF* aanvinken
5. Klik *Open*. De LED van de HC-06 brandt continu. Typ `help`.

Via USB werkt alles hetzelfde: Arduino seriële monitor op **115200 baud**, regeleinde naar keuze.

> Plak meerdere commando's **één regel per keer**. SoftwareSerial kan niet tegelijk zenden en ontvangen; tijdens een antwoord van de robot kunnen nieuwe tekens verloren gaan.

#### commando's
Alle commando's zijn niet hoofdlettergevoelig. Elk commando krijgt een antwoord: `OK …` of `ERR …`.

| commando | functie |
|---|---|
| `help` | overzicht van de commando's en parameters |
| `start` of `run` | rijden |
| `stop` | stoppen (motoren remmen) |
| `show` (of `debug` zonder argument) | alle actuele instellingen, kalibratie, EEPROM-status, de maximale rekentijd en cyclustijd, het aantal overschrijdingen |
| `debug on` / `debug off` | elke `dbgint` ms een regel: `e` fout [mm], `P` `I` `D` bijdragen, `u` correctie, `L` `R` PWM, toestand, `ct` rekentijd [µs] |
| `raw` | eenmalig: ruwe en genormaliseerde sensorwaarden, positie, aantal zwarte sensoren |
| `set <param> <waarde>` | parameter wijzigen (alleen in RAM) |
| `<param>=<waarde>` of `<param> <waarde>` | idem, korte vorm (bv. `kp=2.5`, `power 120`) |
| `<param>` | alleen de waarde van die parameter tonen |
| `calibrate white` | 32 metingen middelen met alle sensoren boven WIT |
| `calibrate black` | 32 metingen middelen met alle sensoren boven ZWART |
| `calibrate auto [s]` | gedurende s seconden (standaard 5) per sensor min/max meten terwijl je de robot heen en weer over de lijn beweegt |
| `calibrate check` | controleren of elke sensor genoeg bereik heeft (≥ 150 ADC-eenheden) |
| `save` | alle parameters + kalibratie in EEPROM bewaren |
| `load` | opnieuw uit EEPROM laden |
| `eeprom` | de inhoud van de EEPROM tonen |
| `defaults` | veilige standaardwaarden laden (niet bewaard) |
| `test left\|right\|both <pwm> [ms]` | motor(en) laten draaien, na swap/invert (standaard 1000 ms) |
| `test a\|b <pwm> [ms]` | DRV8833-kanaal A (OUT1/OUT2) of B (OUT3/OUT4) rechtstreeks |

| parameter | eenheid / bereik | standaard | betekenis |
|---|---|---|---|
| `cycle` | µs, 500–50000 | 2500 | cyclustijd van de regelkring |
| `power` | 0–255 | 90 | basissnelheid (PWM) |
| `max` | 0–255 | 255 | absolute PWM-limiet per motor |
| `diff` | 0–1 | 0,5 | deel van de correctie dat bij het buitenwiel opgeteld wordt (zoals in de template) |
| `slow` | 0–1 | 0,3 | vertraging in bochten: power × (1 − slow × \|e\|/33,3 mm) |
| `kp` | PWM/mm | 2,5 | proportioneel |
| `ki` | PWM/(mm·s) | 0 | integrerend |
| `kd` | PWM/(mm/s) | 0,05 | differentiërend |
| `dfilter` | 0–0,99 | 0,5 | laagdoorlaatfilter op de D-term (0 = geen filter) |
| `ilimit` | PWM | 60 | maximale bijdrage van de I-term (anti-windup) |
| `balance` | %, −50…50 | 0 | + vertraagt de rechtermotor, − de linkermotor |
| `timeout` | ms | 400 | hoe lang zoeken naar een verloren lijn voor de robot stopt |
| `threshold` | 50–950 | 500 | genormaliseerde waarde vanaf waar een sensor "zwart" is |
| `swap` | 0/1 | 0 | 0: linkermotor op kanaal A (OUT1/OUT2), 1: op kanaal B |
| `invleft` / `invright` | 0/1 | 0 | draairichting van de motor omkeren |
| `reverse` | 0/1 | 1 | binnenwiel mag achteruit in scherpe bochten |
| `fastdecay` | 0/1 | 0 | DRV8833 fast decay waar mogelijk (0 = slow decay) |
| `ambient` | 0/1 | 0 | omgevingslichtcompensatie (meting met de IR-leds uit/aan; experimenteel) |
| `echo` | 0/1 | 1 | ontvangen regel terugsturen als `> …` |
| `dbgint` | ms | 100 | interval van de debugregels |

### kalibratie
Kalibreer telkens op de ondergrond waarop gereden wordt, en opnieuw bij ander licht.

**Methode 1, automatisch (aanbevolen):**
1. Zet de robot met de sensorarray dwars over de lijn.
2. Typ `calibrate auto 5`. De LED knippert.
3. Schuif de robot gedurende 5 s langzaam heen en weer, zodat **elke** sensor minstens één keer boven het midden van de zwarte lijn en boven wit komt.
4. Lees het resultaat. Bij `WARN sensor x` heeft die sensor te weinig bereik: herhaal de kalibratie, of controleer de hoogte van de sensor (doel ongeveer 3 mm).
5. `save`

**Methode 2, wit/zwart apart:** zet alle sensoren boven wit, `calibrate white`, dan alle sensoren boven een zwart vlak, `calibrate black`, en daarna `save`.

**Controle:** `raw` met de lijn onder het midden geeft een positie van ongeveer 0 mm. Schuif de lijn naar **links** (in rijrichting): de positie moet **negatief** worden. Is ze positief, keer dan `SENSOR_PINS` om in `config.h`.

### settings
Afregelprocedure (eerst de voeding in orde brengen, zie hierboven):
1. **Motoren:** `test left 120`: het linkerwiel moet vooruit draaien. Draait het rechterwiel, zet dan `swap 1`. Draait het achteruit, zet `invleft 1`. Doe hetzelfde voor `test right 120`. Dan `save`.
2. **Rechtuit:** `test both 150 3000` op de vloer. Wijkt de robot af, pas dan `balance` aan, in stappen van 2. Bij een eerdere meting was de rechtermotor ongeveer 3 % sneller (30 tegen 29 omwentelingen); meet dat eerst opnieuw.
3. **Teken van de regeling:** `power 0`, `start`, en duw de robot met de hand zijwaarts van de lijn. De wielen moeten de robot terug naar de lijn sturen. Gebeurt het omgekeerde, dan klopt de sensorvolgorde niet (zie Controle hierboven).
4. **Kp:** `power 80`, `kd 0`, `ki 0`. Begin met `kp 1`. Verhoog tot de robot de lijn in lichte bochten volgt en net begint te slingeren. Neem dan ongeveer 70 % van die waarde.
5. **Kd:** verhoog `kd` in stappen van 0,02 tot het slingeren verdwijnt. Wordt de robot schokkerig, verhoog dan `dfilter`.
6. **Snelheid:** verhoog `power` in stappen van 10. Vliegt de robot uit de bocht, verhoog dan `slow` of `kp`.
7. **Ki** is meestal niet nodig. Gebruik het alleen als de robot op lange rechte stukken blijvend naast de lijn rijdt.
8. `save`

De robot rijdt stabiel met volgende parameters (in te vullen na het testen):

| power | kp | ki | kd | diff | slow | cycle | gemeten snelheid |
|---|---|---|---|---|---|---|---|
| | | | | | | | |

### start/stop button
- **Locatie:** de drukknop op de perfboard (D10). Vul de exacte plaats in na controle op de robot.
- Eén keer drukken: rijden. Opnieuw drukken: stoppen. De knop wordt gedebounced, en elke druk wordt ook via Bluetooth gemeld.
- Start/stop kan ook met de commando's `start` en `stop`.

### LED (D12)
| LED | betekenis |
|---|---|
| continu aan | voeding aan, robot klaar, geen zwart onder de sensoren |
| snel knipperen (10 Hz) | stilstaand en minstens één sensor ziet zwart |
| aan tijdens het rijden | lijn wordt gezien |
| uit tijdens het rijden | lijn even kwijt (onderbreking of herstel) |
| traag knipperen (1 Hz) | gestopt omdat de lijn verloren was |
| knipperen (4 Hz) tijdens kalibratie | `calibrate auto` loopt |
