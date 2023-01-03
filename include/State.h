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

#ifndef State_h
#define State_h

typedef struct StateBattery
{
    bool Alarm;
    float Percent;
    float Usage;
    float Voltage;
} StateBattery;

typedef struct StateButtons
{
    bool Button1;
    bool Button2;
    bool Button3;
    bool Button4;
    bool Button5;
    bool Button6;
    bool Button7;
} StateButtons;

typedef struct StateGesture
{
    bool Up;
    bool Down;
    bool Left;
    bool Right;
} StateGesture;

typedef struct StateSlider
{
    double Position;
    double OldPosition;
    int Raw;
} StateSlider;

typedef struct StateData
{
    StateBattery Battery;
    StateButtons Buttons;
    StateGesture Gesture;
    StateSlider Slider;
} StateData;

StateData State;

#endif