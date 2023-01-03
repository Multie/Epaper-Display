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

#ifndef deviceBattery_h
#define deviceBattery_h

#include <Arduino.h>
#include "Pins.h"
#include "Debug.h"
#include "Settings.h"
#include "Transient.h"
#include "State.h"

#include "LC709203F.h"
#include "Wire.h"

LC709203F deviceBattery;

void DeviceBatteryReadState()
{
    State.Battery.Alarm = digitalRead(BatteryLowAlarmPin);
    State.Battery.Percent = deviceBattery.cellStateOfCharge() / 100.0;
    State.Battery.Voltage = deviceBattery.cellVoltage_mV() / 1000.0;
}

void DeviceBatteryStartup()
{
    deviceBattery.begin();
    deviceBattery.setCellCapacity(LC709203F_APA_1000MAH);
    deviceBattery.setCellProfile(LC709203_NOM3p7_Charge4p2);
    deviceBattery.setAlarmVoltage(3.4);
}

void DeviceBatteryWakeup()
{
    Wire.begin();
}

void DeviceBatteryUpdate()
{
    DeviceBatteryReadState();
}

void DeviceBatteryShutdown()
{
}

#endif