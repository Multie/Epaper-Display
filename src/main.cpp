#include <Arduino.h>
#include <SPI.h>
#include <UniversalTime.h>
UniversalTime UniversalTime::UniversalTimeNow;
#include "time.h"
#include "Debug.h"

#include "Pins.h"
#include "Transient.h"
#include "Settings.h"

#include "Power.h"
#include "sensorButtons.h"
#include "sensorSlider.h"

#include "sensorBattery.h"
#include "sensorGesture.h"

// #include "BatteryMonitoring.h"
#include "Sleep.h"


void shutdown();
void setup()
{
  setupPins();

  Serial.begin(112500);

  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_UNDEFINED)
  {
    // Reboot
    delay(10000);
    Debug("Start", "Boot");
    PowerStartup();
    SensorButtonsStartup();
    SensorSliderStartup();
    SensorBatteryStartup();
    SensorGestureStartup();
  }
  else
  {
    Debug("Start", "Wakeup");
    PowerWakeup();
    SensorButtonsWakeup();
    SensorSliderWakeup();
    SensorBatteryWakeup();
    SensorGestureWakeup();
  }

  time_t now;
  char strftime_buf[64];
  struct tm timeinfo;

  time(&now);

  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
  // ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);
  Serial.println(strftime_buf);

  //*/
  Debug("Setup", "End");
  loopDebug();
  Debug("Startup", "");
 
  loopDebug();
}

void loop()
{
  PowerUpdate();
  SensorButtonsUpdate();
  SensorSliderUpdate();
  SensorBatteryUpdate();
  SensorGestureUpdate();


  loopDebug();
  delay(1000);

  //shutdown();
}

void shutdown()
{
  Debug("Shutdown", "Start");
  SensorGestureShutdown();
  SensorBatteryShutdown();
  SensorSliderShutdown();
  SensorButtonsShutdown();
  PowerShutdown();

  Debug("Shutdown", "Active");
  setupSleepWakeup();
  StartDeepSleep(10 * 1e6);
}