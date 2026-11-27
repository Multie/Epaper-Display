/**
 *  @filename   :   epdpaint.cpp
 *  @brief      :   Paint tools
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     September 9 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "epdpaint.h"

Paint::Paint(unsigned char *image, int width, int height)
{
    this->rotate = PaintRotation::ROTATE_0;
    this->image = image;
    /* 1 byte = 8 pixels, so the width should be the multiple of 8 */
    this->width = width % 8 ? width + 8 - (width % 8) : width;
    this->height = height;
    this->Changed = false;
}

Paint::~Paint()
{
}

/**
 * @brief clear the image
 *
 * @param colored
 */
void Paint::Clear(PaintColor colored)
{
    this->Changed = true;
    for (int x = 0; x < this->width; x++)
    {
        for (int y = 0; y < this->height; y++)
        {
            DrawAbsolutePixel(x, y, colored);
        }
    }
}

/**
 * @brief this draws a pixel by absolute coordinates.
 *          this function won't be affected by the rotate parameter.
 * @param x
 * @param y
 * @param colored
 */
void Paint::DrawAbsolutePixel(int x, int y, PaintColor colored)
{

    if (x < 0 || x >= this->width || y < 0 || y >= this->height)
    {
        return;
    }
    this->Changed = true;
    if (IF_INVERT_COLOR)
    {
        if (colored)
        {
            image[(x + y * this->width) / 8] |= 0x80 >> (x % 8);
        }
        else
        {
            image[(x + y * this->width) / 8] &= ~(0x80 >> (x % 8));
        }
    }
    else
    {
        if (colored)
        {
            image[(x + y * this->width) / 8] &= ~(0x80 >> (x % 8));
        }
        else
        {
            image[(x + y * this->width) / 8] |= 0x80 >> (x % 8);
        }
    }
}

/**
 * @brief Getters and Setters
 *
 * @return image buffer
 */
unsigned char *Paint::GetImage()
{
    return this->image;
}

/**
 * @brief get the width of the image
 *
 * @return width
 */
int Paint::GetWidth()
{
    return this->width;
}

/**
 * @brief set the width of the image
 *
 * @param width
 */
void Paint::SetWidth(int width)
{
    this->width = width % 8 ? width + 8 - (width % 8) : width;
    this->Changed = true;
}

/**
 * @brief  get the height of the image
 *
 * @return int
 */
int Paint::GetHeight()
{
    return this->height;
}

/**
 * @brief set the height of the image
 *
 * @param height
 */
void Paint::SetHeight(int height)
{
    this->height = height;
    this->Changed = true;
}

/**
 * @brief get the rotation of the image
 *
 * @return int
 */
PaintRotation Paint::GetRotate()
{
    return this->rotate;
}

/**
 * @brief set the rotation of the image
 *
 * @param rotate
 */
void Paint::SetRotate(PaintRotation rotate)
{
    this->rotate = rotate;
    this->Changed = true;
}

/**
 * @brief this draws a pixel by the coordinates
 *
 * @param x position x
 * @param y position y
 * @param colored
 */
void Paint::DrawPixel(int x, int y, PaintColor colored)
{

    int point_temp;
    if (this->rotate == ROTATE_0)
    {
        if (x < 0 || x >= this->width || y < 0 || y >= this->height)
        {
            return;
        }
        this->Changed = true;
        DrawAbsolutePixel(x, y, colored);
    }
    else if (this->rotate == ROTATE_90)
    {
        if (x < 0 || x >= this->height || y < 0 || y >= this->width)
        {
            return;
        }
        point_temp = x;
        x = this->width - 1 - y;
        y = point_temp;
        this->Changed = true;
        DrawAbsolutePixel(x, y, colored);
    }
    else if (this->rotate == ROTATE_180)
    {
        if (x < 0 || x >= this->width || y < 0 || y >= this->height)
        {
            return;
        }
        x = this->width - 1 - x;
        y = this->height - 1 - y;
        this->Changed = true;
        DrawAbsolutePixel(x, y, colored);
    }
    else if (this->rotate == ROTATE_270)
    {
        if (x < 0 || x >= this->height || y < 0 || y >= this->width)
        {
            return;
        }
        point_temp = x;
        x = y;
        y = this->height - 1 - point_temp;
        this->Changed = true;
        DrawAbsolutePixel(x, y, colored);
    }
}

