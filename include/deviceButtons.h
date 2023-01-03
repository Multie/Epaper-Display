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

#ifndef deviceButtons_h
#define deviceButtons_h

#include <Arduino.h>
#include "Pins.h"
#include "Debug.h"
#include "Settings.h"
#include "Transient.h"
#include "State.h"

volatile StateButtons ButtonsInteruptState;

void IRAM_ATTR DeviceButtonsButton1Interrupt()
{
    ButtonsInteruptState.Button1 = true;
}
void IRAM_ATTR DeviceButtonsButton2Interrupt()
{
    ButtonsInteruptState.Button2 = true;
}
void IRAM_ATTR DeviceButtonsButton3Interrupt()
{
    ButtonsInteruptState.Button3 = true;
}
void IRAM_ATTR DeviceButtonsButton4Interrupt()
{
    ButtonsInteruptState.Button4 = true;
}
void IRAM_ATTR DeviceButtonsButton5Interrupt()
{
    ButtonsInteruptState.Button5 = true;
}
void IRAM_ATTR DeviceButtonsButton6Interrupt()
{
    ButtonsInteruptState.Button6 = true;
}
void IRAM_ATTR DeviceButtonsButton7Interrupt()
{
    ButtonsInteruptState.Button7 = true;
}

void DeviceButtonsStartup()
{
    attachInterrupt(Button1Pin, DeviceButtonsButton1Interrupt, RISING);
    attachInterrupt(Button2Pin, DeviceButtonsButton2Interrupt, RISING);
    attachInterrupt(Button3Pin, DeviceButtonsButton3Interrupt, RISING);
    attachInterrupt(Button4Pin, DeviceButtonsButton4Interrupt, RISING);
    attachInterrupt(Button5Pin, DeviceButtonsButton5Interrupt, RISING);
    attachInterrupt(Button6Pin, DeviceButtonsButton6Interrupt, RISING);
    attachInterrupt(Button7Pin, DeviceButtonsButton7Interrupt, RISING);
}

void DeviceButtonsWakeup()
{
    uint64_t GPIO_reason = esp_sleep_get_ext1_wakeup_status();
    int pin = (log(GPIO_reason)) / log(2);
    switch (pin)
    {
    case Button1Pin:
        State.Buttons.Button1 = true;
        break;
    case Button2Pin:
        State.Buttons.Button2 = true;
        break;
    case Button3Pin:
        State.Buttons.Button3 = true;
        break;
    case Button4Pin:
        State.Buttons.Button4 = true;
        break;
    case Button5Pin:
        State.Buttons.Button5 = true;
        break;
    case Button6Pin:
        State.Buttons.Button6 = true;
        break;
    case Button7Pin:
        State.Buttons.Button7 = true;
        break;
    default:

        break;
    }

    attachInterrupt(Button1Pin, DeviceButtonsButton1Interrupt, RISING);
    attachInterrupt(Button2Pin, DeviceButtonsButton2Interrupt, RISING);
    attachInterrupt(Button3Pin, DeviceButtonsButton3Interrupt, RISING);
    attachInterrupt(Button4Pin, DeviceButtonsButton4Interrupt, RISING);
    attachInterrupt(Button5Pin, DeviceButtonsButton5Interrupt, RISING);
    attachInterrupt(Button6Pin, DeviceButtonsButton6Interrupt, RISING);
    attachInterrupt(Button7Pin, DeviceButtonsButton7Interrupt, RISING);
}

void DeviceButtonsUpdate()
{
    State.Buttons.Button1 = false;
    if (ButtonsInteruptState.Button1)
    {
        State.Buttons.Button1 = true;
        ButtonsInteruptState.Button1 = false;
    }
    State.Buttons.Button2 = false;
    if (ButtonsInteruptState.Button2)
    {
        State.Buttons.Button2 = true;
        ButtonsInteruptState.Button2 = false;
    }
    State.Buttons.Button3 = false;
    if (ButtonsInteruptState.Button3)
    {
        State.Buttons.Button3 = true;
        ButtonsInteruptState.Button3 = false;
    }
    State.Buttons.Button4 = false;
    if (ButtonsInteruptState.Button4)
    {
        State.Buttons.Button4 = true;
        ButtonsInteruptState.Button4 = false;
    }
    State.Buttons.Button5 = false;
    if (ButtonsInteruptState.Button5)
    {
        State.Buttons.Button5 = true;
        ButtonsInteruptState.Button5 = false;
    }
    State.Buttons.Button6 = false;
    if (ButtonsInteruptState.Button6)
    {
        State.Buttons.Button6 = true;
        ButtonsInteruptState.Button6 = false;
    }
    State.Buttons.Button7 = false;
    if (ButtonsInteruptState.Button7)
    {
        State.Buttons.Button7 = true;
        ButtonsInteruptState.Button7 = false;
    }
}

void DeviceButtonsShutdown()
{
    detachInterrupt(Button1Pin);
    detachInterrupt(Button2Pin);
    detachInterrupt(Button3Pin);
    detachInterrupt(Button4Pin);
    detachInterrupt(Button5Pin);
    detachInterrupt(Button6Pin);
    detachInterrupt(Button7Pin);
}

#endif