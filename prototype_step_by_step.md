# ESP32-S3 SIP Doorbell Prototype – Step by Step

## 0) Überblick (Zielbild)

- **Hardware:** ESP32-S3 DevKit + 1 Taster + 1–2 Relais + Netzteil
- **Funktion:**
  1. Taster drücken → Gerät baut **SIP-Anruf** zu einer fest hinterlegten Nummer/URI auf (z. B. Handy/Softphone).
  2. Während des Gesprächs sendet der Teilnehmer **DTMF**:
     - `1` → Türöffner-Relais schaltet (kurzzeitig).
     - `2` → Licht-Relais toggelt (an/aus).
- **Firmware-Basis (empfohlen):** ESP-IDF + **ESP-ADF/esp-sip** (liefert SIP+RTP+DTMF).\
  *(Alternativ geht auch eine andere SIP-Lib; die Logik bleibt gleich.)*

---

## 1) Voraussetzungen

### Hardware (Minimal)

- ESP32-S3 DevKit (USB-C bevorzugt)
- Taster (taktile Taste)
- 1–2 Relais (fertige 5 V/3,3 V-Relaismodule mit Optokoppler sind am einfachsten)
- Netzteil 5 V (USB-Netzteil ok)
- Jumperkabel, Breadboard o. ä.

**Optional (für Audio ab Prototyp-2):**

- I²S-Mikro (MEMS) + I²S-Verstärker/kleiner Lautsprecher

### SIP-Infrastruktur

- Ein registrierbarer **SIP-Account** (z. B. Asterisk/FreePBX, Fritz!Box-Nebenstelle, Cloud-PBX)
- **DTMF-Typ:** „RFC2833 / RTP-Events“ aktivieren
- Codecs: G.711 a-law/µ-law (fürs Erste der stabilste Weg)
- Ziel-Rufnummer/URI, die angerufen werden soll (dein Handy/Softphone)

---

## 2) Verdrahtung

> Nutze **beliebige freie GPIOs** (keine Strapping-Pins, kein USB D+/D− auf S3). Die folgenden sind Platzhalter – passe sie an dein DevKit an.

- **Taster**: GPIO `BTN` → Taster → GND
  - internen Pull-up aktivieren, **aktiv LOW**
- **Relais 1 (Tür)**: GPIO `RELAY_DOOR` → Relais-IN
- **Relais 2 (Licht)**: GPIO `RELAY_LIGHT` → Relais-IN
- **Relais-VCC/GND** mit Board-5 V/GND verbinden (bei 5 V-Modul).
- **Türöffner/Licht**: **galvanische Trennung** beachten! Lastspannung niemals direkt am ESP!
  - Türöffner oft 8–12 V AC/DC: über Relais-Kontakt (NO/COM) schalten.

**Empfohlene Defaults (falls passend zum Board):**\
`BTN=GPIO13`, `RELAY_DOOR=GPIO15`, `RELAY_LIGHT=GPIO16`

---

## 3) Firmware-Option wählen

**Empfohlen:** ESP-IDF + **ESP-ADF (esp-sip)**

- Gründe: stabiler SIP-Stack inkl. RTP/DTMF, gute Beispiele, OTA problemlos integrierbar.

> Falls du lieber Arduino nutzt, plane einen Zwischenschritt ein (zuerst nur Call Setup + DTMF, Audio später). Für echte Sprachübertragung ist ESP-IDF/ADF deutlich reibungsloser.

---

## 4) Entwicklungsumgebung vorbereiten (Kurz)

1. **ESP-IDF** installieren (Python-Env, IDF Tools, `idf.py`)
2. **ESP-ADF** holen und als extra-Komponentenpfad einbinden (enthält `esp_sip`)
3. Test-Projekt bauen und flashen (`idf.py set-target esp32s3 && idf.py build flash monitor`)

*(Die genauen Install-Kommandos variieren je OS; nimm deine Standard-IDF-Routine.)*

---

## 5) Projektstruktur (minimal)

```
firmware/esp-idf/
  └─ main/
      ├─ app_main.c
      ├─ sip_logic.c/.h      // Call/DTMF-Logik
      ├─ io_relay.c/.h       // GPIO/Relais
      ├─ io_button.c/.h      // Taster mit Debounce
      └─ config.h            // WLAN + SIP + GPIO Mapping
```

---

## 6) Konfiguration (config.h – Beispiel)

```c
// WLAN
#define WIFI_SSID        "DeinSSID"
#define WIFI_PASS        "DeinPasswort"

// SIP
#define SIP_USER         "tuer1"
#define SIP_PASS         "geheim"
#define SIP_DOMAIN       "pbx.lan"       // oder Fritz.Box / Provider
#define SIP_OUTBOUND     ""              // optional Proxy
#define SIP_CALLEE_URI   "sip:100@pbx.lan"  // Ziel (Handy/Softphone)

// GPIO
#define PIN_BTN          13
#define PIN_RELAY_DOOR   15
#define PIN_RELAY_LIGHT  16

// Verhalten
#define DOOR_PULSE_MS    700   // Öffner nur kurz pulsen
```

---

## 7) I/O implementieren

### Button mit Debounce + Long-Press-Ignore

- interner Pull-up, **fallende Flanke = gedrückt**
- Software-Debounce (\~30–50 ms)
- Bei „pressed“ → **Call starten**, wenn kein Call aktiv

