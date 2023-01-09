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

bool connectWifi()
{
    if (!State.Wifi.Connected)
    {
        DebugProgressStart("Wifi Connect " + String(Settings.Wifi.Ssid));
        WiFi.mode(WIFI_STA);
        String ssid = String(Settings.Wifi.Ssid);
        String password = String(Settings.Wifi.Password);

        WiFi.begin(ssid.c_str(), password.c_str());
        int trys = Settings.Wifi.ConnectionTrys;

        for (int a = 0; a < trys; a++)
        {
            DebugProgressUpdate();
            if (WiFi.status() == WL_CONNECTED)
            {
                State.Wifi.Connected = true;
                DebugProgressEnd(WiFi.localIP());
                return true;
            }
            delay(500);
        }
        DebugProgressEnd("Fail");
        return false;
    }
    return true;
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