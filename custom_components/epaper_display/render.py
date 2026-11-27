"""Baut die Display-DSL (siehe Epaper-Display-Firmware, DisplayParseText) aus
HA-Zustaenden. Reine Textverarbeitung, kein I/O hier - IO lebt in __init__.py.
"""
from __future__ import annotations

import logging
from datetime import datetime
from typing import Any, Iterable

_LOGGER = logging.getLogger(__name__)

DISPLAY_WIDTH = 400
DISPLAY_HEIGHT = 300

# Vom Gehaeuse verdeckter Rand in Pixeln - hier an den Einbau anpassen
MARGIN_LEFT = 5
MARGIN_TOP = 15
MARGIN_RIGHT = 15
MARGIN_BOTTOM = 10

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


def _circle(x: int, y: int, r: int, color: str = "b", fill: bool = True) -> str:
    return _draw("circle", x=x, y=y, r=r, c=color, f=int(fill))


def _rect(x: int, y: int, w: int, h: int, color: str = "b") -> str:
    return _draw("rect", x=x, y=y, w=w, h=h, c=color, f=1)


def weather_icon(condition: str | None, x: int, y: int, size: int) -> list[str]:
    """Zeichnet ein Wettersymbol aus Grundformen in ein size*size-Quadrat."""

    def s(value: float) -> int:
        return round(value * size / 48)

    def line(x1: float, y1: float, x2: float, y2: float, color: str = "b") -> list[str]:
        # zweimal versetzt, damit die Linie 2 px stark ist
        horizontal = y1 == y2
        return [
            _line(x + s(x1) + dx, y + s(y1) + dy, x + s(x2) + dx, y + s(y2) + dy, color)
            for dx, dy in ((0, 0), (0, 1) if horizontal else (1, 0))
        ]

    def circle(cx: float, cy: float, r: float, color: str = "b") -> str:
        return _circle(x + s(cx), y + s(cy), s(r), color)

    def sun(cx: float, cy: float, r: float) -> list[str]:
        parts = [circle(cx, cy, r, "r")]
        for dx, dy in ((0, -1), (1, -1), (1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0), (-1, -1)):
            scale = 0.72 if dx and dy else 1.0
            inner, outer = (r + 4) * scale, (r + 10) * scale
            parts += line(cx + dx * inner, cy + dy * inner, cx + dx * outer, cy + dy * outer, "r")
        return parts

    def cloud(dy: float = 0) -> list[str]:
        """Umriss: schwarze Form, darin dieselbe Form 2 px kleiner in weiss."""
        parts = []
        for color, shrink in (("b", 0), ("w", 3)):
            parts.append(circle(13, 28 + dy, 9 - shrink, color))
            parts.append(circle(24, 21 + dy, 12 - shrink, color))
            parts.append(circle(35, 28 + dy, 9 - shrink, color))
            parts.append(_rect(x + s(13), y + s(24 + dy) + s(shrink), s(22), s(13) - 2 * s(shrink), color))
        return parts

    def drops(positions: tuple[int, ...]) -> list[str]:
        return [part for px in positions for part in line(px, 37, px - 4, 46)]

    def flakes(positions: tuple[int, ...]) -> list[str]:
        return [circle(px, 42, 3) for px in positions]

    def bolt() -> list[str]:
        return line(26, 30, 19, 39, "r") + line(19, 39, 27, 39, "r") + line(27, 39, 20, 47, "r")

    if condition == "sunny":
        return sun(24, 24, 10)
    if condition == "clear-night":
        return [circle(23, 24, 17), circle(31, 18, 15, "w")]
    if condition == "partlycloudy":
        return sun(31, 17, 7) + cloud(7)
    if condition == "cloudy":
        return cloud(2)
    if condition == "rainy":
        return cloud(-5) + drops((18, 32))
    if condition == "pouring":
        return cloud(-5) + drops((12, 21, 30, 39))
    if condition == "snowy":
        return cloud(-5) + flakes((14, 24, 34))
    if condition == "snowy-rainy":
        return cloud(-5) + drops((18,)) + flakes((32,))
    if condition == "hail":
        return cloud(-5) + [circle(14, 40, 3), circle(24, 44, 3), circle(34, 40, 3)]
    if condition == "lightning":
        return cloud(-5) + bolt()
    if condition == "lightning-rainy":
        return cloud(-5) + bolt() + drops((13, 39))
    if condition == "fog":
        return [part for i, py in enumerate((14, 22, 30, 38)) for part in (
            _rect(x + s(6 + (i % 2) * 5), y + s(py), s(32), max(s(3), 1)),
        )]
    if condition in ("windy", "windy-variant"):
        return line(5, 16, 36, 16) + line(36, 16, 40, 10) + line(10, 25, 44, 25) + line(5, 34, 30, 34) + line(30, 34, 34, 40)
    # unbekannt oder "exceptional": Warnkreis
    return [
        _circle(x + s(24), y + s(24), s(18), "r", fill=False),
        _rect(x + s(22), y + s(13), s(5), s(15), "r"),
        circle(24, 35, 3, "r"),
    ]


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


