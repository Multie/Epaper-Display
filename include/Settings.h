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

// Settings are storred in EEPROM and are read when needed. Settings are set by user.

#ifndef Settings_h
#define Settings_h

#include <Arduino.h>
#include <Preferences.h>
#include "Secrets.h"
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
    uint64_t Checksum;
} SettingsStruct;

SettingsStruct Settings;

void setSettings()
{
    strcpy((Settings.Wifi.Ssid), WifiSecretSsid);
    strcpy((Settings.Wifi.Password), WifiSecretPassword);
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
    preferences.begin("settings", false);
    preferences.putBytes("settings", &Settings, sizeof(Settings));
    preferences.end();
}

#endif