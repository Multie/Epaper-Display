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
#pragma region Battery
typedef struct StateBattery
{
    bool Alarm;
    float Percent;
    float Usage;
    float Voltage;
} StateBattery;
#pragma endregion

#pragma region Buttons
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
#pragma endregion

#pragma region Gesture
typedef struct StateGesture
{
    bool Up;
    bool Down;
    bool Left;
    bool Right;
} StateGesture;
#pragma endregion

#pragma region Slider
typedef struct StateSlider
{
    double Position;
    double OldPosition;
    int Raw;
} StateSlider;
#pragma endregion

#pragma region Beeper
enum BeeperSongs
{
    None = 0,
    Accept,
    NotAccept,
};
typedef struct StateBeeper {
    bool Play;
    enum BeeperSongs Song;
    
} StateBeeper;
#pragma endregion

#pragma region Wifi
typedef struct StateWifi {
    bool Connected;
} StateWifi;
#pragma endregion

#pragma region Time

typedef struct StateTime {
    
} StateTime;

#pragma endregion

#pragma region Power
typedef struct StatePower {
    bool Enabled;
} StatePower;

#pragma endregion

#pragma region Display
typedef struct StateDisplay {
    bool painted;
    bool Active;
    bool Sleep;
} StateDisplay;

#pragma endregion


typedef struct StateData
{
    StateBattery Battery;
    StateButtons Buttons;
    StateGesture Gesture;
    StateSlider Slider;
    StateBeeper Beeper;
    StatePower Power;
    StateWifi Wifi;
    StateDisplay Display;
} StateData;

StateData State = {};

#endif