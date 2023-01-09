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

#ifndef DisplayDrawModules_h
#define DisplayDrawModules_h

#include <Arduino.h>
#include "Pins.h"
#include "Debug.h"
#include "Settings.h"
#include "Transient.h"
#include "State.h"

#include "UniversalTime.h"
#include "Display.h"

enum TextAlign
{
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    CenterCenter,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

typedef struct DisplayParameter
{
    char type[10];      //
    int x;              //
    int y;              //
    DisplayColor color; //
    union
    {
        int x2;    //
        int width; //
    };
    union
    {
        int y2;     //
        int height; //
    };
    union
    {
        int radius; //
        int size;   //
    };
    union
    {
        char label[60]; //
        char name[60];  //
    };
    bool fill; //
    TextAlign align;
} DisplayParameter;

void DisplayDrawPixel(int x, int y, DisplayColor color)
{
    if (color == DisplayColor::White)
    {
        DisplayPaintBlack.DrawPixel(x, y, PaintColor::UNCOLORED);
        DisplayPaintRed.DrawPixel(x, y, PaintColor::UNCOLORED);
    }
    else if (color == DisplayColor::Black)
    {
        DisplayPaintBlack.DrawPixel(x, y, PaintColor::COLORED);
        DisplayPaintRed.DrawPixel(x, y, PaintColor::UNCOLORED);
    }
    else if (color == DisplayColor::Red)
    {
        DisplayPaintBlack.DrawPixel(x, y, PaintColor::UNCOLORED);
        DisplayPaintRed.DrawPixel(x, y, PaintColor::COLORED);
    }
}
void DisplayDrawLine(int x1, int y1, int x2, int y2, DisplayColor color)
{
    if (color == DisplayColor::White)
    {
        DisplayPaintBlack.DrawLine(x1, y1, x2, y2, PaintColor::UNCOLORED);
        DisplayPaintRed.DrawLine(x1, y1, x2, y2, PaintColor::UNCOLORED);
    }
    else if (color == DisplayColor::Black)
    {
        DisplayPaintBlack.DrawLine(x1, y1, x2, y2, PaintColor::COLORED);
        DisplayPaintRed.DrawLine(x1, y1, x2, y2, PaintColor::UNCOLORED);
    }
    else if (color == DisplayColor::Red)
    {
        DisplayPaintBlack.DrawLine(x1, y1, x2, y2, PaintColor::UNCOLORED);
        DisplayPaintRed.DrawLine(x1, y1, x2, y2, PaintColor::COLORED);
    }
}
void DisplayDrawCircle(int x, int y, int radius, bool fill, DisplayColor color)
{
    if (color == DisplayColor::White)
    {
        if (fill)
        {
            DisplayPaintBlack.DrawFilledCircle(x, y, radius, PaintColor::UNCOLORED);
            DisplayPaintRed.DrawFilledCircle(x, y, radius, PaintColor::UNCOLORED);
        }
        else
        {
            DisplayPaintBlack.DrawCircle(x, y, radius, PaintColor::UNCOLORED);
            DisplayPaintRed.DrawCircle(x, y, radius, PaintColor::UNCOLORED);
        }
    }
    else if (color == DisplayColor::Black)
    {
        if (fill)
        {
            DisplayPaintBlack.DrawFilledCircle(x, y, radius, PaintColor::COLORED);
            DisplayPaintRed.DrawFilledCircle(x, y, radius, PaintColor::UNCOLORED);
        }
        else
        {
            DisplayPaintBlack.DrawCircle(x, y, radius, PaintColor::COLORED);
            DisplayPaintRed.DrawCircle(x, y, radius, PaintColor::UNCOLORED);
        }
    }
    else if (color == DisplayColor::Red)
    {
        if (fill)
        {
            DisplayPaintBlack.DrawFilledCircle(x, y, radius, PaintColor::UNCOLORED);
            DisplayPaintRed.DrawFilledCircle(x, y, radius, PaintColor::COLORED);
        }
        else
        {
            DisplayPaintBlack.DrawCircle(x, y, radius, PaintColor::UNCOLORED);
            DisplayPaintRed.DrawCircle(x, y, radius, PaintColor::COLORED);
        }
    }
}
void DisplayDrawRectangle(int x, int y, int width, int height, bool fill, DisplayColor color)
{
    if (color == DisplayColor::White)
    {
        if (fill)
        {
            DisplayPaintBlack.DrawFilledRectangle(x, y, x + width, y + height, PaintColor::UNCOLORED);
            DisplayPaintRed.DrawFilledRectangle(x, y, x + width, y + height, PaintColor::UNCOLORED);
        }
        else
        {
            DisplayPaintBlack.DrawRectangle(x, y, x + width, y + height, PaintColor::UNCOLORED);
            DisplayPaintRed.DrawRectangle(x, y, x + width, y + height, PaintColor::UNCOLORED);
        }
    }
    else if (color == DisplayColor::Black)
    {
        if (fill)
        {
            DisplayPaintBlack.DrawFilledRectangle(x, y, x + width, y + height, PaintColor::COLORED);
            DisplayPaintRed.DrawFilledRectangle(x, y, x + width, y + height, PaintColor::UNCOLORED);
        }
        else
        {
            DisplayPaintBlack.DrawRectangle(x, y, x + width, y + height, PaintColor::COLORED);
            DisplayPaintRed.DrawRectangle(x, y, x + width, y + height, PaintColor::UNCOLORED);
        }
    }
    else if (color == DisplayColor::Red)
    {
        if (fill)
        {
            DisplayPaintBlack.DrawFilledRectangle(x, y, x + width, y + height, PaintColor::UNCOLORED);
            DisplayPaintRed.DrawFilledRectangle(x, y, x + width, y + height, PaintColor::COLORED);
        }
        else
        {
            DisplayPaintBlack.DrawRectangle(x, y, x + width, y + height, PaintColor::UNCOLORED);
            DisplayPaintRed.DrawRectangle(x, y, x + width, y + height, PaintColor::COLORED);
        }
    }
}
void DisplayDrawText(int x, int y, int width, int height, DisplayColor color, int size, TextAlign align, String text)
{
    int textX = 0;
    int textY = 0;
    int textWidth = 0;
    int textHeight = 0;
    sFONT *font;
    if (size <= 8)
    {
        font = &Font8;
    }
    else if (size == 12)
    {
        font = &Font12;
    }
    else if (size == 16)
    {
        font = &Font16;
    }
    else if (size == 20)
    {
        font = &Font20;
    }
    else if (size >= 24)
    {
        font = &Font24;
    }
    else
    {
        font = &Font16;
    }
    textWidth = text.length() * font->Width;
    textHeight = font->Height;
    
    // textX
    if (align == TextAlign::TopLeft || align == TextAlign::CenterLeft || align == TextAlign::BottomLeft)
    {
        textX = x;
    }
    else if (align == TextAlign::TopCenter || align == TextAlign::CenterCenter || align == TextAlign::BottomCenter)
    {
        textX = (int)((float)x + ((float)width / 2.0) - ((float)textWidth / 2.0));
    }
    else if (align == TextAlign::TopRight || align == TextAlign::CenterRight || align == TextAlign::BottomRight)
    {
        textX = (int)((float)x + ((float)width) - ((float)textWidth / 2.0));
    }
    // textY
    if (align == TextAlign::TopLeft || align == TextAlign::TopCenter || align == TextAlign::TopRight)
    {
        textY = y;
    }
    else if (align == TextAlign::CenterLeft || align == TextAlign::CenterCenter || align == TextAlign::CenterRight)
    {
        //DebugValue(((float)y));
        //DebugValue((((float)height) / 2.0));
        //DebugValue((((float)textHeight) / 2.0));
        
        textY = (int)(((float)y) + (((float)height) / 2.0) - (((float)font->Baseline) / 2.0));
    }
    else if (align == TextAlign::BottomLeft || align == TextAlign::BottomCenter || align == TextAlign::BottomRight)
    {
        textY = (int)((float)y + ((float)height) - ((float)textHeight / 2.0));
    }
    //
    if (color == DisplayColor::White)
    {
        DisplayPaintBlack.DrawStringAt(textX, textY, text.c_str(), font, PaintColor::UNCOLORED);
        DisplayPaintRed.DrawStringAt(textX, textY, text.c_str(), font, PaintColor::UNCOLORED);
    }
    else if (color == DisplayColor::Black)
    {
        DisplayPaintBlack.DrawStringAt(textX, textY, text.c_str(), font, PaintColor::COLORED);
        DisplayPaintRed.DrawStringAt(textX, textY, text.c_str(), font, PaintColor::UNCOLORED);
    }
    else if (color == DisplayColor::Red)
    {
        DisplayPaintBlack.DrawStringAt(textX, textY, text.c_str(), font, PaintColor::UNCOLORED);
        DisplayPaintRed.DrawStringAt(textX, textY, text.c_str(), font, PaintColor::COLORED);
    }
   /* DebugValue(textX);
    DebugValue(textY);
     DebugValue(textWidth);
     DebugValue(textHeight);*/
}
void DisplayDrawValue(int x, int y, int width, int height, DisplayColor color, int size, TextAlign align, String name)
{
    String text = "VALUE";
    DisplayDrawText(x, y, width, height, color, size, align, text);
}
void DisplayDrawImage(int x, int y, int width, int height, DisplayColor color)
{
    if (color == DisplayColor::White)
    {
        // DisplayPaintBlack.DrawAbsolutePixel(x, y, PaintColor::UNCOLORED);
        // DisplayPaintRed.DrawAbsolutePixel(x, y, PaintColor::UNCOLORED);
    }
    else if (color == DisplayColor::Black)
    {
        // DisplayPaintBlack.DrawAbsolutePixel(x, y, PaintColor::COLORED);
        // DisplayPaintRed.DrawAbsolutePixel(x, y, PaintColor::UNCOLORED);
    }
    else if (color == DisplayColor::Red)
    {
        // DisplayPaintBlack.DrawAbsolutePixel(x, y, PaintColor::UNCOLORED);
        // DisplayPaintRed.DrawAbsolutePixel(x, y, PaintColor::COLORED);
    }
}

void DisplayDrawParameter(DisplayParameter *param)
{
    if (strcmp(param->type, "pixel") == 0)
    {
        Debug("DisplayDraw", String(param->type));
        DisplayDrawPixel(param->x, param->y, param->color);
    }
    else if (strcmp(param->type, "line") == 0)
    {
        Debug("DisplayDraw", String(param->type));
        DisplayDrawLine(param->x, param->y, param->x2, param->y2, param->color);
    }
    else if (strcmp(param->type, "circle") == 0)
    {
        Debug("DisplayDraw", String(param->type));
        DisplayDrawCircle(param->x, param->y, param->radius, param->fill, param->color);
    }
    else if (strcmp(param->type, "rect") == 0)
    {
        Debug("DisplayDraw", String(param->type));
        DisplayDrawRectangle(param->x, param->y, param->width, param->height, param->fill, param->color);
    }
    else if (strcmp(param->type, "text") == 0)
    {
        Debug("DisplayDraw", String(param->type));
        DisplayDrawText(param->x, param->y, param->width, param->height, param->color, param->size, param->align, String(param->label));
    }
    else if (strcmp(param->type, "value") == 0)
    {
        Debug("DisplayDraw", String(param->type));
        DisplayDrawText(param->x, param->y, param->width, param->height, param->color, param->size, param->align, String(param->name));
    }
    else
    {
        Debug("DisplayDrawNone", String(param->type));
    }
}

String DisplayParseKey(String keyAndValue)
{
    char sep = 187;
    int sepIndex = keyAndValue.indexOf(sep);
    if (sepIndex >= 0)
    {
        return keyAndValue.substring(0, sepIndex - 1);
    }
    return "";
}
String DisplayParseValue(String keyAndValue)
{
    char sep = 187;
    int sepIndex = keyAndValue.indexOf(sep);
    if (sepIndex >= 0)
    {
        return keyAndValue.substring(sepIndex + 1);
    }
    return "";
}

void DisplayParseLine(String line)
{
    int lineLength = line.length();
    int seperatorPositionLast = 0;
    int position = 0;

    char sep = 166;
    line += String(sep);

    DisplayParameter parameter = {};
    bool exist = false;
    while (true)
    {
        position = line.indexOf(sep, position + 1);
        if (position < 0)
        {
            break;
        }
        String keyAndValue = line.substring(seperatorPositionLast, position - 1);
        if (keyAndValue.length() > 0)
        {
            String key = DisplayParseKey(keyAndValue);
            String value = DisplayParseValue(keyAndValue);
            exist = true;
            // Type
            if (key == "t")
            {
                strcpy(parameter.type, value.c_str());
            }
            // Position x
            else if (key == "x" || key == "x1")
            {
                parameter.x = value.toInt();
            }
            // Position y
            else if (key == "y" || key == "y1")
            {
                parameter.y = value.toInt();
            }
            // Color
            else if (key == "c")
            {
                if (value == "w")
                {
                    parameter.color = DisplayColor::White;
                }
                else if (value == "b")
                {
                    parameter.color = DisplayColor::Black;
                }
                else if (value == "r")
                {
                    parameter.color = DisplayColor::Red;
                }
            }
            // Position x2 or Width
            else if (key == "x2" || key == "w")
            {
                parameter.x2 = value.toInt();
            }
            // Position y2 or Height
            else if (key == "y2" || key == "h")
            {
                parameter.y2 = value.toInt();
            }
            // Position Radius or Size
            else if (key == "r" || key == "s")
            {
                parameter.radius = value.toInt();
            }
            // Position Radius or Size
            else if (key == "l" || key == "n")
            {
                strcpy(parameter.label, value.c_str());
            }
            // Position Radius or Size
            else if (key == "f")
            {
                parameter.fill = value == "1";
            }
            // TextAlign
            else if (key == "a")
            {
                if (value.compareTo("tl") == 0)
                {
                    parameter.align = TextAlign::TopLeft;
                }
                else if (value.compareTo("tc") == 0)
                {
                    parameter.align = TextAlign::TopCenter;
                }
                else if (value.compareTo("tr") == 0)
                {
                    parameter.align = TextAlign::TopRight;
                }
                else if (value.compareTo("cl") == 0)
                {
                    parameter.align = TextAlign::CenterLeft;
                }
                else if (value.compareTo("cc") == 0)
                {
                    parameter.align = TextAlign::CenterCenter;
                }
                else if (value.compareTo("cr") == 0)
                {
                    parameter.align = TextAlign::CenterRight;
                }
                else if (value.compareTo("bl") == 0)
                {
                    parameter.align = TextAlign::BottomLeft;
                }
                else if (value.compareTo("bc") == 0)
                {
                    parameter.align = TextAlign::BottomCenter;
                }
                else if (value.compareTo("br") == 0)
                {
                    parameter.align = TextAlign::BottomRight;
                }
                else {
                    Debug("TextAlignUnknown",value);
                }
            }
        }
        seperatorPositionLast = position + 1;
    }

if (exist) {
     DebugStruct(parameter);
    DebugStructValue(parameter.type);
    DebugStructValue(parameter.x);
    DebugStructValue(parameter.y);
    DebugStructValue(parameter.x2);
    DebugStructValue(parameter.y2);
    DebugStructValue(parameter.width);
    DebugStructValue(parameter.height);
    DebugStructValue(parameter.radius);
    DebugStructValue(parameter.label);
    DebugStructValue(parameter.name);
    DebugStructValue(parameter.fill);
    DebugStructValue(parameter.color);
    DebugStructValue(parameter.align);
    DebugStructEnd();

    DisplayDrawParameter(&parameter);
}
   
}
void DisplayParseText(String text)
{
    DebugValue(text);
    int position = 0;
    int newLinePosition = 0;
    int newLinePositionLast = 0;
    int textLength = text.length();
    text += "\n";
    while (1)
    {
        position = text.indexOf('\n', position + 1);
        if (position < 0)
        {
            break;
        }
        String line = text.substring(newLinePositionLast, position - 1);
        if (line.length() > 0)
        {
            DisplayParseLine(line);
        }

        newLinePositionLast = position + 1;
    }
}

String DisplayToImage()
{

    unsigned char *imageBlack = DisplayPaintBlack.GetImage();
    unsigned char *imageRed = DisplayPaintRed.GetImage();

    // DisplayPaintBlack.DrawFilledRectangle(0,0,40,40,PaintColor::COLORED);

    int length = DisplayPaintBlack.GetWidth() * DisplayPaintBlack.GetHeight() / 8;
    /*char text[length * 2];

    int byte = 0;
    for (int a = 0; a < length; a++)
    {
        text[a] = imageBlack[a];
    }
    for (int a = 0; a < length; a++)
    {
        text[length + a] = imageRed[a];
    }
    //Debug("Image",text);*/

    return "";
}
#endif