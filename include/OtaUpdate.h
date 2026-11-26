/*
 *   Copyright (c) 2026 Malte Hering
 *   All rights reserved.

 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at

 *   http://www.apache.org/licenses/LICENSE-2.0

 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#ifndef OtaUpdate_h
#define OtaUpdate_h

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ArduinoJson.h>
#include "Debug.h"
#include "Settings.h"

// Bei jedem Firmware-Release vom release.sh-Skript automatisch erhoeht
#define FirmwareVersionCode 3

// Manifest wird vom selben HA-Webhook geliefert wie der Display-Inhalt, nur
// mit Query-Param - die URL kommt aus dem NVS (Settings.Url, per serieller
// Shell gesetzt), nicht mehr aus einem Compile-Time-Secret.
// Manifest-Schema: {"versionCode": N, "binUrl": "...", "md5": "<32 hex>"}.
// "md5" ist Pflicht zur Integritaetspruefung der .bin VOR dem Flashen - ohne
// das wuerde jeder, der den Netzwerkpfad kontrolliert, beliebigen Code aufs
// Geraet bringen koennen.

// Unter dieser Akku-Prozentzahl wird kein OTA-Flash versucht - ein Brownout
// waehrend des Schreibens auf dieses batteriebetriebene Geraet kann es sonst
// ohne USB-Reflash-Moeglichkeit bricken.
#define OtaMinBatteryPercent 30.0

// Letztes Ergebnis des Update-Checks, ueberlebt Deep Sleep, wird an HA gemeldet
RTC_DATA_ATTR char OtaState[24] = "unknown";

// Definiert in EpaperContent.h
void EpaperReportStatus(float batteryPercent, float batteryVoltage);

void OtaSetState(const char *state)
{
    strlcpy(OtaState, state, sizeof(OtaState));
    Debug("Ota", state);
}

bool OtaIsValidMd5(const String &value)
{
    if (value.length() != 32)
    {
        return false;
    }
    for (size_t i = 0; i < value.length(); i++)
    {
        if (!isxdigit((unsigned char)value[i]))
        {
            return false;
        }
    }
    return true;
}

void OtaCheckAndUpdate(float batteryPercent, float batteryVoltage)
{
    if (WiFi.status() != WL_CONNECTED || strlen(Settings.Url) == 0)
    {
        return;
    }
    if (batteryPercent < OtaMinBatteryPercent)
    {
        OtaSetState("skipped_low_battery");
        return;
    }

    String manifestUrl = String(Settings.Url) + "?firmware=version";

    HTTPClient manifestHttp;
    manifestHttp.begin(manifestUrl);
    int manifestCode = manifestHttp.GET();
    if (manifestCode != HTTP_CODE_OK)
    {
        OtaSetState("check_failed");
        manifestHttp.end();
        return;
    }

    JsonDocument manifest;
    DeserializationError parseError = deserializeJson(manifest, manifestHttp.getStream());
    manifestHttp.end();
    if (parseError)
    {
        OtaSetState("check_failed");
        return;
    }

    int remoteVersionCode = manifest["versionCode"] | 0;
    String binUrl = manifest["binUrl"].as<String>();
    String expectedMd5 = manifest["md5"].as<String>();
    expectedMd5.toLowerCase();

    if (remoteVersionCode <= FirmwareVersionCode || binUrl.length() == 0)
    {
        OtaSetState("up_to_date");
        return;
    }
    if (!OtaIsValidMd5(expectedMd5))
    {
        // Kein/ungueltiger Hash im Manifest -> lieber kein Flash als ein unverifiziertes Image
        OtaSetState("rejected_md5");
        return;
    }

    OtaSetState("updating");
    EpaperReportStatus(batteryPercent, batteryVoltage);

    HTTPClient binHttp;
    binHttp.begin(binUrl);
    int binCode = binHttp.GET();
    int contentLength = binHttp.getSize();
    if (binCode != HTTP_CODE_OK || contentLength <= 0)
    {
        OtaSetState("download_failed");
        binHttp.end();
        return;
    }

    if (!Update.begin(contentLength))
    {
        OtaSetState("install_failed");
        binHttp.end();
        return;
    }
    Update.setMD5(expectedMd5.c_str());

    WiFiClient *stream = binHttp.getStreamPtr();
    size_t written = Update.writeStream(*stream);
    binHttp.end();

    if (written != (size_t)contentLength)
    {
        OtaSetState("download_failed");
        Update.abort();
        return;
    }

    // end(true) prueft u.a. die zuvor gesetzte MD5 gegen das geschriebene Image
    if (!Update.end(true))
    {
        Debug("Ota", "VerifyError:" + String(Update.getError()));
        OtaSetState("verify_failed");
        return;
    }

    OtaSetState("installed_rebooting");
    EpaperReportStatus(batteryPercent, batteryVoltage);
    ESP.restart();
}

#endif
