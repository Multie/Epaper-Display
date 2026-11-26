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

#ifndef EpaperContent_h
#define EpaperContent_h

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Debug.h"
#include "Display.h"
#include "Settings.h"
#include "OtaUpdate.h"

// Max. erwartete Antwortgroesse des GET - gegen einen kaputten/boesartigen
// Server, der sonst unbegrenzt Heap fuer die DSL-Antwort belegen wuerde.
#define EpaperContentMaxResponseSize 32768

void EpaperReportStatus(float batteryPercent, float batteryVoltage)
{
    HTTPClient http;
    http.begin(Settings.Url);
    http.addHeader("Content-Type", "application/json");

    JsonDocument status;
    status["batteryPercent"] = batteryPercent;
    status["batteryVoltage"] = batteryVoltage;
    status["firmwareVersion"] = FirmwareVersionCode;

    String body;
    serializeJson(status, body);

    int code = http.POST(body);
    if (code != HTTP_CODE_OK)
    {
        Debug("EpaperContent", "StatusPostFailed:" + String(code));
    }
    http.end();
}

void EpaperFetchAndDrawContent()
{
    HTTPClient http;
    http.begin(Settings.Url);
    int code = http.GET();
    if (code != HTTP_CODE_OK)
    {
        Debug("EpaperContent", "RenderGetFailed:" + String(code));
        http.end();
        return;
    }
    int contentLength = http.getSize();
    if (contentLength <= 0 || contentLength > EpaperContentMaxResponseSize)
    {
        Debug("EpaperContent", "RenderSizeRejected");
        http.end();
        return;
    }

    String dsl = http.getString();
    http.end();

    DisplayPaintBlack.Clear(UNCOLORED);
    DisplayPaintRed.Clear(UNCOLORED);
    // HA liefert die DSL Latin-1-kodiert (ein Byte pro Zeichen, siehe
    // render.py) - dsl.c_str() reicht die Rohbytes 1:1 durch, kein Decoding.
    DisplayParseText((char *)dsl.c_str(), dsl.length());
    DisplayDraw();
}

void EpaperContentUpdate(float batteryPercent, float batteryVoltage)
{
    if (WiFi.status() != WL_CONNECTED || strlen(Settings.Url) == 0)
    {
        return;
    }
    EpaperReportStatus(batteryPercent, batteryVoltage);
    EpaperFetchAndDrawContent();
}

#endif
