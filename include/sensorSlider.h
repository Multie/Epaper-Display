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

#ifndef sensorSlider_h
#define sensorSlider_h

#include <Arduino.h>
#include "Pins.h"
#include "Debug.h"
#include "Settings.h"
#include "Transient.h"
#include "State.h"

#include "Power.h"

void SensorSliderReadState()
{
    State.Slider.Raw = analogRead(SliderPin);
    State.Slider.OldPosition = State.Slider.Position;
    State.Slider.Position = (((float)State.Slider.Raw) - Settings.Slider.Min) / (Settings.Slider.Max - Settings.Slider.Min);
}
// Startup is called when powered up / wakes up from hybernation
void SensorSliderStartup()
{
    //SensorSliderReadState();
}

// Wakeup is called when wakes from deepsleep
void SensorSliderWakeup()
{
    //SensorSliderReadState();
}

// Update is called in a loop
void SensorSliderUpdate()
{
    PowerOn();
    SensorSliderReadState();
}

// Shutdown is called before Sleep
void SensorSliderShutdown()
{
}

#endif