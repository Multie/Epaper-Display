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

#ifndef sensorGesture_h
#define sensorGesture_h

#include <Arduino.h>
#include "Pins.h"
#include "Debug.h"
#include "Settings.h"
#include "Transient.h"
#include "State.h"

#include "Wire.h"
#include <SparkFun_APDS9960.h>
#define APDS9960_INT GesturecontrolInterruptPin

volatile bool GestureInterrupt;

SparkFun_APDS9960 apds = SparkFun_APDS9960();

void IRAM_ATTR SensorGestureInterrupt()
{
    GestureInterrupt = true;
}

void SensorGestureReadState()
{

    State.Gesture.Up = false;
    State.Gesture.Down = false;
    State.Gesture.Left = false;
    State.Gesture.Right = false;

    if (GestureInterrupt)
    {
        detachInterrupt(GesturecontrolInterruptPin);
        if (apds.isGestureAvailable())
        {
            GestureInterrupt = false;
            int gesture = apds.readGesture();
            switch (gesture)
            {
            case DIR_UP:
                State.Gesture.Up = true;
                break;
            case DIR_DOWN:
                State.Gesture.Down = true;
                break;
            case DIR_LEFT:
                State.Gesture.Left = true;
                break;
            case DIR_RIGHT:
                State.Gesture.Right = true;
                break;
            /*case DIR_NEAR:
                state->Near = true;
                break;
            case DIR_FAR:
                state->Far = true;
                break;*/
            default:

                break;
            }
        }
        attachInterrupt(GesturecontrolInterruptPin, SensorGestureInterrupt, FALLING);
    }
}

void SensorGestureStartup()
{
    attachInterrupt(GesturecontrolInterruptPin, SensorGestureInterrupt, FALLING);
    apds.init();
    apds.enableGestureSensor(true);
}

void SensorGestureWakeup()
{
    // GestureInterrupt = !digitalRead(GesturecontrolInterruptPin);
    attachInterrupt(GesturecontrolInterruptPin, SensorGestureInterrupt, FALLING);
    Wire.begin();
}

void SensorGestureUpdate()
{
    Debug("GesturecontrolInterruptPin", digitalRead(GesturecontrolInterruptPin));
    Debug("GestureInterrupt", GestureInterrupt);
    // SensorGestureReadState();
}

void SensorGestureShutdown()
{
    detachInterrupt(GesturecontrolInterruptPin);
    apds.disableGestureSensor();
    apds.disableLightSensor();
    apds.disableProximitySensor();
    apds.disablePower();
     
}

#endif