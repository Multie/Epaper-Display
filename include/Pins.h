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

// Configuration is hard programmed data

#ifndef Pins_h
#define Pins_h
#include <Arduino.h>

/*
#  Pin Left # Pin Right #
#########################
Gnd---------- GND       #
Vin --------- Vin       #
3v3 --------- 3v3       #
EN          # IO0       #
IO21        # IO2       #
IO22        # IO4       #
IO23        # IO5       #
Vbat        # IO12      #
IO27        # IO13      #
IO26        # IO14      #
IO25        # IO15      #
3v3 --------- 3v3       #
IO32        # IO16      #
IO33        # IO17      #
IO34        # IO18      #
IO35        # IO19      #
Vin --------- Vin       #
GND --------- GND       #
IOVp        # Rx        #
IOVn        # Tx        #
    Antenna
*/

// Pin Mapping
// Right
#define IO0 0 //
// #define IO3 // TX
#define IO2 2 //
// #define IO4 // RX
#define IO4 4   //
#define IO5 5   //
#define IO12 12 //
#define IO13 13 //
#define IO14 14 //
#define IO15 15 //
#define IO16 16 //
#define IO17 17 //
#define IO18 18 //
#define IO19 19 //
// Left
#define IO21 21 //
#define IO22 22 //
#define IO23 23 //
#define IO27 27 //
#define IO25 25 // DAC1
#define IO26 26 // DAC0
#define IO32 32 //
#define IO33 33 //
#define IO34 34 //
#define IO35 35 //
                //
#define IOVp 36 //
#define IOVn 39 //

// PIN Configuration
// Power
#define PowerPin IO5
// Battery
#define BatteryLowAlarmPin IO19
// Buttons
#define Button1Pin IOVp
#define Button2Pin IOVn
#define Button3Pin IO35
#define Button4Pin IO34
#define Button5Pin IO33
#define Button6Pin IO25
#define Button7Pin IO13
// SPI
#define SpiMosiPin IO23
// #define SpiMisoPin IO19
#define SpiClkPin IO18
// Display
#define DisplayCsPin IO4
#define DisplayDcPin IO27
#define DisplayRstPin IO12
#define DisplayBusyPin IO17
// Slider
#define SliderPin IO32
// I2C
#define I2cSclPin IO22
#define I2cSdaPin IO21
// Gesturecontrol
#define GesturecontrolInterruptPin IO26
// Beeper
#define BeeperPin IO2

void setupPins()
{
    pinMode(PowerPin, OUTPUT);
    digitalWrite(PowerPin, LOW);

    pinMode(Button1Pin, INPUT);
    pinMode(Button2Pin, INPUT);
    pinMode(Button3Pin, INPUT);
    pinMode(Button4Pin, INPUT);
    pinMode(Button5Pin, INPUT);
    pinMode(Button6Pin, INPUT);
    pinMode(Button7Pin, INPUT);

    pinMode(SpiMosiPin, OUTPUT);
    digitalWrite(SpiMosiPin, LOW);
    // pinMode(SpiMisoPin, INPUT);
    pinMode(SpiClkPin, OUTPUT);
    digitalWrite(SpiClkPin, LOW);

    pinMode(DisplayCsPin, OUTPUT);
    digitalWrite(DisplayCsPin, LOW);
    pinMode(DisplayDcPin, OUTPUT);
    digitalWrite(DisplayDcPin, LOW);
    pinMode(DisplayRstPin, OUTPUT);
    digitalWrite(DisplayRstPin, LOW);
    pinMode(DisplayBusyPin, INPUT);

    pinMode(I2cSclPin, INPUT);
    pinMode(I2cSdaPin, INPUT);

    pinMode(GesturecontrolInterruptPin, INPUT);

    pinMode(BeeperPin, OUTPUT);
    digitalWrite(BeeperPin, LOW);
}

#endif