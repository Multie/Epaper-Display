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

#ifndef Power_h
#define Power_h

#include <Arduino.h>
#include "Pins.h"
#include "Debug.h"
#include "Settings.h"
#include "Transient.h"
#include "State.h"

void setPower(bool state)
{
    if (State.Power.Enabled != state)
    {
        State.Power.Enabled = state;
        delay(50);
        if (state)
        {
            digitalWrite(PowerPin, HIGH);
        }
        else
        {
            digitalWrite(PowerPin, LOW);
        }
    }
}

void PowerOn()
{
    setPower(true);
}

void PowerStartup()
{
    pinMode(PowerPin, OUTPUT);
    digitalWrite(PowerPin, HIGH);
}

void PowerWakeup()
{
    pinMode(PowerPin, OUTPUT);
    digitalWrite(PowerPin, HIGH);
}

void PowerUpdate()
{
}

void PowerShutdown()
{
    digitalWrite(PowerPin, LOW);
}

#endif