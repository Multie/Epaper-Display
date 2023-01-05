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

#ifndef Time_h
#define Time_h

#include <Arduino.h>
#include "Pins.h"
#include "Debug.h"
#include "Settings.h"
#include "Transient.h"
#include "State.h"

#include "Wlan.h"
#include <time.h>
#include <UniversalTime.h>

void TimeSyncNow()
{
    if (connectWifi())
    {
        Debug("Time", "Sync");
        const char *ntpServer = "192.168.0.1";
        const long gmtOffset_sec = 3600;
        const int daylightOffset_sec = 3600;
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        time(&Transient.Time.LastSync);
    }
}

void TimeSyncStartup()
{
    TimeSyncNow();
}

void TimeSyncWakeup()
{
}

void TimeSyncUpdate()
{

    time_t now;
    time(&now);
    if ((now - Transient.Time.LastSync) > Settings.Time.Syncinterval)
    {
        TimeSyncNow();
    }
    /*
        struct tm timeinfo = {};
        UniversalTime::Now().toTm(&timeinfo);

        DebugStruct("NowStruct");
        DebugStructValue(timeinfo.tm_year);
        DebugStructValue(timeinfo.tm_mon);
        DebugStructValue(timeinfo.tm_mday);
        DebugStructValue(timeinfo.tm_hour);
        DebugStructValue(timeinfo.tm_min);
        DebugStructValue(timeinfo.tm_sec);
        DebugStructValue(timeinfo.tm_isdst);
        DebugStructValue(timeinfo.tm_wday);
        DebugStructValue(timeinfo.tm_yday);
        DebugStructEnd();
        */
}

void TimeSyncShutdown()
{
}

#endif