### Relais

- Ausgänge als OUTPUT, initial LOW
- **Türöffnen**: `HIGH` → `delay(DOOR_PULSE_MS)` → `LOW`
- **Licht**: Toggle Zustand

---

## 8) SIP-Logik (Call + DTMF)

### Zustandsautomat

- `IDLE` → (Taste) → `CALLING` (SIP INVITE)
- `CALLING` → (Answer 200 OK) → `IN_CALL`
- `IN_CALL` → (DTMF ‘1’/‘2’) → Relaisaktionen
- (Auflegen remote/Timeout) → `IDLE`

### Ereignisse (typische Callbacks in SIP-Libs)

- **on\_registered / on\_registration\_failed**
- **on\_incoming\_call** (für spätere Varianten)
- **on\_call\_established**
- **on\_call\_terminated**
- **on\_dtmf\_received(char digit)**

> Aktiviere **DTMF via RTP (RFC2833)** in der SIP-Config. In vielen Libs heißt es „out-of-band DTMF“.

---

## 9) Beispiel-Code (vereinfachtes Grundgerüst)

```c
// app_main.c
#include "config.h"
#include "io_button.h"
#include "io_relay.h"
#include "sip_logic.h"

static void on_button_pressed(void) {
    if (!sip_call_active()) {
        sip_call(SIP_CALLEE_URI);  // INVITE absetzen
    } else {
        // optional: zweiter Druck legt auf
        sip_hangup();
    }
}

void app_main(void) {
    io_relays_init(PIN_RELAY_DOOR, PIN_RELAY_LIGHT);
    io_button_init(PIN_BTN, on_button_pressed);

    wifi_start_and_wait_connected(WIFI_SSID, WIFI_PASS);
    sip_init((sip_cfg_t){
        .user=SIP_USER, .pass=SIP_PASS, .domain=SIP_DOMAIN, .outbound=SIP_OUTBOUND,
        .on_registered = NULL,
        .on_call_established = sip_on_call_established,
        .on_call_terminated  = sip_on_call_terminated,
        .on_dtmf             = sip_on_dtmf
    });

    // Event loop läuft, rest passiert in Callbacks
}
```

```c
// sip_logic.c
#include "sip_logic.h"
#include "io_relay.h"
#include "config.h"

static volatile bool in_call = false;

bool sip_call_active(void) { return in_call; }

void sip_on_call_established(void) { in_call = true; }
void sip_on_call_terminated(void)  { in_call = false; }

void sip_on_dtmf(char d) {
    switch (d) {
        case '1':
            relay_pulse_door(DOOR_PULSE_MS);
            break;
        case '2':
            relay_toggle_light();
            break;
        default:
            // ignorieren oder akustisch quittieren
            break;
    }
}
```

---

## 10) Build & Flash

```bash
idf.py set-target esp32s3
idf.py build
idf.py flash monitor
```

Beim Start im **Monitor** prüfen: WLAN-Connect, SIP-Registration OK.\
Taste drücken → im Log INVITE / CALLING → Answer → IN\_CALL.\
Am Handy `1` drücken → Tür-Relais pulst. `2` → Licht toggelt.

---

## 11) Tests (Checkliste)

-

---

## 12) Sicherheit & Betrieb

- **Türöffner nur gepulst** (zeitlich strikt begrenzen).
- **Aktionen protokollieren** (RTC-Zeitstempel → später in HA/MQTT).
- **PIN/2nd-Factor** später ergänzen (DTMF-Sequenz statt Einzeltaste).
- **OTA-Updates** + **Watchdog** aktivieren.
- **PoE / Weitbereichsnetzteil** erst im nächsten Hardware-Spin.

---

## 13) Troubleshooting

| Symptom                      | Ursache                                         | Fix                                                   |
| ---------------------------- | ----------------------------------------------- | ----------------------------------------------------- |
| SIP registriert nicht        | Domain/Benutzer/Passwort falsch, TLS/Zertifikat | Zugangsdaten prüfen, testweise ohne TLS, DNS korrekt? |
| Anruf baut nicht auf         | Ziel-URI/Nebenstelle falsch, PBX-Regel          | PBX-Dialplan prüfen, direkt „sip:100\@domain“ testen  |
| DTMF wird nicht erkannt      | In-Band statt RFC2833, Codec-Mismatches         | PBX/Softphone auf **RFC2833** stellen, G.711 nutzen   |
| Relais schaltet „falsch rum“ | Active-Low-Modul/Inverse Logik                  | Logik invertieren oder anderes Relaismodul            |
| Türöffner hält zu lange      | Pulszeit zu hoch                                | `DOOR_PULSE_MS` reduzieren (z. B. 500–800 ms)         |

---

## 14) Nächste Schritte (Evolution)

- **Audio** sauber integrieren (I²S-Mic + I²S-AMP, AEC/NS per DSP oder ADF-Pipeline).
- **Mehrere Taster** → mehrere Ziele (Appartment A/B), LED-Status.
- **Sicherer Remote-Befehl**: DTMF-PIN (z. B. `*123#` öffnet).
- **MQTT/Home Assistant**: Status, Logs, Remote-Aktoren.
- **PoE/Weitbereich** + **Kamera** im nächsten PCB-Prototyp.

