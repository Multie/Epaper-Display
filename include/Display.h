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

#ifndef Display_h
#define Display_h

#include <Arduino.h>
#include "Pins.h"
#include "Debug.h"
#include "Settings.h"
#include "Transient.h"
#include "State.h"

#include "epd4in2bV2.h"
#include "Power.h"

// unsigned int resetPin, unsigned int dcPin, unsigned int csPin, unsigned int busyPin,unsigned int Width,unsigned int Height
Epd epd(DisplayRstPin, DisplayDcPin, DisplayCsPin, DisplayBusyPin, EPD_WIDTH, EPD_HEIGHT);

unsigned char DisplayImageBlack[(EPD_WIDTH * EPD_HEIGHT / 8)] = {};
unsigned char DisplayImageRed[(EPD_WIDTH * EPD_HEIGHT / 8)] = {};

Paint DisplayPaintBlack(DisplayImageBlack, epd.width, epd.height);
Paint DisplayPaintRed(DisplayImageRed, epd.width, epd.height);

// Ausrichtung passend zum Einbau im Gehaeuse
struct DisplayRotationInit
{
    DisplayRotationInit()
    {
        DisplayPaintBlack.SetRotate(ROTATE_0);
        DisplayPaintRed.SetRotate(ROTATE_0);
    }
} DisplayRotationInitInstance;

enum DisplayColor
{
    White,
    Black,
    Red
};

#include "Display/DisplayDrawModules.h"

// Letzter Refresh am Timeout gescheitert? Wird mit dem naechsten Status gemeldet
RTC_DATA_ATTR bool DisplayTimeout = false;

// false = Panel hat nicht (rechtzeitig) geantwortet
bool DisplayDraw()
{
    if (!DisplayPaintBlack.hasChanged() && !DisplayPaintRed.hasChanged())
    {
        return true;
    }

    int initResult;
    if (State.Display.Sleep)
    {
        Debug("Display", "Reset");
        initResult = epd.Wakeup();
    }
    else
    {
        Debug("Display", "Init");
        initResult = epd.Init();
    }
    if (initResult != 0)
    {
        DisplayTimeout = true;
        Debug("Display", "Timeout");
        return false;
    }

    // Uebertraegt beide Puffer und loest genau einen Refresh aus
    epd.DisplayFrame(DisplayPaintBlack.GetImage(), DisplayPaintRed.GetImage());
    DisplayTimeout = epd.TimedOut;
    Debug("Display", DisplayTimeout ? "Timeout" : "Frame");
    DisplayPaintBlack.resetChanged();
    DisplayPaintRed.resetChanged();
    epd.Sleep();
    Debug("Display", "Sleep");
    State.Display.Sleep = true;
    return !DisplayTimeout;
}

void DisplayStartup()
{
    State.Display.Sleep = false;
}

void DisplayWakeup()
{
    State.Display.Sleep = false;
}

void DisplayShutdown()
{
}

#endif
