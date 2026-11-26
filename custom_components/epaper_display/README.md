# Epaper Display (Custom Integration)

Verbindet das E-Paper-Dashboard (ESP32 Bat Pro, dieses Repo) mit Home
Assistant. Keine YAML-Konfiguration - Setup läuft über die UI.

## Installation via HACS

Liegt im selben (öffentlichen) Repo wie die Firmware, Pfad `custom_components/
epaper_display/` + `hacs.json` am Repo-Root - HACS kann das direkt installieren:

1. HACS/Marketplace -> "Über einen GitHub-Link hinzufügen"
2. URL `https://github.com/Multie/Epaper-Display` eintragen
3. Epaper Display installieren -> Home Assistant neu starten

Alternative ohne HACS: den Ordner `custom_components/epaper_display`
manuell nach `/mnt/data/home-assistant/config/custom_components/` kopieren
und HA neu starten.

Jedes Firmware-Release (siehe `release.sh` im Repo-Root) erhöht auch die
Integrationsversion - ein Firmware-Update für das ESP32 kommt also als
normales HACS-Integrations-Update an.

## Einrichtung

1. Einstellungen -> Geräte & Dienste -> Integration hinzufügen -> "Epaper Display"
2. Wetter-Entity, Kalender(e) und Einkaufslisten-Todo-Entity auswählen
3. Nach dem Anlegen: Gerät öffnen -> die Webhook-URL steht nicht direkt in der
   UI, sondern ergibt sich aus `<HA-Basis-URL>/api/webhook/<webhook_id>` - die
   `webhook_id` einmalig aus den Logs beim Setup holen (`grep webhook_id` im
   HA-Log direkt nach dem Anlegen) oder per Entwicklerwerkzeuge -> Zustände
   die Config-Entry-Daten einsehen.
4. Diese vollständige URL **nicht** in eine Datei im Repo eintragen - sie wird
   per USB über die serielle Setup-Shell der Firmware ins NVS geschrieben
   (siehe Repo-Root, Abschnitt "Setup-Shell"). Es gibt keine Secrets-Datei
   mehr im Projekt.

## Was die Integration macht

- **POST** `/api/webhook/<id>`: ESP32 meldet `{batteryPercent, batteryVoltage,
  firmwareVersion}` - landet als Sensoren (`sensor.epaper_display_akku`, ...)
- **GET** `/api/webhook/<id>`: liefert die aktuelle Display-Zeichenbeschreibung
  (Wetter jetzt/+1h/+2h/heute, Termine heute aus den gewählten Kalendern,
  Anzahl offener Einkaufslisten-Items, Akkustand, Zeitpunkt des letzten
  Refreshs) als Text im Protokoll, das `DisplayParseText` in der Firmware
  versteht (siehe `include/Display/DisplayDrawModules.h`)
- **GET** `/api/webhook/<id>?firmware=version`: liefert `version.json`
  (`versionCode`, `md5`) aus `firmware/` plus die passende `binUrl`
- **GET** `/api/webhook/<id>?firmware=bin`: liefert `firmware/firmware.bin`

Kein Access-Token nötig (Webhooks sind in HA bewusst offen) - die
`webhook_id` selbst (lang, zufällig) ist der Zugriffsschutz.

## Bekannte Einschränkung

Die Bitmap-Fonts der Firmware haben keine Umlaut-Glyphen - `render.py`
transliteriert ä/ö/ü/ß automatisch, alles andere Nicht-ASCII wird zu `?`.
