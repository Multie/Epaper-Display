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

#ifndef DisplayDrawHeader_h
#define DisplayDrawHeader_h

#include <Arduino.h>
#include "Pins.h"
#include "Debug.h"
#include "Settings.h"
#include "Transient.h"
#include "State.h"

#include "Display.h"
#include "UniversalTime.h"


int DisplayDrawHeader(Paint *DisplayPaintBlack, Paint *DisplayPaintRed)
{
    UniversalTime now = UniversalTime::Now();
    String timedate = "";
    String adder = "";
    adder = String(now.getHour());
    timedate += adder;
    timedate += ":";
    adder = String(now.getMinute());
    while (adder.length() < 2) {
        adder = " " + adder;
    }
    timedate += adder;
    timedate += " ";

    adder = String(now.getDay());
    while (adder.length() < 2) {
        adder = " " + adder;
    }
    timedate += adder;
    timedate += ".";
    
    adder = String(now.getMonth());
    while (adder.length() < 2) {
        adder = " " + adder;
    }
    timedate += adder;
    timedate += ".";

    adder = String(now.getYear());
    while (adder.length() < 4) {
        adder = " " + adder;
    }
    timedate += adder;
    
    sFONT* font = &Font20; 

    int width = font->Width * timedate.length();
 
    int x = DisplayPaintBlack->GetWidth() / 2 - width / 2;

    DisplayPaintBlack->DrawStringAt(0, 0, timedate.c_str(), font, COLORED);
    DisplayPaintBlack->DrawLine(0,font->Height+1,DisplayPaintBlack->GetWidth(),font->Height+1,COLORED);

    return font->Height+1;
}

#endif