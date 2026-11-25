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
#include <HTTPUpdate.h>
#include <ArduinoJson.h>
#include "Debug.h"

// Bei jedem Firmware-Release von Hand erhoehen
#define FirmwareVersionCode 1

// Gleiches Manifest-Schema wie ColorApp (versionCode/versionName/binUrl/notes),
// gehostet auf der gleichen statischen Ablage wie die APK-OTA-Datei.
#define OtaManifestUrl "https://home.your-tailnet.ts.net/epaper-display/version.json"

void OtaCheckAndUpdate()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    HTTPClient http;
    http.begin(OtaManifestUrl);
    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK)
    {
        Debug("Ota", "ManifestRequestFailed");
        http.end();
        return;
    }

    JsonDocument manifest;
    DeserializationError error = deserializeJson(manifest, http.getStream());
    http.end();
    if (error)
    {
        Debug("Ota", "ManifestParseFailed");
        return;
    }

    int remoteVersionCode = manifest["versionCode"] | 0;
    String binUrl = manifest["binUrl"].as<String>();

    if (remoteVersionCode <= FirmwareVersionCode || binUrl.length() == 0)
    {
        Debug("Ota", "UpToDate");
        return;
    }

    Debug("Ota", "UpdateAvailable");
    WiFiClient client;
    httpUpdate.rebootOnUpdate(true);
    t_httpUpdate_return result = httpUpdate.update(client, binUrl);

    switch (result)
    {
    case HTTP_UPDATE_FAILED:
        Debug("Ota", "UpdateFailed");
        break;
    case HTTP_UPDATE_NO_UPDATES:
        Debug("Ota", "NoUpdates");
        break;
    case HTTP_UPDATE_OK:
        Debug("Ota", "UpdateOk");
        break;
    }
}

#endif
