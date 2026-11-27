/*
 *   Copyright (c) 2023 Malte Hering
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

#ifndef Wifi_h
#define Wifi_h

#include <Arduino.h>
#include "Pins.h"
#include "Debug.h"
#include "Settings.h"
#include "Transient.h"
#include "State.h"

#include <WiFi.h>

// Zuletzt genutzter Access Point, erspart nach dem Deep Sleep den Kanal-Scan
RTC_DATA_ATTR uint8_t WifiCachedBssid[6] = {};
RTC_DATA_ATTR int32_t WifiCachedChannel = 0;
#define WifiCachedTimeoutMs 3000

bool WifiWaitConnected(uint32_t timeoutMs)
{
    uint32_t start = millis();
    while (millis() - start < timeoutMs)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            return true;
        }
        delay(100);
    }
    return false;
}

bool connectWifi()
{
    if (!State.Wifi.Connected)
    {
        DebugProgressStart("Wifi Connect " + String(Settings.Wifi.Ssid));
        WiFi.persistent(false);
        WiFi.mode(WIFI_STA);
        // Reduce TX power to limit the current spike at radio start (brownout fix)
        WiFi.setTxPower(WIFI_POWER_8_5dBm);

        bool connected = false;
        if (WifiCachedChannel > 0)
        {
            WiFi.begin(Settings.Wifi.Ssid, Settings.Wifi.Password, WifiCachedChannel, WifiCachedBssid);
            connected = WifiWaitConnected(WifiCachedTimeoutMs);
            if (!connected)
            {
                // Access Point gewechselt: normal suchen
                WifiCachedChannel = 0;
                WiFi.disconnect();
                delay(100);
            }
        }
        if (!connected)
        {
            WiFi.begin(Settings.Wifi.Ssid, Settings.Wifi.Password);
            connected = WifiWaitConnected(Settings.Wifi.ConnectionTrys * 500);
        }
        if (!connected)
        {
            DebugProgressEnd("Fail");
            return false;
        }
        memcpy(WifiCachedBssid, WiFi.BSSID(), sizeof(WifiCachedBssid));
        WifiCachedChannel = WiFi.channel();
        State.Wifi.Connected = true;
        DebugProgressEnd(WiFi.localIP());
    }
    return true;
}

void disconnectWifi()
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    State.Wifi.Connected = false;
}

void WifiSetup()
{
}

void WifiWakeup()
{
}

void WifiUpdate()
{
}

void WifiShutdown()
{
}

#endif