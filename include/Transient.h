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

// TransientData is storred in rtc ram and is deleted when powered off. Checksum is used to validate ram data.

#ifndef TransientData_h
#define TransientData_h
#include <Arduino.h>
#pragma region Battery
typedef struct TransientBattery {
    float LastPercent;
} TransientBattery;
#pragma endregion

#pragma region Time

typedef struct TransientTime {
    time_t LastSync;
} TransientTime;

#pragma endregion

// Max 8KB of Data
typedef struct __attribute__((packed)) TransientData
{
    TransientBattery Battery;
    TransientTime Time;
    uint64_t Checksum;
} TransientData;

RTC_DATA_ATTR TransientData DeepSleepState = {};
TransientData Transient = {};

unsigned int CalculateStateChecksum(const TransientData *state)
{
    TransientData copy;
    memcpy(&copy, state, sizeof(copy));
    copy.Checksum = 0;

    unsigned int sum = 0;
    const unsigned char *p = (const unsigned char *)&copy;
    for (size_t i = 0; i < sizeof(copy); i++)
    {
        sum += p[i];
    }

    return sum;
}

void ResetTransientData(TransientData *state)
{
    memset(state, 0, sizeof(*state));
}

void LoadTransientData(TransientData *state)
{
    if (CalculateStateChecksum(&DeepSleepState) == DeepSleepState.Checksum)
    {
        // Data is Valid
        memcpy(state, &DeepSleepState, sizeof(DeepSleepState));
    }
    else
    {
        // Data is Invalid
        ResetTransientData(state);
    }
}

void SaveTransientData(const TransientData *state)
{
    memcpy(&DeepSleepState, state, sizeof(DeepSleepState));
    DeepSleepState.Checksum = CalculateStateChecksum(&DeepSleepState);
}


void TransientStartup() {

}

void TransientWakeup() {
    LoadTransientData(&Transient);
}

void TransientUpdate() {

}

void TransientShutdown() {
    SaveTransientData(&Transient);
}

#endif
