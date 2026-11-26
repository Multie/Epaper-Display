"""Epaper Display Integration.

Registriert pro Config-Entry einen HA-Webhook, den der ESP32 direkt anspricht:
  POST /api/webhook/<webhook_id>  -> Geraet meldet Akku/Firmware-Status
  GET  /api/webhook/<webhook_id>  -> Geraet holt die naechste Display-DSL ab

Webhooks sind in HA bewusst ohne Access-Token erreichbar (siehe HA-Doku) -
passend fuer ein Geraet, das keine OAuth-Flows kann. Die webhook_id selbst
ist der Zugriffsschutz (lang, zufaellig, siehe config_flow.py).
"""
from __future__ import annotations

import json
import logging
from datetime import datetime, timedelta
from pathlib import Path
from typing import Any

from aiohttp import web
from homeassistant.components import webhook
from homeassistant.config_entries import ConfigEntry
from homeassistant.core import HomeAssistant
from homeassistant.helpers.dispatcher import async_dispatcher_send
from homeassistant.helpers.network import NoURLAvailableError
from homeassistant.util import dt as dt_util

from .const import (
    CONF_CALENDAR_ENTITIES,
    CONF_SHOPPING_TODO_ENTITY,
    CONF_WEATHER_ENTITY,
    DOMAIN,
    SIGNAL_STATUS_UPDATED,
)
from .render import render_dsl

_LOGGER = logging.getLogger(__name__)
PLATFORMS = ["sensor"]

# Vom release.sh-Skript befuellt, nicht von Hand editieren.
FIRMWARE_DIR = Path(__file__).parent / "firmware"
FIRMWARE_VERSION_FILE = FIRMWARE_DIR / "version.json"
FIRMWARE_BIN_FILE = FIRMWARE_DIR / "firmware.bin"


async def async_setup_entry(hass: HomeAssistant, entry: ConfigEntry) -> bool:
    hass.data.setdefault(DOMAIN, {})
    hass.data[DOMAIN][entry.entry_id] = {
        "battery_percent": None,
        "battery_voltage": None,
        "firmware_version": None,
        "power_state": None,
        "ota_state": None,
        "last_seen": None,
        "last_refresh": None,
    }

    webhook_id = entry.data["webhook_id"]

    async def handle_webhook(hass: HomeAssistant, webhook_id: str, request: web.Request) -> web.Response:
        if request.method == "POST":
            return await _handle_status_post(hass, entry, request)
        firmware_request = request.query.get("firmware")
        if firmware_request == "version":
            return await _handle_firmware_version(hass, request)
        if firmware_request == "bin":
            return await _handle_firmware_bin(hass)
        return await _handle_render_get(hass, entry)

    # HA-Webhooks erlauben per Default nur POST/PUT - GET fuer den Render-Abruf
    # muss explizit freigegeben werden, sonst antwortet HA mit 405.
    webhook.async_register(
        hass,
        DOMAIN,
        entry.title,
        webhook_id,
        handle_webhook,
        allowed_methods=["GET", "POST"],
    )
    entry.async_on_unload(lambda: webhook.async_unregister(hass, webhook_id))

    # Fuer die serielle Setup-Shell der Firmware ("url <wert>")
    try:
        webhook_url = webhook.async_generate_url(hass, webhook_id)
    except NoURLAvailableError:
        # Keine HA-URL konfiguriert: nur den Pfad zeigen
        webhook_url = webhook.async_generate_path(webhook_id)
    hass.data[DOMAIN][entry.entry_id]["webhook_url"] = webhook_url
    # warning, damit die Zeile beim Standard-Loglevel sichtbar ist
    _LOGGER.warning("Webhook-URL fuer %s: %s", entry.title, webhook_url)

    await hass.config_entries.async_forward_entry_setups(entry, PLATFORMS)
    return True


async def async_unload_entry(hass: HomeAssistant, entry: ConfigEntry) -> bool:
    unload_ok = await hass.config_entries.async_unload_platforms(entry, PLATFORMS)
    if unload_ok:
        hass.data[DOMAIN].pop(entry.entry_id, None)
    return unload_ok


async def _handle_status_post(hass: HomeAssistant, entry: ConfigEntry, request: web.Request) -> web.Response:
    try:
        body: dict[str, Any] = await request.json()
    except ValueError:
        return web.Response(status=400, text="invalid json")

    store = hass.data[DOMAIN][entry.entry_id]
    store["battery_percent"] = _safe_float(body.get("batteryPercent"))
    store["battery_voltage"] = _safe_float(body.get("batteryVoltage"))
    store["firmware_version"] = body.get("firmwareVersion")
    store["power_state"] = body.get("powerState")
    store["ota_state"] = body.get("otaState")
    store["last_seen"] = dt_util.utcnow()

    async_dispatcher_send(hass, f"{SIGNAL_STATUS_UPDATED}_{entry.entry_id}")
    return web.Response(status=200, text="ok")


