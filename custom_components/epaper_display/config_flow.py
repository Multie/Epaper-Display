"""Config Flow: Setup laeuft komplett ueber die HA-UI (Einstellungen ->
Geraete & Dienste -> Integration hinzufuegen -> Epaper Display), keine YAML."""
from __future__ import annotations

import secrets
from typing import Any

import voluptuous as vol
from homeassistant import config_entries
from homeassistant.helpers import selector

from .const import (
    CONF_CALENDAR_ENTITIES,
    CONF_SHOPPING_TODO_ENTITY,
    CONF_WEATHER_ENTITY,
    DEFAULT_SHOPPING_TODO_ENTITY,
    DEFAULT_WEATHER_ENTITY,
    DOMAIN,
)


def _schema() -> vol.Schema:
    return vol.Schema(
        {
            vol.Required(CONF_WEATHER_ENTITY, default=DEFAULT_WEATHER_ENTITY): selector.EntitySelector(
                selector.EntitySelectorConfig(domain="weather")
            ),
            vol.Required(CONF_CALENDAR_ENTITIES): selector.EntitySelector(
                selector.EntitySelectorConfig(domain="calendar", multiple=True)
            ),
            vol.Required(CONF_SHOPPING_TODO_ENTITY, default=DEFAULT_SHOPPING_TODO_ENTITY): selector.EntitySelector(
                selector.EntitySelectorConfig(domain="todo")
            ),
        }
    )


class EpaperDisplayConfigFlow(config_entries.ConfigFlow, domain=DOMAIN):
    """Config Flow fuer die Epaper-Display-Integration."""

    VERSION = 1

    async def async_step_user(self, user_input: dict[str, Any] | None = None):
        errors: dict[str, str] = {}

        if user_input is not None:
            data = {
                **user_input,
                # Lang + zufaellig: der Webhook ist ohne HA-Access-Token erreichbar,
                # die webhook_id selbst ist daher der einzige Zugriffsschutz.
                "webhook_id": secrets.token_hex(16),
            }
            return self.async_create_entry(title="Epaper Display", data=data)

        return self.async_show_form(step_id="user", data_schema=_schema(), errors=errors)

    async def async_step_reconfigure(self, user_input: dict[str, Any] | None = None):
        """Auswahl nachtraeglich aendern. Die webhook_id bleibt erhalten, am
        Geraet muss nichts neu eingestellt werden."""
        entry = self._get_reconfigure_entry()

        if user_input is not None:
            # data_updates ergaenzt die vorhandenen Daten, webhook_id bleibt
            return self.async_update_reload_and_abort(entry, data_updates=user_input)

        return self.async_show_form(
            step_id="reconfigure",
            data_schema=self.add_suggested_values_to_schema(_schema(), entry.data),
        )
