#include <Arduino.h>
#include <SPI.h>
#include <UniversalTime.h>
UniversalTime UniversalTime::UniversalTimeNow;
#include "time.h"
#include "Debug.h"

#include "Pins.h"
#include "Transient.h"
#include "Settings.h"

#include "deviceButtons.h"
#include "deviceSlider.h"

#include "deviceBattery.h"
#include "deviceGesture.h"

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
    DeviceButtonsStartup();
    DeviceSliderStartup();
    DeviceBatteryStartup();
    DeviceGestureStartup();
  }
  else
  {
    Debug("Start", "Wakeup");
    DeviceButtonsWakeup();
    DeviceSliderWakeup();
    DeviceBatteryWakeup();
    DeviceGestureWakeup();
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
  DeviceButtonsUpdate();
  DeviceSliderUpdate();
  DeviceBatteryUpdate();
  DeviceGestureUpdate();


  loopDebug();
  delay(1000);

  //shutdown();
}

void shutdown()
{
  Debug("Shutdown", "Start");
  DeviceGestureShutdown();
  DeviceBatteryShutdown();
  DeviceSliderShutdown();
  DeviceButtonsShutdown();

  Debug("Shutdown", "Active");
  setupSleepWakeup();
  StartDeepSleep(10 * 1e6);
}