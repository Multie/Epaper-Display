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

#ifndef sleep_h
#define sleep_h
#include <Arduino.h>
#include "Pins.h"
#include <UniversalTime.h>
#include "Transient.h"




/*
 * Active: ESP32Core, ULP Coprocessor, RTC
 * Paused:
 * Inactive: Wifi, Bluetooth, Radio, Periphials
 */
// void StartModemSleep()
//{
// }
/*
 * Active: ULP Coprocessor, RTC
 * Paused: ESP32Core
 * Inactive: Wifi, Bluetooth, Radio, Periphials
 */
void StartLightSleep(uint64_t duration_us)
{
    esp_sleep_enable_timer_wakeup(duration_us);
    esp_light_sleep_start();
}
/*
 * Active: ULP Coprocessor, RTC
 * Paused:
 * Inactive: ESP32Core, Wifi, Bluetooth, Radio, Periphials
 */
void StartDeepSleep(uint64_t duration_us)
{
    esp_sleep_enable_timer_wakeup(duration_us);
    esp_deep_sleep_start();
}
/*
 * Active: RTC
 * Paused:
 * Inactive: ESP32Core,ULP Coprocessor , Wifi, Bluetooth, Radio, Periphials
 */
void StartHibernationSleep(uint64_t duration_us)
{
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);

    esp_sleep_enable_timer_wakeup(duration_us);
    esp_deep_sleep_start();
}

void StartSleep(UniversalTime duration)
{
    uint64_t duration_us = 0;
    duration_us += duration.getYear() * 31536000000000ull;
    duration_us += duration.getMonth() * 2592000000000Ull;
    duration_us += duration.getDay() * 86400000000ull;
    duration_us += duration.getHour() * 3600000000ull;
    duration_us += duration.getMinute() * 60000000ull;
    duration_us += duration.getSecond() * 1000000ull;
    duration_us += duration.getMillisecond() * 1000ull;

    if (
        duration.getYear() >= 0 &&
        duration.getMonth() >= 0 &&
        duration.getDay() >= 0 &&
        duration.getHour() >= 8)
    {
        // Hibernat
        Serial.print("Start HibernationSleep:");
        Serial.print(duration.toString());
        Serial.print(" ");
        Serial.print(duration_us);
        Serial.println();
        StartHibernationSleep(duration_us);
    }
    else
    {
        if (
            duration.getMinute() > 0 ||
            duration.getSecond() > 0)
        {
            // Deep Sleep
            Serial.print("Start DeepSleep:");
            Serial.print(duration.toString());
            Serial.print(" ");
            Serial.print(duration_us);
            Serial.println();
            StartDeepSleep(duration_us);
        }
    }
}
#endif