/**
 * @brief this draws a charactor on the frame buffer but not refresh
 *
 * @param x position x
 * @param y position y
 * @param ascii_char
 * @param font
 * @param colored
 */
void Paint::DrawCharAt(int x, int y, char ascii_char, sFONT *font, PaintColor colored)
{
    this->Changed = true;
    int i, j;
    unsigned int char_offset = (ascii_char - ' ') * font->Height * (font->Width / 8 + (font->Width % 8 ? 1 : 0));
    const unsigned char *ptr = &font->table[char_offset];

    for (j = 0; j < font->Height; j++)
    {
        for (i = 0; i < font->Width; i++)
        {
            if (pgm_read_byte(ptr) & (0x80 >> (i % 8)))
            {
                DrawPixel(x + i, y + j, colored);
            }
            if (i % 8 == 7)
            {
                ptr++;
            }
        }
        if (font->Width % 8 != 0)
        {
            ptr++;
        }
    }
}

/**
 * @brief this displays a string on the frame buffer but not refresh
 *
 * @param x position x
 * @param y position y
 * @param text
 * @param font
 * @param colored
 */
void Paint::DrawStringAt(int x, int y, const char *text, sFONT *font, PaintColor colored)
{
    this->Changed = true;
    const char *p_text = text;
    unsigned int counter = 0;
    int refcolumn = x;

    /* Send the string character by character on EPD */
    while (*p_text != 0)
    {
        /* Display one character on EPD */
        DrawCharAt(refcolumn, y, *p_text, font, colored);
        /* Decrement the column position by 16 */
        refcolumn += font->Width;
        /* Point on the next character */
        p_text++;
        counter++;
    }
}

/**
 * @brief this draws a line on the frame buffer
 *
 * @param x0 position x
 * @param y0 position y
 * @param x1 position x
 * @param y1 position y
 * @param colored
 */