def _fit(text: str, width: int, char_width: int) -> str:
    """Kuerzt auf die Pixelbreite - die Firmware bricht Text nicht selbst um."""
    max_chars = width // char_width
    return text if len(text) <= max_chars else text[: max_chars - 2] + ".."


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
    left = MARGIN_LEFT
    top = MARGIN_TOP
    right = DISPLAY_WIDTH - MARGIN_RIGHT
    bottom = DISPLAY_HEIGHT - MARGIN_BOTTOM
    width = right - left

    lines: list[str] = []

    # Jetzt
    lines.extend(weather_icon(current_condition, left, top, 48))
    headline = f"{condition_label(current_condition)} {format_temp(current_temperature)}"
    lines.append(_text(left + 58, top + 4, width - 58, 24, _fit(headline, width - 58, 17), size=24))
    lines.append(_text(left + 58, top + 32, width - 58, 16, "Jetzt", size=16))

    # Drei Spalten: +1h, +2h, heute
    columns = [
        (entry.get("condition"), format_hhmm(entry.get("datetime", "")), format_temp(entry.get("temperature")))
        for entry in hourly_forecast[:2]
    ]
    if daily_forecast:
        today = daily_forecast[0]
        low = today.get("templow")
        high = format_temp(today.get("temperature"))
        temps = high if low is None else f"{round(low)}/{high}"
        columns.append((today.get("condition"), "Heute", temps))

    column_width = width // 3
    for index, (condition, title, temperature) in enumerate(columns):
        x = left + index * column_width
        lines.extend(weather_icon(condition, x, top + 58, 32))
        lines.append(_text(x + 38, top + 58, column_width - 38, 16, title, size=16))
        lines.append(_text(x + 38, top + 76, column_width - 38, 16, temperature, size=16))
    if not columns:
        lines.append(_text(left, top + 66, width, 16, "Keine Vorhersage", size=16))

    lines.append(_line(left, top + 98, right, top + 98))

    # Einkaufsliste + Footer haengen am unteren Rand
    footer_y = bottom - 16
    shopping_y = footer_y - 30
    lines.append(_line(left, shopping_y - 7, right, shopping_y - 7))
    lines.append(_text(left, shopping_y, width, 16, f"Einkaufsliste: {shopping_count} Artikel", size=16))
    lines.append(_line(left, footer_y - 7, right, footer_y - 7))
    battery_label = f"{round(battery_percent)}%" if battery_percent is not None else "?"
    lines.append(_text(left, footer_y, width, 16, f"Akku: {battery_label}   Stand: {last_refresh.strftime('%H:%M')}", size=16))

    # Termine fuellen den Platz dazwischen
    events_y = top + 104
    lines.append(_text(left, events_y, width, 16, "Termine heute:", size=16))
    row_y = events_y + 19
    rows = max((shopping_y - 9 - row_y) // 19, 0)
    if not events_today and rows:
        lines.append(_text(left, row_y, width, 16, "Keine Termine", size=16))
    else:
        # Passt nicht alles, wird die letzte Zeile zum "+ N weitere"
        shown = events_today if len(events_today) <= rows else events_today[: max(rows - 1, 0)]
        for event in shown:
            prefix = "Ganztags" if event.get("all_day") else format_hhmm(event.get("start", ""))
            label = ascii_safe(f"{prefix}  {event.get('summary', '')}")
            lines.append(_text(left, row_y, width, 16, _fit(label, width, 11), size=16))
            row_y += 19
        remaining = len(events_today) - len(shown)
        if remaining > 0 and rows:
            lines.append(_text(left, row_y, width, 16, f"+ {remaining} weitere", size=16))

    return "\n".join(lines) + "\n"
