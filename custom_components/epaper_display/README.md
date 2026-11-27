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
3. Webhook-URL ablesen: Gerät öffnen -> Diagnose -> Sensor "Webhook-URL".
   Sie steht außerdem bei jedem Start der Integration im HA-Log
   (nach `Webhook-URL` suchen). Ist in HA keine URL konfiguriert, erscheint
   nur der Pfad - dann `<HA-Basis-URL>` davorsetzen. Die URL ist wie ein
   Passwort zu behandeln: Sensor und Log sind für HA-Nutzer lesbar.
4. Diese vollständige URL **nicht** in eine Datei im Repo eintragen - sie wird
   per USB über die serielle Setup-Shell der Firmware ins NVS geschrieben
   (siehe Repo-Root, Abschnitt "Setup-Shell"). Es gibt keine Secrets-Datei
   mehr im Projekt.

## Auswahl ändern

Einstellungen -> Geräte & Dienste -> Epaper Display -> Menü (drei Punkte) ->
"Neu konfigurieren": Wetter-Entity, Kalender und Einkaufsliste ändern. Die
Webhook-URL bleibt gleich. Das Display zeigt die Änderung beim nächsten
Inhaltsabruf (bis zu 10 Minuten, oder Reset-Taste).

## Was die Integration macht

- **POST** `/api/webhook/<id>`: ESP32 meldet `{batteryPercent, batteryVoltage,
  firmwareVersion, powerState, otaState,
  displayState}` - landet als Sensoren (`sensor.epaper_display_akku`, ...)
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
