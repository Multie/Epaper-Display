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

#ifndef Beeper_h
#define Beeper_h

#include <Arduino.h>
#include "Pins.h"
#include "Debug.h"
#include "Settings.h"
#include "Transient.h"
#include "State.h"

#pragma region Notes
#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978
#pragma endregion

#pragma region Main Api
#define BeeperChannel 0
#define BeeperFrequency 2000
#define BeeperResolution 8
typedef struct BeeperNote
{
    note_t note;
    int octave;
    uint16_t duration; // ms
} BeeperNote;

#define BeeperSongAcceptLength 7
BeeperNote BeeperSongAccept[BeeperSongAcceptLength] = {
    {NOTE_C, 3, 4000},
    {NOTE_D, 3, 4000},
    {NOTE_E, 3, 4000},
    {NOTE_F, 3, 4000},
    {NOTE_G, 3, 4000},
    {NOTE_A, 3, 4000},
    {NOTE_B, 3, 8000},
};

void BeeperPlaySong(BeeperNote *song, int length)
{
    for (int a = 0; a < length; a++)
    {
        if (song[a].note != NOTE_MAX)
        {
            Debug("Note:",song[a].note);
            Debug("Octave:",song[a].octave);
            Debug("Octave:",song[a].duration);
            ledcWriteNote(BeeperChannel, song[a].note, song[a].octave);
            delay(song[a].duration);
        }
        else
        {
            Debug("Note:","Sleep");
            Debug("Octave:",song[a].duration);
            ledcWrite(BeeperChannel, 0);
            delay(song[a].duration);
        }
    }
    ledcWrite(BeeperChannel, 0);
    
}

void BeeperStartup()
{
    ledcSetup(BeeperChannel, BeeperFrequency, BeeperResolution);
    ledcAttachPin(BeeperPin, BeeperChannel);
}

void BeeperWakeup()
{
    ledcSetup(BeeperChannel, BeeperFrequency, BeeperResolution);
    ledcAttachPin(BeeperPin, BeeperChannel);
}

void BeeperUpdate()
{
    if (State.Beeper.Play)
    {
        switch (State.Beeper.Song)
        {
        case BeeperSongs::None:
            State.Beeper.Play = false;
            break;
        case BeeperSongs::Accept:
            State.Beeper.Play = false;
            BeeperPlaySong(BeeperSongAccept,BeeperSongAcceptLength);

            break;
        case BeeperSongs::NotAccept:
            State.Beeper.Play = false;
            break;
        default:
            break;
        }
    }
}

void BeeperShutdown()
{
    ledcDetachPin(BeeperChannel);
}
#pragma endregion

#endif