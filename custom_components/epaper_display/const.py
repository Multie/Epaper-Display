"""Konstanten fuer die Epaper-Display-Integration."""

DOMAIN = "epaper_display"

CONF_WEATHER_ENTITY = "weather_entity"
CONF_CALENDAR_ENTITIES = "calendar_entities"
CONF_SHOPPING_TODO_ENTITY = "shopping_todo_entity"

DEFAULT_WEATHER_ENTITY = "weather.home"
DEFAULT_CALENDAR_ENTITIES = "calendar.kalender,calendar.nicole,calendar.alltag"
DEFAULT_SHOPPING_TODO_ENTITY = "todo.mealie_einkaufen"

# Unter diesem Pfad haengt pro Config-Entry ein Webhook:
# POST .../api/webhook/<webhook_id>  -> ESP32 meldet Akkustatus+Firmware-Version
# GET  .../api/webhook/<webhook_id>  -> ESP32 holt die Display-DSL zum Zeichnen
SIGNAL_STATUS_UPDATED = f"{DOMAIN}_status_updated"
