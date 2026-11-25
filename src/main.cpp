// Libraries
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include "time.h"
// UniversalTime
#define NOW_ESP32_RTC
#define NOW_MILLIS
#include <UniversalTime.h>
UniversalTime UniversalTime::UniversalTimeNow;

#define TestWebServer
#ifdef TestWebServer
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);
#endif

// Includes
#include "Debug.h"

#include "Pins.h"
#include "Transient.h"
#include "Settings.h"

#include "Power.h"
#include "TimeSync.h"
#include "Display.h"
#include "Sleep.h"
#include "OtaUpdate.h"

#include "sensorBattery.h"

void startup()
{
  // Reboot
  delay(10000);
  Debug("Start", "Boot");
  TimeSyncStartup();
  TransientStartup();

  PowerStartup();
  SensorBatteryStartup();

  DisplayStartup();

#ifdef TestWebServer
  connectWifi();

  OtaCheckAndUpdate();

  server.begin();
#endif
}

void wakeup()
{
  Debug("Start", "Wakeup");
  TimeSyncWakeup();
  TransientWakeup();

  PowerWakeup();
  SensorBatteryWakeup();

  DisplayWakeup();
}

void update()
{
  PowerUpdate();
  TransientUpdate();
  // Input
  SensorBatteryUpdate();

  TimeSyncUpdate();
  // Output
  DisplayUpdate();

 // Debug("Now", UniversalTime::Now().toISO8601());
  loopDebug();
  delay(1000);
}

void shutdown()
{
  Debug("Shutdown", "Start");
  SensorBatteryShutdown();

  TimeSyncShutdown();
  DisplayShutdown();
  PowerShutdown();

  TransientShutdown();
  Debug("Shutdown", "Active");
  StartDeepSleep(10 * 1e6);
}

void shutdown();
void setup()
{
  setupPins();
  Serial.begin(112500);
  setSettings();
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_UNDEFINED)
  {
    startup();
  }
  else
  {
    wakeup();
  }
  loopDebug();
}

void loop()
{
  update();
  // shutdown();
}
