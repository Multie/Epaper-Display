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
Preferences preferences;

// Battery
typedef struct SettingsBattery
{
    float MinVoltage;
    float MaxVoltage;
} SettingsBattery;

// Buttons
typedef struct SettingsButtons
{

} SettingsButtons;
// Gesture
typedef struct SettingsGesture
{

} SettingsGesture;
// Slider
typedef struct SettingsSlider
{
    int Min;
    int Max;
} SettingsSlider;

typedef struct __attribute__((packed)) SettingsStruct
{
    SettingsBattery Battery;
    SettingsButtons Buttons;
    SettingsGesture Gesture;
    SettingsSlider Slider;
    uint64_t Checksum;
} SettingsStruct;

SettingsStruct Settings;


void resetSettings() {
    
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