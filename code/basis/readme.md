# Basisprogramma

Map: [basis/](basis/) (`basis.ino` + `SerialCommand.h/.cpp`). Gebaseerd op de HOGENT-basistemplate, aangepast aan de pinnen van deze robot. Gecompileerd zonder warnings (6,4 kB flash).

Volgende zaken werken samen in dit project:

* instelbare parameters met SerialCommand
  * cyclustijd: `set cycle <µs>`
  * debug: `debug on` / `debug off`, en `debug` toont alle parameters + de maximale rekentijd
  * start/stop: `run` (of `start`) / `stop`
* parameters worden in het EEPROM-geheugen opgeslagen met `save`, en bij het opstarten automatisch geladen. Is de EEPROM leeg of ongeldig, dan worden standaardwaarden gebruikt.
* start/stop ook met de drukknop. Op D10 gebeurt dat met polling en debounce, op D2/D3 automatisch met een externe interrupt (zie [finale code](../finaal/readme.md#startstop-en-interrupt)).
* draadloze communicatie: dezelfde commando's via USB (115200 baud) en de HC-06 (9600 baud).

Met de cyclus wordt een knipperende LED (D12) gemaakt. Tijdens `run` wisselt de LED elke `blink` cycli van toestand. Standaard is dat `cycle` 10000 µs × `blink` 25 = 250 ms aan, 250 ms uit.

Verschillen met de originele template:
- De parser reageert op `\r` én `\n`, verwerkt backspace, verwerpt te lange regels en is niet hoofdlettergevoelig.
- `set` schrijft niet meer bij elke wijziging naar EEPROM, alleen bij `save`. Dat spaart de EEPROM (ongeveer 100 000 schrijfcycli).
- De EEPROM-structuur heeft een magic number en een versieveld, zodat een lege EEPROM (alle bytes 0xFF) herkend wordt.
