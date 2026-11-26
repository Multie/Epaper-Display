/*
 *   Copyright (c) 2022 Malte Hering
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

// Settings werden im NVS (Preferences) gespeichert. Es gibt keine Compile-Time-
// Secrets mehr - WLAN und Webhook-URL werden einmalig ueber die serielle Shell
// (siehe SerialShell.h) gesetzt.

#ifndef Settings_h
#define Settings_h

#include <Arduino.h>
#include <Preferences.h>

Preferences preferences;

#pragma region Battery
typedef struct SettingsBattery
{
    float MinVoltage;
    float MaxVoltage;
} SettingsBattery;
#pragma endregion

#pragma region Wifi
typedef struct SettingsWifi
{
    char Ssid[32];      // string
    char Password[32];  // string
    int ConnectionTrys; // count
} SettingsWifi;
#pragma endregion

#pragma region Time
typedef struct SettingsTime
{
    int Syncinterval; // seconds
} SettingsTime;
#pragma endregion

typedef struct __attribute__((packed)) SettingsStruct
{
    SettingsBattery Battery;
    SettingsTime Time;
    SettingsWifi Wifi;
    char Url[160]; // Webhook-URL der HA-Integration, z.B. https://host/api/webhook/<id>
    uint64_t Checksum;
} SettingsStruct;

SettingsStruct Settings;

// FNV-1a 64-bit - reicht als Plausibilitaetspruefung ob im NVS ueberhaupt
// gueltige Settings liegen (nicht als kryptografische Absicherung gedacht).
uint64_t calcSettingsChecksum()
{
    const uint8_t *data = (const uint8_t *)&Settings;
    size_t len = sizeof(Settings) - sizeof(Settings.Checksum);
    uint64_t hash = 0xcbf29ce484222325ULL;
    for (size_t i = 0; i < len; i++)
    {
        hash ^= data[i];
        hash *= 0x100000001b3ULL;
    }
    return hash;
}

void setDefaultSettings()
{
    memset(&Settings, 0, sizeof(Settings));
    Settings.Wifi.ConnectionTrys = 25;
    Settings.Time.Syncinterval = 3600;
}

void loadSettings()
{
    preferences.begin("settings", false);
    preferences.getBytes("settings", &Settings, sizeof(Settings));
    preferences.end();
};

void saveSettings()
{
    Settings.Checksum = calcSettingsChecksum();
    preferences.begin("settings", false);
    preferences.putBytes("settings", &Settings, sizeof(Settings));
    preferences.end();
}

// Laedt Settings aus dem NVS und prueft die Checksum. Liefert false, wenn
// noch nichts (gueltiges) gespeichert ist - z.B. beim allerersten Boot -
// und setzt in diesem Fall Defaults statt Zufallsbytes/leerem NVS zu nutzen.
bool loadSettingsOrDefaults()
{
    loadSettings();
    if (Settings.Checksum != calcSettingsChecksum())
    {
        setDefaultSettings();
        return false;
    }
    return true;
}

#endif
