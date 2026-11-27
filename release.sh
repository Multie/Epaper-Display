#!/usr/bin/env bash
# Erhoeht Firmware- + Integrations-Version, baut die Firmware, prueft auf
# Secrets und legt firmware.bin + version.json in den HACS-Integrationsordner,
# damit ein HA-Nutzer das naechste Integration-Update auch das neue firmware.bin
# bekommt. Committet/pusht NICHTS - das macht der Nutzer nach Pruefung.
#
# Wonach gesucht wird, steht NIE in diesem Skript, sondern lokal (nicht im Repo):
#   .release-secrets   ein Wert pro Zeile (WLAN-Passwort, webhook_id, ...),
#                      Zeilen mit # am Anfang werden ignoriert
#   include/Secrets.h  falls noch vorhanden: Passwort und webhook_id daraus
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")"

OTA_HEADER="include/OtaUpdate.h"
MANIFEST="custom_components/epaper_display/manifest.json"
FIRMWARE_DIR="custom_components/epaper_display/firmware"
ENV_NAME="esp-bat-pro"
MIN_SECRET_LENGTH=6

fail() {
    echo "FEHLER: $*" >&2
    exit 1
}

# Unter Windows kann "python3" nur der Store-Platzhalter sein - ausprobieren
PYTHON=""
for candidate in python3 python; do
    if "$candidate" -c "" >/dev/null 2>&1; then
        PYTHON="$candidate"
        break
    fi
done
[ -n "$PYTHON" ] || fail "kein funktionierendes Python gefunden."
command -v platformio >/dev/null 2>&1 || fail "'platformio' nicht im PATH gefunden."

WORK_DIR=$(mktemp -d)
RELEASE_DONE=0
cleanup() {
    # Bei Abbruch die Versionsnummern wiederherstellen
    if [ "$RELEASE_DONE" -ne 1 ] && [ -f "$WORK_DIR/OtaUpdate.h" ]; then
        cp "$WORK_DIR/OtaUpdate.h" "$OTA_HEADER"
        cp "$WORK_DIR/manifest.json" "$MANIFEST"
        echo "Abgebrochen - Versionsnummern zurueckgesetzt." >&2
    fi
    rm -rf "$WORK_DIR"
}
trap cleanup EXIT

echo "==> Zu pruefende Werte sammeln"
PATTERNS="$WORK_DIR/patterns"
{
    if [ -f .release-secrets ]; then
        tr -d '\r' < .release-secrets | grep -v '^#' || true
    fi
    if [ -f include/Secrets.h ]; then
        tr -d '\r' < include/Secrets.h | sed -n 's/^#define[ ]*WifiSecretPassword[ ]*"\(.*\)".*/\1/p'
        tr -d '\r' < include/Secrets.h | sed -n 's|.*/api/webhook/\([^"]*\)".*|\1|p'
    fi
} | awk -v min="$MIN_SECRET_LENGTH" 'length($0) >= min && $0 !~ /^YOUR_/' | sort -u > "$PATTERNS"
PATTERN_COUNT=$(wc -l < "$PATTERNS" | tr -d ' ')
[ "$PATTERN_COUNT" -gt 0 ] || fail "keine Werte zum Pruefen. Lege .release-secrets an (WLAN-Passwort und webhook_id, je eine Zeile, mind. $MIN_SECRET_LENGTH Zeichen)."
echo "    $PATTERN_COUNT Wert(e)"

# Gibt nur Dateinamen aus, nie die gefundenen Werte
scan_files() {
    local found=0 file
    while IFS= read -r -d '' file; do
        [ -f "$file" ] || continue
        if grep -a -q -F -f "$PATTERNS" -- "$file"; then
            echo "    Secret gefunden in: $file" >&2
            found=1
        fi
    done
    return "$found"
}

echo "==> Secret-Scan: Dateien, die im Repo sind oder committet werden koennten"
# Das alte firmware.bin wird ersetzt, das neue wird nach dem Bauen geprueft
git ls-files -z --cached --others --exclude-standard -- . ":(exclude)$FIRMWARE_DIR/firmware.bin" | scan_files || fail "Secrets im Repo - Abbruch."

echo "==> Firmware-Version erhoehen"
CURRENT_FW_VERSION=$(sed -n 's/^#define FirmwareVersionCode \([0-9][0-9]*\).*/\1/p' "$OTA_HEADER")
[ -n "$CURRENT_FW_VERSION" ] || fail "FirmwareVersionCode nicht in $OTA_HEADER gefunden."
# Nie unter die bereits veroeffentlichte Version fallen
PUBLISHED_VERSION=0
if [ -f "$FIRMWARE_DIR/version.json" ]; then
    PUBLISHED_VERSION=$(sed -n 's/.*"versionCode": *\([0-9][0-9]*\).*/\1/p' "$FIRMWARE_DIR/version.json")
fi
NEW_FW_VERSION=$(( (CURRENT_FW_VERSION > PUBLISHED_VERSION ? CURRENT_FW_VERSION : PUBLISHED_VERSION) + 1 ))
cp "$OTA_HEADER" "$WORK_DIR/OtaUpdate.h"
cp "$MANIFEST" "$WORK_DIR/manifest.json"
sed -i "s/^#define FirmwareVersionCode ${CURRENT_FW_VERSION}/#define FirmwareVersionCode ${NEW_FW_VERSION}/" "$OTA_HEADER"
echo "    FirmwareVersionCode: ${CURRENT_FW_VERSION} -> ${NEW_FW_VERSION}"

echo "==> Integrations-Version erhoehen"
"$PYTHON" - "$MANIFEST" <<'PYEOF'
import json
import sys

path = sys.argv[1]
with open(path, encoding="utf-8") as f:
    manifest = json.load(f)

major, minor, patch = (int(part) for part in manifest["version"].split("."))
manifest["version"] = f"{major}.{minor}.{patch + 1}"

with open(path, "w", encoding="utf-8", newline="\n") as f:
    json.dump(manifest, f, indent=2)
    f.write("\n")

print(f"    manifest.json version: {major}.{minor}.{patch} -> {manifest['version']}")
PYEOF

echo "==> Firmware bauen"
platformio run -e "$ENV_NAME"

# PLATFORMIO_WORKSPACE_DIR: noetig, wenn der Projektpfad fuer Windows zu lang ist
BUILT_BIN="${PLATFORMIO_WORKSPACE_DIR:-.pio}/build/${ENV_NAME}/firmware.bin"
[ -f "$BUILT_BIN" ] || fail "$BUILT_BIN wurde nicht erzeugt."

echo "==> Secret-Scan: gebaute Firmware"
printf '%s\0' "$BUILT_BIN" | scan_files || fail "Secrets in der Firmware - nicht veroeffentlicht."

echo "==> MD5 berechnen"
MD5=$(md5sum < "$BUILT_BIN" | awk '{print $1}')

echo "==> In Integrationsordner kopieren"
mkdir -p "$FIRMWARE_DIR"
cp "$BUILT_BIN" "$FIRMWARE_DIR/firmware.bin"
printf '{\n  "versionCode": %d,\n  "md5": "%s"\n}\n' "$NEW_FW_VERSION" "$MD5" > "$FIRMWARE_DIR/version.json"
RELEASE_DONE=1

echo "==> Fertig."
echo "    FirmwareVersionCode: $NEW_FW_VERSION"
echo "    MD5: $MD5"
echo "    Nichts committet/gepusht - git status/diff pruefen und selbst freigeben."