async def _handle_render_get(hass: HomeAssistant, entry: ConfigEntry) -> web.Response:
    store = hass.data[DOMAIN][entry.entry_id]

    weather_entity = entry.data[CONF_WEATHER_ENTITY]
    calendar_entities = entry.data[CONF_CALENDAR_ENTITIES]
    shopping_entity = entry.data[CONF_SHOPPING_TODO_ENTITY]

    current_condition, current_temperature, hourly_forecast, daily_forecast = await _fetch_weather(hass, weather_entity)
    events_today = await _fetch_events_today(hass, calendar_entities)
    shopping_count = _fetch_shopping_count(hass, shopping_entity)

    last_refresh = dt_util.now()
    store["last_refresh"] = last_refresh
    async_dispatcher_send(hass, f"{SIGNAL_STATUS_UPDATED}_{entry.entry_id}")

    dsl = render_dsl(
        current_condition=current_condition,
        current_temperature=current_temperature,
        hourly_forecast=hourly_forecast,
        daily_forecast=daily_forecast,
        events_today=events_today,
        shopping_count=shopping_count,
        battery_percent=store["battery_percent"],
        last_refresh=last_refresh,
    )

    # Firmware vergleicht Bytes 1:1 (chr(187)/chr(166)) - muss also tatsaechlich
    # Latin-1-codiert als einzelne Bytes rausgehen, nicht UTF-8-Mehrbyte.
    return web.Response(body=dsl.encode("latin-1"), content_type="text/plain", charset="ISO-8859-1")


async def _handle_firmware_version(hass: HomeAssistant, request: web.Request) -> web.Response:
    if not FIRMWARE_VERSION_FILE.exists():
        return web.Response(status=404, text="no firmware published")

    manifest = await hass.async_add_executor_job(
        lambda: json.loads(FIRMWARE_VERSION_FILE.read_text(encoding="utf-8"))
    )
    manifest["binUrl"] = str(request.url.update_query({"firmware": "bin"}))
    return web.json_response(manifest)


async def _handle_firmware_bin(hass: HomeAssistant) -> web.Response:
    if not FIRMWARE_BIN_FILE.exists():
        return web.Response(status=404, text="no firmware published")

    data = await hass.async_add_executor_job(FIRMWARE_BIN_FILE.read_bytes)
    return web.Response(body=data, content_type="application/octet-stream")


async def _fetch_weather(hass: HomeAssistant, entity_id: str):
    state = hass.states.get(entity_id)
    current_condition = state.state if state else None
    current_temperature = state.attributes.get("temperature") if state else None

    hourly_forecast: list[dict[str, Any]] = []
    daily_forecast: list[dict[str, Any]] = []
    try:
        hourly_response = await hass.services.async_call(
            "weather", "get_forecasts", {"entity_id": entity_id, "type": "hourly"},
            blocking=True, return_response=True,
        )
        daily_response = await hass.services.async_call(
            "weather", "get_forecasts", {"entity_id": entity_id, "type": "daily"},
            blocking=True, return_response=True,
        )
        hourly_forecast = (hourly_response or {}).get(entity_id, {}).get("forecast", [])
        daily_forecast = (daily_response or {}).get(entity_id, {}).get("forecast", [])
    except Exception:  # noqa: BLE001 - ein kaputtes Wetter-Entity soll das Display nicht leer lassen
        _LOGGER.exception("Konnte Wettervorhersage fuer %s nicht laden", entity_id)

    return current_condition, current_temperature, hourly_forecast, daily_forecast


async def _fetch_events_today(hass: HomeAssistant, entity_ids: list[str]) -> list[dict[str, Any]]:
    if not entity_ids:
        return []
    now = dt_util.now()
    start = dt_util.start_of_local_day(now)
    end = start + timedelta(days=1)

    try:
        response = await hass.services.async_call(
            "calendar", "get_events",
            {
                "entity_id": entity_ids,
                "start_date_time": start.isoformat(),
                "end_date_time": end.isoformat(),
            },
            blocking=True, return_response=True,
        )
    except Exception:  # noqa: BLE001 - ein kaputter Kalender soll das Display nicht leer lassen
        _LOGGER.exception("Konnte Kalenderevents fuer %s nicht laden", entity_ids)
        return []

    events: list[dict[str, Any]] = []
    for data in (response or {}).values():
        for event in data.get("events", []):
            start_value = event.get("start", "")
            events.append(
                {
                    "summary": event.get("summary", "(ohne Titel)"),
                    "start": start_value,
                    "all_day": "T" not in start_value,
                }
            )
    events.sort(key=lambda e: e["start"])
    return events


def _fetch_shopping_count(hass: HomeAssistant, entity_id: str) -> int:
    state = hass.states.get(entity_id)
    if state is None:
        return 0
    try:
        return int(float(state.state))
    except (ValueError, TypeError):
        return 0


def _safe_float(value: Any) -> float | None:
    try:
        return float(value)
    except (TypeError, ValueError):
        return None
