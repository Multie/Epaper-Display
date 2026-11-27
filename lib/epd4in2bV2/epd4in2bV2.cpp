/**
 *  @filename   :   epd4in2b_V2.cpp
 *  @brief      :   Implements for Dual-color e-paper library
 *  @author     :	Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     Nov 25 2020
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

#include "epd4in2bV2.h"

Epd::~Epd(){};

Epd::Epd(unsigned int resetPin, unsigned int dcPin, unsigned int csPin, unsigned int busyPin, unsigned int Width, unsigned int Height)
{
    reset_pin = resetPin;
    dc_pin = dcPin;
    cs_pin = csPin;
    busy_pin = busyPin;
    width = Width;
    height = Height;
    pinMode(cs_pin, OUTPUT);
    pinMode(reset_pin, OUTPUT);
    pinMode(dc_pin, OUTPUT);
    pinMode(busy_pin, INPUT);
};

int Epd::Init(void)
{
    /* this calls the peripheral hardware interface, see epdif */

    IfInit();

    /* EPD hardware init start */
    TimedOut = false;
    Reset();

    SendCommand(POWER_ON);
    WaitUntilIdle();
    if (TimedOut)
    {
        return 1;
    }

    SendCommand(PANEL_SETTING);
    SendData(0x0F); // LUT from OTP

    /* EPD hardware init end */
    return 0;
}


int Epd::Wakeup(void) {
    TimedOut = false;
    Reset();
    SendCommand(POWER_ON);
    WaitUntilIdle();
    if (TimedOut)
    {
        return 1;
    }

    SendCommand(PANEL_SETTING);
    SendData(0x0F); // LUT from OTP
    return 0;
}

/**
 *  @brief: basic function for sending commands
 */
void Epd::SendCommand(unsigned char command)
{
    DigitalWrite(dc_pin, LOW);
    SpiTransfer(cs_pin, command);
}

/**
 *  @brief: basic function for sending data
 */
void Epd::SendData(unsigned char data)
{
    DigitalWrite(dc_pin, HIGH);
    SpiTransfer(cs_pin, data);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH, gives up after a timeout
 */
void Epd::WaitUntilIdle(void)
{
    // Ein Refresh dieses 3-Farb-Panels dauert regulaer deutlich unter 60 s
    const unsigned long timeoutMs = 60000;
    // Dem Controller Zeit geben, Busy ueberhaupt erst zu setzen
    DelayMs(100);
    unsigned long start = millis();
    while (DigitalRead(busy_pin) == 0)
    { // 0: busy, 1: idle
        if (TimedOut || millis() - start > timeoutMs)
        {
            TimedOut = true;
            return;
        }
        DelayMs(20);
    }
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void Epd::Reset(void)
{
    DigitalWrite(reset_pin, HIGH);
    DelayMs(200);
    DigitalWrite(reset_pin, LOW);
    DelayMs(2);
    DigitalWrite(reset_pin, HIGH);
    DelayMs(200);
}

/**
 *  @brief: transmit partial data to the SRAM
 */
void Epd::SetPartialWindow(const unsigned char *buffer_black, const unsigned char *buffer_red, int x, int y, int w, int l)
{
    SendCommand(PARTIAL_IN);
    SendCommand(PARTIAL_WINDOW);
    SendData(x >> 8);
    SendData(x & 0xf8); // x should be the multiple of 8, the last 3 bit will always be ignored
    SendData(((x & 0xf8) + w - 1) >> 8);
    SendData(((x & 0xf8) + w - 1) | 0x07);
    SendData(y >> 8);
    SendData(y & 0xff);
    SendData((y + l - 1) >> 8);
    SendData((y + l - 1) & 0xff);
    SendData(0x01); // Gates scan both inside and outside of the partial window. (default)
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_1);
    if (buffer_black != NULL)
    {
        for (int i = 0; i < w / 8 * l; i++)
        {
            SendData(buffer_black[i]);
        }
    }
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_2);
    if (buffer_red != NULL)
    {
        for (int i = 0; i < w / 8 * l; i++)
        {
            SendData(buffer_red[i]);
        }
    }
    DelayMs(2);
    SendCommand(PARTIAL_OUT);
}

