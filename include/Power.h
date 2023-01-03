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



void setPower(bool state)
{
    Debug("PowerSet", state);
    if (state)
    {
        digitalWrite(PowerPin, HIGH);
    }
    else
    {
        digitalWrite(PowerPin, LOW);
    }
}

int activeDevices = 0;

void activateDevice() {
    if (activeDevices == 0) {
       setPower(true); 
       delay(10);
    }
    activeDevices++;
}
void deactivateDevice() {
    if (activeDevices == 1) {
        setPower(false); 
        delay(10);
    }
    if (activateDevice > 0) {
        activeDevices--;
    }
}

void setupPower()
{
    pinMode(PowerPin, OUTPUT);
    digitalWrite(PowerPin, LOW);
    Debug("Power", "Complete");
}



#endif