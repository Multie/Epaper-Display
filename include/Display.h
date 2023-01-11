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

enum DisplayColor
{
    White,
    Black,
    Red
};

#include "Display/DisplayDrawModules.h"

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

#ifdef TestWebServer
uint16_t indexvReal = 0;
char* postData;
int postDataSize =0;
bool DisplayServerGodData = false;
void DisplaySetupServer()
{
    server.on(
        "/api/display/image",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            indexvReal = 0;
            AsyncWebServerResponse *response = request->beginChunkedResponse(
                "text/plain",
                [](uint8_t *buffer, size_t maxLen, size_t index)
                {
                    maxLen = maxLen >> 1;
                    size_t len = 0;
                    int len1 = DisplayPaintBlack.GetWidth() * DisplayPaintBlack.GetHeight() / 8;
                    int len2 = len1 * 2;

                    while (len < (maxLen - 20) && indexvReal < (len2))
                    {
                        if (indexvReal < len1)
                        {
                            len += sprintf(((char *)buffer + len), "%c", DisplayImageBlack[indexvReal]);
                        }
                        else
                        {
                            len += sprintf(((char *)buffer + len), "%c", DisplayImageRed[indexvReal - len1]);
                        }
                        indexvReal++;
                    }
                    return len;
                });
            response->addHeader("Access-Control-Allow-Origin", "*");
            response->addHeader("Access-Control-Allow-Methods", "GET, POST");
            response->addHeader("Access-Control-Allow-Headers", "Content-Type");
            request->send(response);
        });
    server.on(
        "/api/display/image",
        HTTP_POST,
        [](AsyncWebServerRequest *request)
        {
            AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "123");
            response->addHeader("Access-Control-Allow-Origin", "*");
            response->addHeader("Access-Control-Allow-Methods", "GET, POST");
            response->addHeader("Access-Control-Allow-Headers", "Content-Type");
            request->send(response);
        },
        NULL,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
            Debug("HttpLen", len);
            if (index == 0)
            {
                postData = (char*) malloc(total);
                postDataSize = total;
            }
            for (int a = 0; a <index + len; a++)
            {
                postData[index + a] = (char)data[a];
            }
            if (index + len == total)
            {
                AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "");
                response->addHeader("Access-Control-Allow-Origin", "*");
                response->addHeader("Access-Control-Allow-Methods", "GET, POST");
                response->addHeader("Access-Control-Allow-Headers", "Content-Type");
                request->send(response);
                DisplayServerGodData = true;
            }
        });
}
#endif

void DisplayStartup()
{
    State.Display.Sleep = false;
#ifdef TestWebServer
    DisplaySetupServer();
#endif
    // DisplayParseText("t»rect¦x»10¦y»10¦w»20¦h»20¦f»1¦c»b¦\nt»rect¦x»30¦y»10¦w»20¦h»20¦f»1¦c»r¦");
    //  DisplayParseText("t»pixel¦x»0¦y»0¦c»b¦\nt»circle¦x»0¦y»0¦r»0¦c»b¦\nt»line¦x1»0¦y1»0¦x2»0¦y2»0¦c»b¦\nt»rect¦x»0¦y»0¦w»0¦h»0¦c»b¦\nt»text¦x»0¦y»0¦w»0¦h»0¦c»b¦s»16¦a»cc¦l»¦\nt»value¦x»0¦y»0¦w»0¦h»0¦c»b¦s»16¦a»cc¦v»¦\nt»image¦x»0¦y»0¦w»0¦h»0¦c»b¦n»¦");

    // DisplayPaintRed.DrawCircleSegment(100,100,25,0,180,PaintColor::COLORED);
   
    DisplayPaintBlack.Clear(PaintColor::UNCOLORED);
    int x[] = {50, 100, 20};
    int y[] = {10, 50, 50};
    DisplayPaintBlack.DrawFilledPolygone(x, y, 3, PaintColor::COLORED);
}

void DisplayWakeup()
{
    State.Display.Sleep = false;
#ifdef TestWebServer
    DisplaySetupServer();
#endif
}

void DisplayUpdate()
{
    // PaintTest();
    // Debug("Update", "Display");
    if (State.Buttons.Button1)
    {
        State.Buttons.Button1 = false;
        DisplayPaintBlack.Clear(UNCOLORED);
        DisplayPaintRed.Clear(UNCOLORED);
        DisplayDraw();
    }
#ifdef TestWebServer
    else if (DisplayServerGodData && postDataSize > 0)
    {
        DisplayServerGodData = false;
        DisplayPaintBlack.Clear(UNCOLORED);
        DisplayPaintRed.Clear(UNCOLORED);
        DisplayParseText(postData,postDataSize);

        postDataSize = 0;
        free(postData);
        postData = nullptr;
    }

    /*ClearDisplay = false;
     DisplayPaintBlack.Clear(UNCOLORED);
    DisplayPaintRed.Clear(UNCOLORED);
    postData = "";
    */

#endif
}

void DisplayShutdown()
{
}

#endif