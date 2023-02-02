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

int DisplayPositionX = 0;
int DisplayPositionY = 0;
float DisplayScale = 1;

typedef struct DisplayParameterData
{
#define DisplayParameterDataHold "z"
    bool hold;
#define DisplayParameterDataType "t"
    char type[10];

#define DisplayParameterDataX "x"
    int x;
#define DisplayParameterDataY "y"
    int y;
#define DisplayParameterDataColor "c"
    DisplayColor color;
#define DisplayParameterDataX1 "x1"
    int x1;
#define DisplayParameterDataY1 "y1"
    int y1;
#define DisplayParameterDataX2 "x2"
    int x2;
#define DisplayParameterDataY2 "y2"
    int y2;
#define DisplayParameterDataWidth "w"
    int width; //
#define DisplayParameterDataHeight "h"
    int height; //
#define DisplayParameterDataRadius "r"
    int radius; //
#define DisplayParameterDataSize "s"
    int size; //
    union
    {
#define DisplayParameterDataLabelLength 60
#define DisplayParameterDataPointsLength DisplayParameterDataLabelLength / 4
#define DisplayParameterDataLabel "l"
        char label[DisplayParameterDataLabelLength]; //
#define DisplayParameterDataName "n"
        char name[DisplayParameterDataLabelLength]; //

        int16_t points[DisplayParameterDataPointsLength];
    };
#define DisplayParameterDataFill "f"
    bool fill; //
#define DisplayParameterDataAlign "a"
    TextAlign align;
} DisplayParameterData;

void CopyDisplayParameterData(DisplayParameterData *dest, DisplayParameterData *source)
{
    strcpy(dest->type, source->type);
    dest->x = source->x;
    dest->y = source->y;
    dest->x2 = source->x2;
    dest->y2 = source->y2;
    dest->color = source->color;
    dest->radius = source->radius;
    strcpy(dest->label, source->label);
    dest->x = source->x;
    dest->fill = source->fill;
    dest->align = source->align;
}

PaintColor DisplayColorToPaintColor(DisplayColor color, DisplayColor reference)
{
    if (color == reference)
    {
        return PaintColor::COLORED;
    }
    else
    {
        return PaintColor::UNCOLORED;
    }
}

void DisplaySetPositionScale(int x, int y, float scale)
{
    DisplayPositionX = x;
    DisplayPositionY = y;
    DisplayScale = scale;
}

