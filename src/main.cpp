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
#include "EpaperContent.h"

#include "sensorBattery.h"

// Wie lange nach dem Aufwachen der lokale Webserver (/api/display/image, fuer
// manuelles Debugging) erreichbar bleibt, bevor wieder in den Deep Sleep geht.
#define EpaperActiveWindowSeconds 10
// Zykluszeit zwischen zwei Display-Refreshes/HA-Abfragen.
#define EpaperWakeIntervalSeconds (10 * 60)

int activeSeconds = 0;

void startup()
{
  // Reboot
  delay(10000);
  Debug("Start", "Boot");
  TimeSyncStartup();
  TransientStartup();

  PowerStartup();
  SensorBatteryStartup();
  SensorBatteryReadState();

  DisplayStartup();

#ifdef TestWebServer
  connectWifi();

  OtaCheckAndUpdate(State.Battery.Percent);
  EpaperContentUpdate(State.Battery.Percent, State.Battery.Voltage);

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
  SensorBatteryReadState();

  DisplayWakeup();

#ifdef TestWebServer
  connectWifi();

  EpaperContentUpdate(State.Battery.Percent, State.Battery.Voltage);

  server.begin();
#endif
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
  StartDeepSleep((uint64_t)EpaperWakeIntervalSeconds * 1000000ull);
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
  activeSeconds++;
  if (activeSeconds >= EpaperActiveWindowSeconds)
  {
    shutdown();
  }
}