void Paint::DrawLine(int x0, int y0, int x1, int y1, PaintColor colored)
{
    this->Changed = true;
    /* Bresenham algorithm, beide Endpunkte inklusive, alle Richtungen */
    int dx = x1 - x0 >= 0 ? x1 - x0 : x0 - x1;
    int sx = x0 < x1 ? 1 : -1;
    int dy = y1 - y0 <= 0 ? y1 - y0 : y0 - y1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true)
    {
        DrawPixel(x0, y0, colored);
        if (x0 == x1 && y0 == y1)
        {
            break;
        }
        int e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

/**
 * @brief this draws a horizontal line on the frame buffer
 *
 * @param x position x
 * @param y position y
 * @param line_width
 * @param colored
 */
void Paint::DrawHorizontalLine(int x, int y, int line_width, PaintColor colored)
{
    this->Changed = true;
    int i;
    int step = 1;
    if (line_width < 0)
    {
        step = -1;
    }
    for (i = x; i < x + line_width; i += step)
    {
        DrawPixel(i, y, colored);
    }
}

/**
 * @brief this draws a vertical line on the frame buffer
 *
 * @param x position x
 * @param y position y
 * @param line_height
 * @param colored
 */
void Paint::DrawVerticalLine(int x, int y, int line_height, PaintColor colored)
{
    this->Changed = true;
    int i;
    int step = 1;
    if (line_height < 0)
    {
        step = -1;
    }
    for (i = y; i < y + line_height; i += step)
    {
        DrawPixel(x, i, colored);
    }
}

/**
 * @brief this draws a rectangle
 *
 * @param x0 position x
 * @param y0 position y
 * @param x1 position x
 * @param y1 position y
 * @param colored
 */
void Paint::DrawRectangle(int x0, int y0, int x1, int y1, PaintColor colored)
{
    this->Changed = true;
    int min_x, min_y, max_x, max_y;
    min_x = x1 > x0 ? x0 : x1;
    max_x = x1 > x0 ? x1 : x0;
    min_y = y1 > y0 ? y0 : y1;
    max_y = y1 > y0 ? y1 : y0;

    DrawHorizontalLine(min_x, min_y, max_x - min_x + 1, colored);
    DrawHorizontalLine(min_x, max_y, max_x - min_x + 1, colored);
    DrawVerticalLine(min_x, min_y, max_y - min_y + 1, colored);
    DrawVerticalLine(max_x, min_y, max_y - min_y + 1, colored);
}

/**
 * @brief this draws a filled rectangle
 *
 * @param x0 position x
 * @param y0 position y
 * @param x1 position x
 * @param y1 position y
 * @param colored
 */
void Paint::DrawFilledRectangle(int x0, int y0, int x1, int y1, PaintColor colored)
{
    this->Changed = true;
    int min_x, min_y, max_x, max_y;
    int i;
    min_x = x1 > x0 ? x0 : x1;
    max_x = x1 > x0 ? x1 : x0;
    min_y = y1 > y0 ? y0 : y1;
    max_y = y1 > y0 ? y1 : y0;

    for (i = min_x; i <= max_x; i++)
    {
        DrawVerticalLine(i, min_y, max_y - min_y + 1, colored);
    }
}

/**
 * @brief this draws a circle
 *
 * @param x position x
 * @param y position y
 * @param radius radius of the circle
 * @param colored
 */
void Paint::DrawCircle(int x, int y, int radius, PaintColor colored)
{
    this->Changed = true;
    /* Bresenham algorithm */
    int x_pos = -radius;
    int y_pos = 0;
    int err = 2 - 2 * radius;
    int e2;

    do
    {
        DrawPixel(x - x_pos, y + y_pos, colored);
        DrawPixel(x + x_pos, y + y_pos, colored);
        DrawPixel(x + x_pos, y - y_pos, colored);
        DrawPixel(x - x_pos, y - y_pos, colored);
        e2 = err;
        if (e2 <= y_pos)
        {
            err += ++y_pos * 2 + 1;
            if (-x_pos == y_pos && e2 <= x_pos)
            {
                e2 = 0;
            }
        }
        if (e2 > x_pos)
        {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
}

/**
 * @brief this draws a filled circle
 *
 * @param x position x
 * @param y position y
 * @param radius radius of the circle
 * @param colored
 */
void Paint::DrawFilledCircle(int x, int y, int radius, PaintColor colored)
{
    this->Changed = true;
    /* Bresenham algorithm */
    int x_pos = -radius;
    int y_pos = 0;
    int err = 2 - 2 * radius;
    int e2;

    do
    {
        DrawPixel(x - x_pos, y + y_pos, colored);
        DrawPixel(x + x_pos, y + y_pos, colored);
        DrawPixel(x + x_pos, y - y_pos, colored);
        DrawPixel(x - x_pos, y - y_pos, colored);
        DrawHorizontalLine(x + x_pos, y + y_pos, 2 * (-x_pos) + 1, colored);
        DrawHorizontalLine(x + x_pos, y - y_pos, 2 * (-x_pos) + 1, colored);
        e2 = err;
        if (e2 <= y_pos)
        {
            err += ++y_pos * 2 + 1;
            if (-x_pos == y_pos && e2 <= x_pos)
            {
                e2 = 0;
            }
        }
        if (e2 > x_pos)
        {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
}

/**
 * @brief this draws a circle segment
 *
 * @param x position x
 * @param y position y
 * @param radius radius of the circle
 * @param startAngle  in degrees
 * @param endAngle in degrees
 * @param colored color or white
 */
void Paint::DrawCircleSegment(int x, int y, int radius, int startAngle, int endAngle, PaintColor colored)
{
    float circumference = 2 * PI * radius;

    if (startAngle > endAngle)
    {
        int h = startAngle;
        startAngle = endAngle;
        endAngle = h;
    }

    float circumferenceSection = circumference * (endAngle - startAngle) / 360;
    float step = 1 / circumferenceSection;

    for (float angle = startAngle; angle <= endAngle; angle += step)
    {
        float rad = angle * DEG_TO_RAD;
        float posX = (float)x + cosf(rad) * (float)radius;
        float posY = (float)y + sinf(rad) * (float)radius;
        DrawPixel((int)roundf(posX), (int)roundf(posY), colored);
    }
}

/**
 * @brief this draws a filled circle segment
 *
 * @param x position x
 * @param y position y
 * @param startRadius start radius of the circle
 * @param endRadius end radius of the circle
 * @param startAngle in degrees
 * @param endAngle in degrees
 * @param colored color or white
 */
void Paint::DrawFilledCircleSegment(int x, int y, int startRadius, int endRadius, int startAngle, int endAngle, PaintColor colored)
{
    float step = 1; //.0 / (endRadius - startRadius);
    if (startRadius > endRadius)
    {
        int h = startRadius;
        startRadius = endRadius;
        endRadius = h;
    }
    for (float radius = startRadius; radius >= startRadius && radius <= endRadius; radius += step)
    {
        DrawCircleSegment(x, y, radius, startAngle, endAngle, colored);
    }
}

/**
 * @brief this draws a filled polygone
 * 
 * @param xs array of x positions
 * @param ys array of y positions
 * @param length count of positions
 * @param colored color or not
 */
void Paint::DrawFilledPolygone(int xs[], int ys[], int length, PaintColor colored)
{

    float ms[length] = {};
    float bs[length] = {};
    float crossingX[length] = {};
    int minX = xs[0];
    int minY = ys[0];
    int maxX = xs[0];
    int maxY = ys[0];
    int nextA = 0;

    // Preparing
    for (int a = 0; a < length; a++)
    {
        if (xs[a] < minX)
        {
            minX = xs[a];
        }
        else if (xs[a] > maxX)
        {
            maxX = xs[a];
        }
        if (ys[a] < minY)
        {
            minY = ys[a];
        }
        else if (ys[a] > maxY)
        {
            maxY = ys[a];
        }

        nextA = a + 1;
        if (nextA == length)
        {
            nextA = 0;
        }
        float devisor = (xs[nextA] - xs[a]);
        if (devisor != 0)
        {
            ms[a] = (ys[nextA] - ys[a]) / devisor;
            if (ms[a] == 0)
            {
                ms[a] = 0.00001;
            }
            bs[a] = ys[a] - ms[a] * xs[a];
        }
        else
        {
            ms[a] = 100000;
            if (ys[a] > ys[nextA])
            {
                ms[a] = ms[a] * -1;
            }
        }
        bs[a] = ys[a] - ms[a] * xs[a];

    }

    for (int pixelY = minY; pixelY <= maxY; pixelY++)
    {

        for (int a = 0; a < length; a++)
        {
            crossingX[a] = round(-(bs[a] - pixelY) / ms[a]);

        }
        // sort in ascending
        int temp, i, j, k;
        for (j = 0; j < length; ++j)
        {
            for (k = j + 1; k < length; ++k)
            {
                if (crossingX[j] > crossingX[k])
                {
                    temp = crossingX[j];
                    crossingX[j] = crossingX[k];
                    crossingX[k] = temp;
                }
            }
        }

        for (i = 1; i < length; i++)
        {
            // everything is gerater than image
            if (crossingX[i - 1] >= maxX)
            {
                break;
            }
            if (crossingX[i - 1] < minX) {
                continue;
            }

            if (crossingX[i] >= minX && crossingX[i] <= maxX )
            {
                for (int pixelX = crossingX[i - 1]; pixelX <= crossingX[i]; pixelX++)
                {
                    DrawPixel(pixelX, pixelY, colored);
                }
            }
        }
    }
}

/**
 * @brief check if buffer was changed
 *
 * @return true
 * @return false
 */
bool Paint::hasChanged()
{
    return this->Changed;
}

/**
 * @brief reset the changed flag
 *
 */
void Paint::resetChanged()
{
    this->Changed = false;
}

/* END OF FILE */
