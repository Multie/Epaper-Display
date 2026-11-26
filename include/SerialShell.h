/*
 *   Copyright (c) 2026 Malte Hering
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

// Serielle Setup-Shell: WLAN + Webhook-URL landen im NVS (siehe Settings.h)
// statt in einer Secrets.h-Datei. Nur bei USB-Kaltstart erreichbar, nie nach
// Deep-Sleep-Wakeup.

#ifndef SerialShell_h
#define SerialShell_h

#include <Arduino.h>
#include "Settings.h"

void SerialShellPrintHelp()
{
    Serial.println("Setup-Shell - Befehle:");
    Serial.println("  wifi ssid <wert>");
    Serial.println("  wifi pass <wert>");
    Serial.println("  url <wert>          (volle Webhook-URL der HA-Integration)");
    Serial.println("  show                (aktuelle Werte anzeigen)");
    Serial.println("  save                (in NVS schreiben)");
    Serial.println("  reboot");
}

void SerialShellShow()
{
    Serial.print("Ssid: ");
    Serial.println(Settings.Wifi.Ssid);
    Serial.print("Pass: ");
    Serial.println(strlen(Settings.Wifi.Password) > 0 ? "*** (gesetzt)" : "(leer)");
    Serial.print("Url:  ");
    Serial.println(Settings.Url);
}

// value darf nicht laenger als fieldSize-1 sein, sonst Fehlermeldung statt
// stillem Abschneiden/Overflow.
bool SerialShellSetField(char *field, size_t fieldSize, const String &value)
{
    if (value.length() > fieldSize - 1)
    {
        Serial.println("Fehler: Wert zu lang");
        return false;
    }
    strncpy(field, value.c_str(), fieldSize - 1);
    field[fieldSize - 1] = 0;
    return true;
}

void SerialShellHandleLine(const String &line)
{
    int firstSpace = line.indexOf(' ');
    String command = (firstSpace < 0) ? line : line.substring(0, firstSpace);
    String rest = (firstSpace < 0) ? "" : line.substring(firstSpace + 1);
    command.trim();
    rest.trim();

    if (command == "wifi")
    {
        int secondSpace = rest.indexOf(' ');
        if (secondSpace < 0)
        {
            Serial.println("Nutzung: wifi ssid|pass <wert>");
            return;
        }
        String sub = rest.substring(0, secondSpace);
        String value = rest.substring(secondSpace + 1);
        if (sub == "ssid")
        {
            if (SerialShellSetField(Settings.Wifi.Ssid, sizeof(Settings.Wifi.Ssid), value))
            {
                Serial.println("Ok");
            }
        }
        else if (sub == "pass")
        {
            if (SerialShellSetField(Settings.Wifi.Password, sizeof(Settings.Wifi.Password), value))
            {
                Serial.println("Ok");
            }
        }
        else
        {
            Serial.println("Nutzung: wifi ssid|pass <wert>");
        }
    }
    else if (command == "url")
    {
        if (rest.length() == 0)
        {
            Serial.println("Nutzung: url <wert>");
            return;
        }
        if (SerialShellSetField(Settings.Url, sizeof(Settings.Url), rest))
        {
            Serial.println("Ok");
        }
    }
    else if (command == "show")
    {
        SerialShellShow();
    }
    else if (command == "save")
    {
        saveSettings();
        Serial.println("Gespeichert");
    }
    else if (command == "reboot")
    {
        Serial.println("Neustart...");
        delay(100);
        ESP.restart();
    }
    else if (command.length() > 0)
    {
        Serial.println("Unbekannter Befehl, siehe 'help'");
        SerialShellPrintHelp();
    }
}

void SerialShellRun()
{
    SerialShellPrintHelp();
    SerialShellShow();
    String line;
    while (true)
    {
        if (Serial.available())
        {
            char c = Serial.read();
            if (c == '\n')
            {
                line.trim();
                if (line.length() > 0)
                {
                    SerialShellHandleLine(line);
                }
                line = "";
            }
            else if (c != '\r')
            {
                line += c;
            }
        }
        delay(10);
    }
}

// Oeffnet die Shell sofort, wenn noch keine gueltigen Settings gespeichert
// sind (hasSettings == false) - ein WLAN-Connect-Versuch waere eh sinnlos.
// Sonst ein kurzes Zeitfenster, in dem ein Tastendruck die Shell oeffnet.
void SerialShellOfferAtBoot(bool hasSettings)
{
    if (!hasSettings)
    {
        Serial.println("Keine gueltigen Settings im NVS - Setup-Shell.");
        SerialShellRun();
        return;
    }

    Serial.println("Taste druecken fuer Setup-Shell (3s)...");
    unsigned long start = millis();
    while (millis() - start < 3000)
    {
        if (Serial.available())
        {
            while (Serial.available())
            {
                Serial.read();
            }
            SerialShellRun();
            return;
        }
        delay(50);
    }
}

#endif
