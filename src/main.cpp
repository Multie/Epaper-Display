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
#include "PowerSource.h"
#include "Transient.h"
#include "Settings.h"

#include "Power.h"
#include "TimeSync.h"
#include "Display.h"
#include "Sleep.h"
#include "OtaUpdate.h"
#include "EpaperContent.h"
#include "SerialShell.h"

#include "sensorBattery.h"

// Wie lange nach dem Aufwachen der lokale Webserver (/api/display/image, fuer
// manuelles Debugging) erreichbar bleibt, bevor wieder in den Deep Sleep geht.
#define EpaperActiveWindowSeconds 10
// Zykluszeit zwischen zwei Display-Refreshes/Update-Checks, auf Akku zugleich
// das Aufwach-Intervall.
#define EpaperWakeIntervalSeconds (10 * 60)
// Am Strom: Status (Akkuspannung usw.) einmal pro Minute melden
#define EpaperPoweredIntervalSeconds 60
#define EpaperMinSleepSeconds 10

// Unter dieser Akku-Prozentzahl nur noch schlafen: kein WLAN, kein Display,
// kein OTA-Check - das waere ohnehin alles stromintensiv und bringt dem Nutzer
// nichts mehr, wenn der Akku gleich leer ist. Stattdessen seltener aufwachen,
// um die Restladung zu strecken.
#define EpaperBatteryEmptyPercent 5.0
#define EpaperBatteryEmptySleepSeconds (60 * 60)

int activeSeconds = 0;
bool batteryEmpty = false;
// Zeit seit dem letzten Display-Refresh, ueberlebt Deep Sleep
RTC_DATA_ATTR uint32_t secondsSinceContent = 0;

void shutdown();

// Netzwerk-Teil eines Wachzyklus
void networkCycle(bool contentDue)
{
#ifdef TestWebServer
  connectWifi();

  // Leerer Akku (nur beim Laden moeglich): nur Status melden
  if (contentDue && !batteryEmpty)
  {
    secondsSinceContent = 0;
    OtaCheckAndUpdate(State.Battery.Percent, State.Battery.Voltage);
    EpaperContentUpdate(State.Battery.Percent, State.Battery.Voltage);
  }
  else if (WiFi.status() == WL_CONNECTED && strlen(Settings.Url) > 0)
  {
    EpaperReportStatus(State.Battery.Percent, State.Battery.Voltage);
  }

  server.begin();
#endif
}

void startup()
{
  // Reboot
  delay(10000);
  Debug("Start", "Boot");
  TransientStartup();

  PowerStartup();
  SensorBatteryStartup();
  SensorBatteryReadState();
  // Muss vor TimeSyncStartup() (WLAN) stehen
  PowerStateUpdate(State.Battery.Voltage, State.Battery.Percent);

  batteryEmpty = State.Battery.Percent < EpaperBatteryEmptyPercent;
  if (batteryEmpty && !PowerIsExternal())
  {
    Debug("Battery", "EmptySkipStartup");
    shutdown();
    return;
  }

  TimeSyncStartup();
  DisplayStartup();

  networkCycle(true);
}

void wakeup()
{
  Debug("Start", "Wakeup");
  TimeSyncWakeup();
  TransientWakeup();

  PowerWakeup();
  SensorBatteryWakeup();
  SensorBatteryReadState();
  PowerStateUpdate(State.Battery.Voltage, State.Battery.Percent);

  batteryEmpty = State.Battery.Percent < EpaperBatteryEmptyPercent;
  if (batteryEmpty && !PowerIsExternal())
  {
    Debug("Battery", "EmptySkipWakeup");
    shutdown();
    return;
  }

  DisplayWakeup();

  networkCycle(secondsSinceContent >= EpaperWakeIntervalSeconds);
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
  if (!batteryEmpty)
  {
    DisplayShutdown();
  }
  PowerShutdown();

  TransientShutdown();
  Debug("Shutdown", "Active");
  uint32_t awakeSeconds = millis() / 1000;
  uint64_t sleepSeconds;
  if (PowerIsExternal() || PowerNeedsSecondReading())
  {
    // Wachzeit abziehen, damit etwa einmal pro Minute gemeldet wird
    sleepSeconds = awakeSeconds + EpaperMinSleepSeconds < EpaperPoweredIntervalSeconds
                       ? EpaperPoweredIntervalSeconds - awakeSeconds
                       : EpaperMinSleepSeconds;
  }
  else
  {
    sleepSeconds = batteryEmpty ? EpaperBatteryEmptySleepSeconds : EpaperWakeIntervalSeconds;
  }
  secondsSinceContent += awakeSeconds + sleepSeconds;
  Debug("Power", String(PowerStateName()) + " " + String(State.Battery.Voltage, 3) + "V Sleep " + String((uint32_t)sleepSeconds));
  StartDeepSleep(sleepSeconds * 1000000ull);
}

void setup()
{
  setupPins();
  Serial.begin(112500);
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_UNDEFINED)
  {
    // Setup-Shell nur bei USB-Kaltstart erreichbar, nie nach Deep-Sleep-Wakeup.
    bool hasSettings = loadSettingsOrDefaults();
    SerialShellOfferAtBoot(hasSettings);
    startup();
  }
  else
  {
    loadSettingsOrDefaults();
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
