"""Baut die Display-DSL (siehe Epaper-Display-Firmware, DisplayParseText) aus
HA-Zustaenden. Reine Textverarbeitung, kein I/O hier - IO lebt in __init__.py.
"""
from __future__ import annotations

import logging
from datetime import datetime
from typing import Any, Iterable

_LOGGER = logging.getLogger(__name__)

# Protokoll-Trennzeichen aus der Firmware (Display/DisplayDrawModules.h):
# Key-Ende = chr(187) "»", Value-Ende = chr(166) "¦". Muessen als einzelne
# Bytes rausgehen (Latin-1), nicht als Mehrbyte-UTF-8 - siehe async_handle_webhook.
KEY_END = chr(187)
VALUE_END = chr(166)

# Die Bitmap-Fonts der Firmware (epd4in2bV2) haben kein Umlaut-Glyphenset -
# deshalb hier transliterieren statt Risiko auf Platzhalter-Kaestchen am Display.
_UMLAUT_MAP = {
    "ä": "ae", "ö": "oe", "ü": "ue", "ß": "ss",
    "Ä": "Ae", "Ö": "Oe", "Ü": "Ue",
}

_CONDITION_LABELS = {
    "sunny": "Sonnig",
    "clear-night": "Klar",
    "cloudy": "Bewoelkt",
    "partlycloudy": "Teilw.bewoelkt",
    "rainy": "Regen",
    "pouring": "Regen",
    "snowy": "Schnee",
    "snowy-rainy": "Schneeregen",
    "fog": "Nebel",
    "windy": "Wind",
    "windy-variant": "Wind",
    "lightning": "Gewitter",
    "lightning-rainy": "Gewitter",
    "hail": "Hagel",
    "exceptional": "Extrem",
}


def ascii_safe(text: str) -> str:
    for umlaut, replacement in _UMLAUT_MAP.items():
        text = text.replace(umlaut, replacement)
    return "".join(c if 32 <= ord(c) < 127 else "?" for c in text)


def condition_label(condition: str | None) -> str:
    if not condition:
        return "?"
    return _CONDITION_LABELS.get(condition, ascii_safe(condition))


def _field(key: str, value: Any) -> str:
    return f"{key}{KEY_END}{value}{VALUE_END}"


def _draw(draw_type: str, **fields: Any) -> str:
    parts = [_field("t", draw_type)]
    for key, value in fields.items():
        parts.append(_field(key, value))
    return "".join(parts)


def _text(x: int, y: int, w: int, h: int, label: str, size: int = 16, color: str = "b", align: str = "tl") -> str:
    return _draw("text", x=x, y=y, w=w, h=h, c=color, s=size, a=align, l=ascii_safe(label))


def _line(x: int, y: int, x2: int, y2: int, color: str = "b") -> str:
    return _draw("line", x=x, y=y, x2=x2, y2=y2, c=color)


def format_hhmm(iso_datetime: str) -> str:
    try:
        dt = datetime.fromisoformat(iso_datetime)
        return dt.strftime("%H:%M")
    except ValueError:
        return "--:--"


def format_temp(value: float | None) -> str:
    if value is None:
        return "?"
    return f"{round(value)}C"


def render_dsl(
    *,
    current_condition: str | None,
    current_temperature: float | None,
    hourly_forecast: list[dict[str, Any]],
    daily_forecast: list[dict[str, Any]],
    events_today: list[dict[str, Any]],
    shopping_count: int,
    battery_percent: float | None,
    last_refresh: datetime,
) -> str:
    lines: list[str] = []

    # Jetzt + naechste 2 Stunden
    lines.append(_text(4, 2, 392, 28, f"Jetzt: {condition_label(current_condition)} {format_temp(current_temperature)}", size=24))

    forecast_parts = []
    for entry in hourly_forecast[:2]:
        forecast_parts.append(f"{format_hhmm(entry.get('datetime', ''))} {condition_label(entry.get('condition'))} {format_temp(entry.get('temperature'))}")
    lines.append(_text(4, 32, 392, 20, "  |  ".join(forecast_parts) or "Keine Vorhersage", size=16))

    # Tagesvorhersage (erster Eintrag = heute)
    if daily_forecast:
        today = daily_forecast[0]
        lines.append(
            _text(
                4, 54, 392, 20,
                f"Heute: {condition_label(today.get('condition'))} {format_temp(today.get('templow'))}/{format_temp(today.get('temperature'))}",
                size=16,
            )
        )

    lines.append(_line(4, 80, 396, 80))

    # Termine heute
    lines.append(_text(4, 86, 392, 18, "Termine heute:", size=16))
    if not events_today:
        lines.append(_text(4, 106, 392, 18, "Keine Termine", size=16))
    else:
        shown = events_today[:4]
        y = 106
        for event in shown:
            prefix = "Ganztags" if event.get("all_day") else format_hhmm(event.get("start", ""))
            lines.append(_text(4, y, 392, 18, f"{prefix}  {event.get('summary', '')}", size=16))
            y += 20
        remaining = len(events_today) - len(shown)
        if remaining > 0:
            lines.append(_text(4, y, 392, 18, f"+ {remaining} weitere", size=16))

    lines.append(_line(4, 196, 396, 196))

    # Einkaufsliste
    lines.append(_text(4, 202, 392, 20, f"Einkaufsliste: {shopping_count} Artikel", size=16))

    lines.append(_line(4, 270, 396, 270))

    # Footer: Akku + letzter Refresh
    battery_label = f"{round(battery_percent)}%" if battery_percent is not None else "?"
    lines.append(_text(4, 276, 392, 20, f"Akku: {battery_label}   Stand: {last_refresh.strftime('%H:%M')}", size=16))

    return "\n".join(lines) + "\n"
