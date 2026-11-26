# Firmware-Ablage

`firmware.bin` und `version.json` werden ausschliesslich von `release.sh`
(Repo-Root) erzeugt - nicht von Hand bearbeiten.

Der Webhook der Integration liefert diese Dateien aus:
- `GET .../api/webhook/<id>?firmware=version` -> Inhalt von `version.json`
  plus berechneter `binUrl`
- `GET .../api/webhook/<id>?firmware=bin` -> `firmware.bin`

Solange kein Release gelaufen ist, antwortet der Webhook auf beide Anfragen
mit 404 - das ESP32 bleibt dann einfach auf der aktuell geflashten Version.
