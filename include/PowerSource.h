// Leitet den Ladezustand aus dem Verlauf der Akkuspannung ab. Der CHRG-Ausgang
// des Ladereglers geht nur an eine LED, nicht an den ESP32.

#ifndef PowerSource_h
#define PowerSource_h

#include <Arduino.h>

// Aenderung gegenueber der Referenz, ab der die Richtung als gewechselt gilt
#define PowerTrendThresholdVolt 0.030
#define PowerFullPercent 99.0
#define PowerFullVolt 4.15

enum PowerTrend
{
    PowerTrendUnknown,
    PowerTrendCharging,
    PowerTrendDischarging
};

// Ueberleben Deep Sleep, werden bei jedem Neustart zurueckgesetzt.
// Referenz ist beim Laden der hoechste, beim Entladen der niedrigste Wert seit
// dem letzten Richtungswechsel, so addieren sich auch langsame Aenderungen.
RTC_DATA_ATTR float PowerReferenceVolt = 0;
RTC_DATA_ATTR PowerTrend PowerCurrentTrend = PowerTrendUnknown;
RTC_DATA_ATTR uint32_t PowerReadings = 0;

bool PowerFull = false;

// Einmal pro Wachzyklus aufrufen, VOR dem WLAN-Start: unter Funklast bricht die
// Spannung ein und saehe wie Entladen aus.
void PowerStateUpdate(float voltage, float percent)
{
    if (voltage <= 0)
    {
        return; // Fuel Gauge nicht lesbar
    }
    PowerReadings++;

    float delta = voltage - PowerReferenceVolt;
    if (PowerReferenceVolt <= 0)
    {
        PowerReferenceVolt = voltage;
    }
    else if (delta >= PowerTrendThresholdVolt)
    {
        PowerCurrentTrend = PowerTrendCharging;
        PowerReferenceVolt = voltage;
    }
    else if (delta <= -PowerTrendThresholdVolt)
    {
        PowerCurrentTrend = PowerTrendDischarging;
        PowerReferenceVolt = voltage;
    }
    else if ((PowerCurrentTrend == PowerTrendCharging && delta > 0) ||
             (PowerCurrentTrend == PowerTrendDischarging && delta < 0))
    {
        PowerReferenceVolt = voltage;
    }

    PowerFull = PowerCurrentTrend != PowerTrendDischarging &&
                percent >= PowerFullPercent && voltage >= PowerFullVolt;
}

// true = Netzteil sehr wahrscheinlich angeschlossen
bool PowerIsExternal()
{
    return PowerFull || PowerCurrentTrend == PowerTrendCharging;
}

// Nach einem Neustart gibt es noch nichts zu vergleichen
bool PowerNeedsSecondReading()
{
    return PowerReadings < 2;
}

const char *PowerStateName()
{
    if (PowerFull)
    {
        return "full";
    }
    switch (PowerCurrentTrend)
    {
    case PowerTrendCharging:
        return "charging";
    case PowerTrendDischarging:
        return "discharging";
    default:
        return "unknown";
    }
}

#endif