void DisplayDrawPixel(int x, int y, DisplayColor color)
{
    int localX = DisplayPositionX + (float)x * DisplayScale;
    int localY = DisplayPositionY + (float)y * DisplayScale;

    if (color == DisplayColor::White)
    {
        DisplayPaintBlack.DrawPixel(localX, localY, PaintColor::UNCOLORED);
        DisplayPaintRed.DrawPixel(localX, localY, PaintColor::UNCOLORED);
    }
    else if (color == DisplayColor::Black)
    {
        DisplayPaintBlack.DrawPixel(localX, localY, PaintColor::COLORED);
        DisplayPaintRed.DrawPixel(localX, localY, PaintColor::UNCOLORED);
    }
    else if (color == DisplayColor::Red)
    {
        DisplayPaintBlack.DrawPixel(localX, localY, PaintColor::UNCOLORED);
        DisplayPaintRed.DrawPixel(localX, localY, PaintColor::COLORED);
    }
}
void DisplayDrawLine(int x1, int y1, int x2, int y2, DisplayColor color)
{
    int localX1 = DisplayPositionX + (float)x1 * DisplayScale;
    int localY1 = DisplayPositionY + (float)y1 * DisplayScale;
    int localX2 = DisplayPositionX + (float)x2 * DisplayScale;
    int localY2 = DisplayPositionY + (float)y2 * DisplayScale;
    PaintColor blackColor = DisplayColorToPaintColor(color, DisplayColor::Black);
    PaintColor redColor = DisplayColorToPaintColor(color, DisplayColor::Red);

    DisplayPaintBlack.DrawLine(localX1, localY1, localX2, localY2, blackColor);
    DisplayPaintRed.DrawLine(localX1, localY1, localX2, localY2, redColor);
}
void DisplayDrawCircle(int x, int y, int radius, bool fill, DisplayColor color)
{
    int localX = DisplayPositionX + (float)x * DisplayScale;
    int localY = DisplayPositionY + (float)y * DisplayScale;
    int localRadius = (float)radius * DisplayScale;

    PaintColor blackColor = DisplayColorToPaintColor(color, DisplayColor::Black);
    PaintColor redColor = DisplayColorToPaintColor(color, DisplayColor::Red);

    if (fill)
    {
        DisplayPaintBlack.DrawFilledCircle(localX, localY, localRadius, blackColor);
        DisplayPaintRed.DrawFilledCircle(localX, localY, localRadius, redColor);
    }
    else
    {
        DisplayPaintBlack.DrawCircle(localX, localY, localRadius, blackColor);
        DisplayPaintRed.DrawCircle(localX, localY, localRadius, redColor);
    }
}
void DisplayDrawRectangle(int x, int y, int width, int height, bool fill, DisplayColor color)
{
    int localX1 = DisplayPositionX + (float)x * DisplayScale;
    int localY1 = DisplayPositionY + (float)y * DisplayScale;
    int localX2 = localX1 + (float)width * DisplayScale;
    int localY2 = localY1 + (float)height * DisplayScale;

    PaintColor blackColor = DisplayColorToPaintColor(color, DisplayColor::Black);
    PaintColor redColor = DisplayColorToPaintColor(color, DisplayColor::Red);

    if (fill)
    {
        DisplayPaintBlack.DrawFilledRectangle(localX1, localY1, localX2, localY2, blackColor);
        DisplayPaintRed.DrawFilledRectangle(localX1, localY1, localX2, localY2, redColor);
    }
    else
    {
        DisplayPaintBlack.DrawRectangle(localX1, localY1, localX2, localY2, blackColor);
        DisplayPaintRed.DrawRectangle(localX1, localY1, localX2, localY2, redColor);
    }
}
void DisplayDrawCircleSegment(int x, int y, int startRadius, int endRadius, int startAngle, int endAngle, bool fill, DisplayColor color)
{
    int localX = DisplayPositionX + (float)x * DisplayScale;
    int localY = DisplayPositionY + (float)y * DisplayScale;
    int localStartRadius = (float)startRadius * DisplayScale;
    int localEndAngle = (float)endAngle * DisplayScale;
    PaintColor blackColor = DisplayColorToPaintColor(color, DisplayColor::Black);
    PaintColor redColor = DisplayColorToPaintColor(color, DisplayColor::Red);
    if (fill)
    {
        DisplayPaintBlack.DrawFilledCircleSegment(localX, localY, localStartRadius, localEndAngle, startAngle, endAngle, blackColor);
        DisplayPaintRed.DrawFilledCircleSegment(localX, localY, localStartRadius, localEndAngle, startAngle, endAngle, redColor);
    }
    else
    {
        DisplayPaintBlack.DrawCircleSegment(localX, localY, localStartRadius, startAngle, endAngle, blackColor);
        DisplayPaintRed.DrawCircleSegment(localX, localY, localStartRadius, startAngle, endAngle, redColor);
    }
}
void DisplayDrawFilledPolygone(int *arrX, int *arrY, int count, bool fill, DisplayColor color)
{
    int localXs[count];
    int localYs[count];
    PaintColor blackColor = DisplayColorToPaintColor(color, DisplayColor::Black);
    PaintColor redColor = DisplayColorToPaintColor(color, DisplayColor::Red);

    for (int a = 0; a < count; a++)
    {
        localXs[a] = DisplayPositionX + (float)arrX[a] * DisplayScale;
        localYs[a] = DisplayPositionY + (float)arrY[a] * DisplayScale;
        if (!fill)
        {
            int nextA = a + 1;
            if (nextA >= count) {
                nextA = 0;
            }
            DisplayPaintBlack.DrawLine(localXs[a], localYs[a], localXs[nextA], localYs[nextA], blackColor);
            DisplayPaintRed.DrawLine(localXs[a], localYs[a], localXs[nextA], localYs[nextA], redColor);
        }
    }
    if (fill)
    {
        DisplayPaintBlack.DrawFilledPolygone(localXs, localYs, count, blackColor);
        DisplayPaintRed.DrawFilledPolygone(localXs, localYs, count, redColor);
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
        // DebugValue(((float)y));
        // DebugValue((((float)height) / 2.0));
        // DebugValue((((float)textHeight) / 2.0));

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

void DisplayDrawParameter(DisplayParameterData *param)
{
    if (strcmp(param->type, "pos") == 0)
    {
        DisplaySetPositionScale(param->x, param->y,(float)param->size/100.0);
    }
    else if (strcmp(param->type, "pixel") == 0)
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
    else if (strcmp(param->type, "cicseg") == 0)
    {
        Debug("DisplayDraw", String(param->type));
        DisplayDrawCircleSegment(
            param->x,      // int x,
            param->y,      // int y,
            param->radius, // int startRadius,
            param->size,   // int endRadius,
            param->x2,     // int startAngle,
            param->y2,     //  int endAngle,
            param->fill,   //  bool fill,
            param->color   // DisplayColor color
        );
    }
    else if (strcmp(param->type, "rect") == 0)
    {
        Debug("DisplayDraw", String(param->type));
        DisplayDrawRectangle(param->x, param->y, param->width, param->height, param->fill, param->color);
    }
    else if (strcmp(param->type, "poly") == 0)
    {
        Debug("DisplayDraw", String(param->type));

        int arrX[param->size] = {};
        int arrY[param->size] = {};

        for (int a = 0; a < (param->size); a++)
        {
            arrX[a] = (int)param->points[a * 2];
            arrY[a] = (int)param->points[a * 2 + 1];
            Debug("ArrX" + String(a), arrX[a]);
            Debug("ArrY" + String(a), arrY[a]);
        }
        Debug("Count", param->size);
        DisplayDrawFilledPolygone(
            arrX,        // int x,
            arrY,        // int y,
            param->size, // int endRadius,
            param->fill,
            param->color // DisplayColor color
        );
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

DisplayParameterData DisplayParameterOld = {};

void DisplayParseText(char *text, int length)
{
    DebugValue(text);

    ;
    DebugValue(length);

    bool key = true;
    char keyText[DisplayParameterDataLabelLength];
    int keyTextIndex = 0;
    bool value = false;
    int valueTextIndex = 0;
    char valueText[DisplayParameterDataLabelLength];

    for (int f = 0; f < 60; f++)
    {
        keyText[f] = 0;
        valueText[f] = 0;
    }
    keyTextIndex = 0;
    valueTextIndex = 0;

    DisplayParameterData parameter = {};
    int justread = 0;
    for (int a = 0; a < length; a++)
    {
        char chararacter = text[a];
        if (key)
        {
            if (chararacter == 187)
            {
                key = false;
                value = true;
                Debug("Key Done", keyText);
                if (strcmp(parameter.type, "poly") == 0 && strcmp(keyText, "n") == 0)
                {
                    justread = parameter.size * 4;
                    DebugValue(justread);
                }
            }
            else if (chararacter == '\n')
            {
                // Line done
                DebugStruct(parameter);
                DebugStructValue(parameter.type);
                DebugStructValue(parameter.x);
                DebugStructValue(parameter.y);
                DebugStructValue(parameter.x2);
                DebugStructValue(parameter.y2);
                DebugStructValue(parameter.width);
                DebugStructValue(parameter.height);
                DebugStructValue(parameter.radius);
                DebugStructValue(parameter.size);
                DebugStructValue(parameter.label);
                for (int f = 0; f < DisplayParameterDataPointsLength; f++)
                {
                    DebugStructValue(parameter.points[f]);
                }
                DebugStructValue(parameter.name);
                DebugStructValue(parameter.fill);
                DebugStructValue(parameter.color);
                DebugStructValue(parameter.align);
                DebugStructEnd();
                DisplayDrawParameter(&parameter);
                CopyDisplayParameterData(&DisplayParameterOld, &parameter);
                parameter = {};
            }
            else
            {
                keyText[keyTextIndex] = chararacter;
                keyTextIndex++;
            }
        }
        else if (value)
        {
            if (justread > 0)
            {
                valueText[valueTextIndex] = chararacter;
                valueTextIndex++;
                justread--;
            }
            else if (chararacter == 166)
            {
                key = true;
                value = false;
                Debug("Value Done", valueText);
                bool appand = valueText[0] == '+' || valueText[0] == '-';
                // Hold
                if (strcmp(keyText, DisplayParameterDataHold) == 0)
                {
                    if (strcmp(valueText, "1") == 0)
                    {
                        CopyDisplayParameterData(&parameter, &DisplayParameterOld);
                        parameter.hold = true;
                    }
                }
                // Type
                if (strcmp(keyText, DisplayParameterDataType) == 0)
                {
                    Debug("TypeValue", valueText);
                    Debug("TypeParm", parameter.type);
                    strcpy(parameter.type, valueText);
                }
                // Position x
                else if (strcmp(keyText, DisplayParameterDataX) == 0)
                {
                    if (appand && parameter.hold)
                    {
                        parameter.x += atoi(valueText);
                    }
                    else
                    {
                        parameter.x = atoi(valueText);
                    }
                }
                // Position y
                else if (strcmp(keyText, DisplayParameterDataY) == 0)
                {
                    if (appand && parameter.hold)
                    {
                        parameter.y += atoi(valueText);
                    }
                    else
                    {
                        parameter.y = atoi(valueText);
                    }
                }
                // Color
                else if (strcmp(keyText, DisplayParameterDataColor) == 0)
                {
                    if (strcmp(valueText, "w") == 0)
                    {
                        parameter.color = DisplayColor::White;
                    }
                    else if (strcmp(valueText, "b") == 0)
                    {
                        parameter.color = DisplayColor::Black;
                    }
                    else if (strcmp(valueText, "r") == 0)
                    {
                        parameter.color = DisplayColor::Red;
                    }
                }
                // Position x1
                else if (strcmp(keyText, DisplayParameterDataX1) == 0)
                {
                    if (appand && parameter.hold)
                    {
                        parameter.x1 += atoi(valueText);
                    }
                    else
                    {
                        parameter.x1 = atoi(valueText);
                    }
                }
                // Position y1
                else if (strcmp(keyText, DisplayParameterDataY1) == 0)
                {
                    if (appand && parameter.hold)
                    {
                        parameter.y1 += atoi(valueText);
                    }
                    else
                    {
                        parameter.y1 = atoi(valueText);
                    }
                }
                // Position x2
                else if (strcmp(keyText, DisplayParameterDataX2) == 0)
                {
                    if (appand && parameter.hold)
                    {
                        parameter.x2 += atoi(valueText);
                    }
                    else
                    {
                        parameter.x2 = atoi(valueText);
                    }
                }
                // Position y2
                else if (strcmp(keyText, DisplayParameterDataY2) == 0)
                {
                    if (appand && parameter.hold)
                    {
                        parameter.y2 += atoi(valueText);
                    }
                    else
                    {
                        parameter.y2 = atoi(valueText);
                    }
                }
                // width
                else if (strcmp(keyText, DisplayParameterDataWidth) == 0)
                {
                    parameter.width = atoi(valueText);
                }
                // Height
                else if (strcmp(keyText, DisplayParameterDataHeight) == 0)
                {
                    parameter.height = atoi(valueText);
                }
                // Position Radius
                else if (strcmp(keyText, DisplayParameterDataRadius) == 0)
                {
                    if (appand && parameter.hold)
                    {
                        parameter.radius += atoi(valueText);
                    }
                    else
                    {
                        parameter.radius = atoi(valueText);
                    }
                }
                // Size
                else if (strcmp(keyText, DisplayParameterDataSize) == 0)
                {
                    if (appand && parameter.hold)
                    {
                        parameter.size += atoi(valueText);
                    }
                    else
                    {
                        parameter.size = atoi(valueText);
                    }
                }
                // Label or Name
                else if (strcmp(keyText, DisplayParameterDataLabel) == 0 || strcmp(keyText, DisplayParameterDataName) == 0)
                {
                    if (strcmp(parameter.type, "poly") == 0)
                    {
                        Debug("IS", "POLY");
                        for (int f = 0; f < DisplayParameterDataLabelLength; f++)
                        {
                            parameter.label[f] = valueText[f];
                        }
                    }
                    else
                    {
                        strcpy(parameter.label, valueText);
                    }
                }
                // Fill
                else if (strcmp(keyText, DisplayParameterDataFill) == 0)
                {
                    parameter.fill = strcmp(valueText, "1") == 0;
                }
                // TextAlign
                else if (strcmp(keyText, DisplayParameterDataAlign) == 0)
                {
                    if (strcmp(valueText, "tl") == 0)
                    {
                        parameter.align = TextAlign::TopLeft;
                    }
                    else if (strcmp(valueText, "tc") == 0)
                    {
                        parameter.align = TextAlign::TopCenter;
                    }
                    else if (strcmp(valueText, "tr") == 0)
                    {
                        parameter.align = TextAlign::TopRight;
                    }
                    else if (strcmp(valueText, "cl") == 0)
                    {
                        parameter.align = TextAlign::CenterLeft;
                    }
                    else if (strcmp(valueText, "cc") == 0)
                    {
                        parameter.align = TextAlign::CenterCenter;
                    }
                    else if (strcmp(valueText, "cr") == 0)
                    {
                        parameter.align = TextAlign::CenterRight;
                    }
                    else if (strcmp(valueText, "bl") == 0)
                    {
                        parameter.align = TextAlign::BottomLeft;
                    }
                    else if (strcmp(valueText, "bc") == 0)
                    {
                        parameter.align = TextAlign::BottomCenter;
                    }
                    else if (strcmp(valueText, "br") == 0)
                    {
                        parameter.align = TextAlign::BottomRight;
                    }
                    else
                    {
                        Debug("TextAlignUnknown", valueText);
                    }
                }

                for (int f = 0; f < DisplayParameterDataLabelLength; f++)
                {
                    keyText[f] = 0;
                    valueText[f] = 0;
                }
                keyTextIndex = 0;
                valueTextIndex = 0;
            }
            else
            {
                valueText[valueTextIndex] = chararacter;
                valueTextIndex++;
            }
        }
    }
}

#endif