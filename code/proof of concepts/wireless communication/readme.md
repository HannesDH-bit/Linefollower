# Draadloze communicatie proof of concept

Minimale hard- en software waarmee aangetoond wordt dat duplex gecommuniceerd kan worden tussen de microcontroller en een laptop, met **PuTTY** over een Bluetooth-COM-poort (HC-06).

Code: [poc_bluetooth/poc_bluetooth.ino](poc_bluetooth/poc_bluetooth.ino)

### configuratie
| HC-06 | Arduino Nano |
|---|---|
| TXD | D7 (SoftwareSerial RX) |
| RXD | D8 via spanningsdeler: D8 → 1 kΩ → knooppunt → 2 kΩ → GND; RXD op het knooppunt (≈ 3,3 V) |
| VCC | 5V |
| GND | GND |

- HC-06: 9600 baud (bevestigd), Bluetooth Classic SPP. De HC-06 is een slave-only module; de AT-commando's verschillen van de HC-05 ([datasheet](../../../datasheets/HC-06.pdf)).
- USB: 115200 baud, voor de seriële monitor.
- `SoftwareSerial bt(7, 8); // RX, TX`. D0/D1 blijven vrij voor USB.

### opmerkingen
- **SoftwareSerial is half-duplex in de praktijk:** tijdens het zenden staan de interrupts uit (ongeveer 1 ms per teken bij 9600 baud). Tekens die op dat moment binnenkomen, gaan verloren. Plak dus één regel tegelijk, en laat de robot niet tegelijk veel zenden en ontvangen.
- **Zelf typen in PuTTY:** PuTTY stuurt bij Enter alleen `\r`, en toont bij een seriële verbinding standaard niet wat je typt. Backspace stuurt 0x7F. Deze PoC en de finale code verwerken `\r`, `\n` en backspace.
- Beste PuTTY-instelling: *Terminal → Local echo: Force on* en *Local line editing: Force on*. Dan bewerk je de regel lokaal en stuurt PuTTY de volledige regel pas bij Enter.

### gebruiksaanwijzing
1. Voeding aan: de LED van de HC-06 knippert (niet verbonden).
2. Windows: *Instellingen → Bluetooth → apparaat toevoegen → HC-06*, pincode `1234` (of `0000`).
3. Zoek de **uitgaande** COM-poort: *Meer Bluetooth-opties → COM-poorten*. Op deze laptop was dat COM4.
4. PuTTY: *Connection type: Serial*, *Serial line: COM4*, *Speed: 9600*, plus de echo-instellingen hierboven. Klik *Open*: de LED van de HC-06 brandt nu continu.
5. Typ `ping` en Enter: antwoord `pong <ms>`. `led on` / `led off` schakelt de LED op D12.
6. Elke 5 s stuurt de Arduino zelf `alive <n>` (zenden zonder dat er eerst iets ontvangen werd). Alles wat via Bluetooth binnenkomt, verschijnt ook in de seriële monitor (USB), en wat je in de monitor typt, gaat naar PuTTY. Zo is de duplex-werking aangetoond.
