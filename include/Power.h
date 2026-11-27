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
#include "driver/gpio.h"
#include "Pins.h"
#include "Debug.h"
#include "Settings.h"
#include "Transient.h"
#include "State.h"

// PowerPin versorgt das Display (HIGH = an). E-Paper haelt das Bild auch ohne
// Strom, deshalb ist es nur waehrend des Zeichnens eingeschaltet.
#define PowerOnDelayMs 100

// Leitungen zum Display: ohne Versorgung muessen alle LOW sein, sonst wird es
// ueber die Signale rueckwaerts gespeist.
const gpio_num_t PowerHeldPins[] = {
    (gpio_num_t)DisplayCsPin, (gpio_num_t)DisplayDcPin, (gpio_num_t)DisplayRstPin,
    (gpio_num_t)SpiClkPin, (gpio_num_t)SpiMosiPin, (gpio_num_t)PowerPin};

void PowerDisplayLinesLow()
{
    for (gpio_num_t pin : PowerHeldPins)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }
}

void PowerOn()
{
    // Busy mit Pulldown: ein fehlendes Display gilt als "busy" und laeuft in den Timeout
    pinMode(DisplayBusyPin, INPUT_PULLDOWN);
    digitalWrite(DisplayCsPin, HIGH);
    digitalWrite(PowerPin, HIGH);
    State.Power.Enabled = true;
    delay(PowerOnDelayMs);
}

void PowerOff()
{
    // Erst die Signale LOW, zuletzt die Versorgung (Reihenfolge in PowerHeldPins)
    PowerDisplayLinesLow();
    State.Power.Enabled = false;
}

// Beim Start aufrufen, nachdem die Pins auf LOW gesetzt sind: loest das Halten
void PowerReleaseHold()
{
    gpio_deep_sleep_hold_dis();
    for (gpio_num_t pin : PowerHeldPins)
    {
        gpio_hold_dis(pin);
    }
}

// Direkt vor dem Deep Sleep: sonst schweben die Pins (IO5 zieht dabei nach HIGH)
void PowerHoldForSleep()
{
    PowerOff();
    for (gpio_num_t pin : PowerHeldPins)
    {
        gpio_hold_en(pin);
    }
    gpio_deep_sleep_hold_en();
}

void PowerStartup()
{
    PowerOff();
}

void PowerWakeup()
{
    PowerOff();
}

void PowerUpdate()
{
}

void PowerShutdown()
{
    PowerOff();
}

#endif