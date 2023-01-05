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

#include "Power.h"
#include "epd4in2bV2.h"

#define COLORED 0
#define UNCOLORED 1

// unsigned int resetPin, unsigned int dcPin, unsigned int csPin, unsigned int busyPin,unsigned int Width,unsigned int Height
Epd epd(DisplayRstPin, DisplayDcPin, DisplayCsPin, DisplayBusyPin, EPD_WIDTH, EPD_HEIGHT);

unsigned char DisplayImageBlack[(EPD_WIDTH * EPD_HEIGHT / 8)] = {};
unsigned char DisplayImageRed[(EPD_WIDTH * EPD_HEIGHT / 8)] = {};

Paint DisplayPaintBlack(DisplayImageBlack, epd.width, epd.height);
Paint DisplayPaintRed(DisplayImageRed, epd.width, epd.height);

void DisplayDraw()
{
    if (!DisplayPaintBlack.hasChanged() && !DisplayPaintRed.hasChanged())
    {
        return;
    }

    if (State.Display.Sleep)
    {
        Debug("Display", "Reset");
        epd.Wakeup();
    }
    else
    {
        Debug("Display", "Init");
        if (epd.Init() != 0)
        {
            return;
        }
    }

    epd.DisplayFrame(DisplayPaintBlack.GetImage(), DisplayPaintRed.GetImage());
    DisplayPaintBlack.resetChanged();
    DisplayPaintRed.resetChanged();
    Debug("Display", "Frame");
    epd.DisplayFrame();
    Debug("Display", "Sleep");
    epd.Sleep();
    State.Display.Sleep = true;
}

void PaintTest()
{
    if (!State.Display.painted)
    {
        State.Display.painted = true;

        DisplayPaintBlack.Clear(UNCOLORED);
        DisplayPaintRed.Clear(UNCOLORED);

        DisplayPaintBlack.DrawStringAt(0, 0, "e-Paper Demo", &Font24, COLORED);
        DisplayPaintRed.DrawStringAt(0, 150 - 12, "e-Paper Demo", &Font24, COLORED);

        DisplayDraw();
    }
}

void DisplayStartup()
{
    State.Display.Sleep = false;
}

void DisplayWakeup()
{
    State.Display.Sleep = false;
}

void DisplayUpdate()
{
    PaintTest();

    if (State.Buttons.Button1)
    {
        State.Buttons.Button1 = false;
        DisplayPaintBlack.Clear(UNCOLORED);
        DisplayPaintRed.Clear(UNCOLORED);
        DisplayDraw();
    }
}

void DisplayShutdown()
{
}

#endif