"""Sensoren, die der ESP32 per Webhook-POST selbst befuellt (siehe __init__.py)."""
from __future__ import annotations

from homeassistant.components.sensor import SensorDeviceClass, SensorEntity
from homeassistant.config_entries import ConfigEntry
from homeassistant.const import PERCENTAGE, EntityCategory, UnitOfElectricPotential
from homeassistant.core import HomeAssistant, callback
from homeassistant.helpers.dispatcher import async_dispatcher_connect
from homeassistant.helpers.entity import DeviceInfo
from homeassistant.helpers.entity_platform import AddEntitiesCallback

from .const import DOMAIN, SIGNAL_STATUS_UPDATED


async def async_setup_entry(hass: HomeAssistant, entry: ConfigEntry, async_add_entities: AddEntitiesCallback) -> None:
    device_info = DeviceInfo(
        identifiers={(DOMAIN, entry.entry_id)},
        name=entry.title,
        manufacturer="EzSBC / Eigenbau",
        model="ESP32 Bat Pro + 4.2in Epaper",
    )
    async_add_entities(
        [
            EpaperDisplaySensor(entry, "battery_percent", "Akku", PERCENTAGE, SensorDeviceClass.BATTERY, device_info),
            EpaperDisplaySensor(
                entry, "battery_voltage", "Akkuspannung", UnitOfElectricPotential.VOLT, SensorDeviceClass.VOLTAGE, device_info
            ),
            EpaperDisplaySensor(entry, "firmware_version", "Firmware-Version", None, None, device_info),
            EpaperDisplaySensor(entry, "power_state", "Ladezustand", None, None, device_info),
            EpaperDisplaySensor(entry, "ota_state", "Update-Status", None, None, device_info),
            EpaperDisplaySensor(
                entry, "webhook_url", "Webhook-URL", None, None, device_info, EntityCategory.DIAGNOSTIC
            ),
            EpaperDisplaySensor(entry, "last_seen", "Zuletzt gemeldet", None, SensorDeviceClass.TIMESTAMP, device_info),
            EpaperDisplaySensor(entry, "last_refresh", "Letztes Display-Update", None, SensorDeviceClass.TIMESTAMP, device_info),
        ]
    )


class EpaperDisplaySensor(SensorEntity):
    """Liest einen Wert aus dem In-Memory-Store, den der Webhook-Handler befuellt."""

    _attr_has_entity_name = True
    _attr_should_poll = False

    def __init__(
        self,
        entry: ConfigEntry,
        store_key: str,
        name: str,
        unit: str | None,
        device_class: SensorDeviceClass | None,
        device_info: DeviceInfo,
        entity_category: EntityCategory | None = None,
    ) -> None:
        self._entry = entry
        self._store_key = store_key
        self._attr_name = name
        self._attr_native_unit_of_measurement = unit
        self._attr_device_class = device_class
        self._attr_unique_id = f"{entry.entry_id}_{store_key}"
        self._attr_device_info = device_info
        self._attr_entity_category = entity_category

    @property
    def native_value(self):
        return self.hass.data[DOMAIN][self._entry.entry_id].get(self._store_key)

    async def async_added_to_hass(self) -> None:
        self.async_on_remove(
            async_dispatcher_connect(
                self.hass, f"{SIGNAL_STATUS_UPDATED}_{self._entry.entry_id}", self._handle_update
            )
        )

    @callback
    def _handle_update(self) -> None:
        self.async_write_ha_state()
