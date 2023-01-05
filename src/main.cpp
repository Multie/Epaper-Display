#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>

#define NOW_ESP32_RTC
#define NOW_MILLIS
#include <UniversalTime.h>
UniversalTime UniversalTime::UniversalTimeNow;

#include "time.h"
#include "Debug.h"

#include "Pins.h"
#include "Transient.h"
#include "Settings.h"

#include "Power.h"
#include "TimeSync.h"
#include "Display.h"
#include "Beeper.h"
#include "Sleep.h"



#include "sensorButtons.h"
#include "sensorSlider.h"
#include "sensorBattery.h"
#include "sensorGesture.h"



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
    // Reboot
    delay(10000);
    Debug("Start", "Boot");
    TimeSyncStartup();
    TransientStartup();

    PowerStartup();
    SensorButtonsStartup();
    SensorSliderStartup();
    SensorBatteryStartup();
    SensorGestureStartup();

    
    BeeperStartup();
    DisplayStartup();
  }
  else
  {
    Debug("Start", "Wakeup");
    TimeSyncWakeup();
    TransientWakeup();


    PowerWakeup();
    SensorButtonsWakeup();
    SensorSliderWakeup();
    SensorBatteryWakeup();
    SensorGestureWakeup();

    
    BeeperWakeup();
    DisplayWakeup();
  }

  time_t now;
  
  struct tm timeinfo;

  time(&now);

  localtime_r(&now, &timeinfo);
  char strftime_buf[64];
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    Serial.println(strftime_buf);
  // ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);



  //*/
  Debug("Setup", "End");
  loopDebug();
  Debug("Startup", "");

  loopDebug();
}

void loop()
{
  PowerUpdate();
  TransientUpdate();
  // Input
  SensorButtonsUpdate();
  SensorSliderUpdate();
  SensorBatteryUpdate();
  SensorGestureUpdate();

  TimeSyncUpdate();
  // Output
  BeeperUpdate();
  DisplayUpdate();

  Debug("Now",UniversalTime::Now().toISO8601());
  loopDebug();
  delay(1000);

  // shutdown();
}

void shutdown()
{
  Debug("Shutdown", "Start");
  SensorGestureShutdown();
  SensorBatteryShutdown();
  SensorSliderShutdown();
  SensorButtonsShutdown();
  
  TimeSyncShutdown();
  BeeperShutdown();
  DisplayShutdown();
  PowerShutdown();

  
  TransientShutdown();
  Debug("Shutdown", "Active");
  setupSleepWakeup();
  StartDeepSleep(10 * 1e6);
}