/**
 *  @brief: transmit partial data to the black part of SRAM
 */
void Epd::SetPartialWindowBlack(const unsigned char *buffer_black, int x, int y, int w, int l)
{
    SendCommand(PARTIAL_IN);
    SendCommand(PARTIAL_WINDOW);
    SendData(x >> 8);
    SendData(x & 0xf8); // x should be the multiple of 8, the last 3 bit will always be ignored
    SendData(((x & 0xf8) + w - 1) >> 8);
    SendData(((x & 0xf8) + w - 1) | 0x07);
    SendData(y >> 8);
    SendData(y & 0xff);
    SendData((y + l - 1) >> 8);
    SendData((y + l - 1) & 0xff);
    SendData(0x01); // Gates scan both inside and outside of the partial window. (default)
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_1);
    if (buffer_black != NULL)
    {
        for (int i = 0; i < w / 8 * l; i++)
        {
            SendData(buffer_black[i]);
        }
    }
    DelayMs(2);
    SendCommand(PARTIAL_OUT);
}

/**
 *  @brief: transmit partial data to the red part of SRAM
 */
void Epd::SetPartialWindowRed(const unsigned char *buffer_red, int x, int y, int w, int l)
{
    SendCommand(PARTIAL_IN);
    SendCommand(PARTIAL_WINDOW);
    SendData(x >> 8);
    SendData(x & 0xf8); // x should be the multiple of 8, the last 3 bit will always be ignored
    SendData(((x & 0xf8) + w - 1) >> 8);
    SendData(((x & 0xf8) + w - 1) | 0x07);
    SendData(y >> 8);
    SendData(y & 0xff);
    SendData((y + l - 1) >> 8);
    SendData((y + l - 1) & 0xff);
    SendData(0x01); // Gates scan both inside and outside of the partial window. (default)
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_2);
    if (buffer_red != NULL)
    {
        for (int i = 0; i < w / 8 * l; i++)
        {
            SendData(buffer_red[i]);
        }
    }
    DelayMs(2);
    SendCommand(PARTIAL_OUT);
}

/**
 * @brief: refresh and displays the frame
 */
void Epd::DisplayFrame(const unsigned char *frame_black, const unsigned char *frame_red)
{
    if (frame_black != NULL)
    {
        SendCommand(DATA_START_TRANSMISSION_1);
        DelayMs(2);
        for (int i = 0; i < this->width / 8 * this->height; i++)
        {
            SendData(pgm_read_byte(&frame_black[i]));
        }
        DelayMs(2);
    }
    if (frame_red != NULL)
    {
        SendCommand(DATA_START_TRANSMISSION_2);
        DelayMs(2);
        for (int i = 0; i < this->width / 8 * this->height; i++)
        {
            SendData(pgm_read_byte(&frame_red[i]));
        }
        DelayMs(2);
    }
    SendCommand(DISPLAY_REFRESH);
    WaitUntilIdle();
}

/**
 * @brief: clear the frame data from the SRAM, this won't refresh the display
 */
void Epd::ClearFrame(void)
{
    SendCommand(DATA_START_TRANSMISSION_1);
    DelayMs(2);
    for (int i = 0; i < width / 8 * height; i++)
    {
        SendData(0xFF);
    }
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_2);
    DelayMs(2);
    for (int i = 0; i < width / 8 * height; i++)
    {
        SendData(0xFF);
    }
    DelayMs(2);
}

/**
 * @brief: This displays the frame data from SRAM
 */
void Epd::DisplayFrame(void)
{
    SendCommand(DISPLAY_REFRESH);
    DelayMs(100);
    WaitUntilIdle();
}

/**
 * @brief: After this command is transmitted, the chip would enter the deep-sleep mode to save power.
 *         The deep sleep mode would return to standby by hardware reset. The only one parameter is a
 *         check code, the command would be executed if check code = 0xA5.
 *         You can use Epd::Reset() to awaken and use Epd::Init() to initialize.
 */
void Epd::Sleep()
{
    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendData(0xF7); // border floating
    SendCommand(POWER_OFF);
    WaitUntilIdle();
    SendCommand(DEEP_SLEEP);
    SendData(0xA5); // check code
}

/* END OF FILE */
