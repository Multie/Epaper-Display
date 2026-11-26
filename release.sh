#!/usr/bin/env bash
# Erhoeht Firmware- + Integrations-Version, baut die Firmware, prueft auf
# versehentlich eingecheckte Secrets und legt firmware.bin + version.json in
# den HACS-Integrationsordner, damit ein HA-Nutzer das naechste Integration-
# Update auch das neue firmware.bin bekommt. Committet/pusht NICHS - das
# macht der Nutzer nach manueller Pruefung.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")"

OTA_HEADER="include/OtaUpdate.h"
MANIFEST="custom_components/epaper_display/manifest.json"
FIRMWARE_DIR="custom_components/epaper_display/firmware"
ENV_NAME="esp-bat-pro"

echo "==> Secret-Scan"
# Schutznetz: diese Werte waren mal geleakt, duerfen nie wieder auftauchen.
# Zusaetzlich generische Pruefung auf hartcodierte Wifi-Secret-Defines, die
# es nach der NVS-Umstellung nicht mehr geben sollte.
LEAK_PATTERNS=('v1114' '191371535219' 'WifiSecretSsid' 'WifiSecretPassword')
FOUND_LEAK=0
for pattern in "${LEAK_PATTERNS[@]}"; do
    if git ls-files -z | xargs -0 grep -l -- "$pattern" 2>/dev/null; then
        echo "FEHLER: Muster '$pattern' in getrackten Dateien gefunden - Abbruch." >&2
        FOUND_LEAK=1
    fi
done
if [ "$FOUND_LEAK" -ne 0 ]; then
    exit 1
fi

echo "==> Firmware-Version erhoehen"
CURRENT_FW_VERSION=$(grep -oP '(?<=#define FirmwareVersionCode )\d+' "$OTA_HEADER")
NEW_FW_VERSION=$((CURRENT_FW_VERSION + 1))
sed -i "s/#define FirmwareVersionCode ${CURRENT_FW_VERSION}/#define FirmwareVersionCode ${NEW_FW_VERSION}/" "$OTA_HEADER"
echo "    FirmwareVersionCode: ${CURRENT_FW_VERSION} -> ${NEW_FW_VERSION}"

echo "==> Integrations-Version erhoehen"
python3 - "$MANIFEST" <<'PYEOF'
import json
import sys

path = sys.argv[1]
with open(path) as f:
    manifest = json.load(f)

major, minor, patch = (int(part) for part in manifest["version"].split("."))
manifest["version"] = f"{major}.{minor}.{patch + 1}"

with open(path, "w") as f:
    json.dump(manifest, f, indent=2)
    f.write("\n")

print(f"    manifest.json version: {major}.{minor}.{patch} -> {manifest['version']}")
PYEOF

echo "==> Firmware bauen"
if ! command -v platformio >/dev/null 2>&1; then
    echo "FEHLER: 'platformio' nicht im PATH gefunden." >&2
    exit 1
fi
platformio run -e "$ENV_NAME"

BUILT_BIN=".pio/build/${ENV_NAME}/firmware.bin"
if [ ! -f "$BUILT_BIN" ]; then
    echo "FEHLER: $BUILT_BIN wurde nicht erzeugt." >&2
    exit 1
fi

echo "==> MD5 berechnen"
MD5=$(md5sum "$BUILT_BIN" | awk '{print $1}')
echo "    MD5: $MD5"

echo "==> In Integrationsordner kopieren"
mkdir -p "$FIRMWARE_DIR"
cp "$BUILT_BIN" "$FIRMWARE_DIR/firmware.bin"
python3 - "$FIRMWARE_DIR/version.json" "$NEW_FW_VERSION" "$MD5" <<'PYEOF'
import json
import sys

path, version_code, md5 = sys.argv[1], int(sys.argv[2]), sys.argv[3]
with open(path, "w") as f:
    json.dump({"versionCode": version_code, "md5": md5}, f, indent=2)
    f.write("\n")
PYEOF

echo "==> Fertig."
echo "    FirmwareVersionCode: $NEW_FW_VERSION"
echo "    MD5: $MD5"
echo "    Nichts committet/gepusht - git status/diff pruefen und selbst freigeben."
