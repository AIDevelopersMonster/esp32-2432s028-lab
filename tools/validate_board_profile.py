#!/usr/bin/env python3
"""Validate the sample-specific ESP32-2432S028R electrical truth profile."""

from __future__ import annotations

import json
import pathlib
import re
import sys
from collections import defaultdict

PROFILE = pathlib.Path("config/board_profiles/esp32-2432s028r-sample-a.json")
HEADER = pathlib.Path("include/b1/BoardProfile.h")
VALID_DIRECTIONS = {"input", "output", "bidirectional", "input-only"}
VALID_EVIDENCE = {
    "software-tested",
    "component-and-software",
    "board-trace-and-software",
    "board-function",
    "connector-label",
    "connector-label-and-software",
}
INPUT_ONLY = {34, 35, 36, 39}
FLASH_RESERVED = {6, 7, 8, 9, 10, 11}


def fail(message: str) -> None:
    print(f"profile error: {message}", file=sys.stderr)
    raise SystemExit(1)


def validate_header_sync(signals: list[dict[str, object]]) -> None:
    text = HEADER.read_text(encoding="utf-8")
    pairs = {
        name: int(gpio)
        for name, gpio in re.findall(r'\{"([a-z0-9_]+)",\s*(-?\d+),\s*Direction::', text)
    }
    expected = {str(signal["name"]): int(signal["gpio"]) for signal in signals}
    if pairs != expected:
        missing = sorted(expected.keys() - pairs.keys())
        extra = sorted(pairs.keys() - expected.keys())
        mismatched = sorted(
            name for name in expected.keys() & pairs.keys() if expected[name] != pairs[name]
        )
        fail(
            "typed header differs from JSON profile; "
            f"missing={missing}, extra={extra}, gpio_mismatch={mismatched}"
        )


def main() -> None:
    data = json.loads(PROFILE.read_text(encoding="utf-8"))
    if data.get("schema_version") != 1:
        fail("schema_version must be 1")
    signals = data.get("signals")
    if not isinstance(signals, list) or not signals:
        fail("signals must be a non-empty list")

    names: set[str] = set()
    by_gpio: dict[int, list[dict[str, object]]] = defaultdict(list)
    for signal in signals:
        name = signal.get("name")
        gpio = signal.get("gpio")
        direction = signal.get("direction")
        evidence = signal.get("evidence")
        if not isinstance(name, str) or not name:
            fail("every signal needs a non-empty name")
        if name in names:
            fail(f"duplicate signal name {name}")
        names.add(name)
        if not isinstance(gpio, int) or not 0 <= gpio <= 39:
            fail(f"{name}: invalid GPIO {gpio}")
        if gpio in FLASH_RESERVED:
            fail(f"{name}: GPIO{gpio} is reserved for module flash")
        if direction not in VALID_DIRECTIONS:
            fail(f"{name}: invalid direction {direction}")
        if evidence not in VALID_EVIDENCE:
            fail(f"{name}: unknown evidence class {evidence}")
        if gpio in INPUT_ONLY and direction not in {"input", "input-only"}:
            fail(f"{name}: GPIO{gpio} is input-only")
        by_gpio[gpio].append(signal)

    reserved = set(data.get("reserved_gpio", []))
    if reserved != FLASH_RESERVED:
        fail("reserved_gpio must explicitly contain GPIO6-GPIO11")

    for gpio, entries in by_gpio.items():
        if len(entries) == 1:
            continue
        entry_names = {str(entry["name"]) for entry in entries}
        for entry in entries:
            conflicts = set(entry.get("conflicts", []))
            if not (entry_names - {str(entry["name"])}) <= conflicts:
                fail(f"GPIO{gpio}: shared ownership must be declared by every signal")

    required = {
        "tft_backlight": (21, "high"),
        "led_red": (4, "low"),
        "led_green": (16, "low"),
        "led_blue": (17, "low"),
        "boot_button": (0, "low"),
        "ldr": (34, "analog"),
    }
    indexed = {str(signal["name"]): signal for signal in signals}
    for name, (gpio, active_level) in required.items():
        signal = indexed.get(name)
        if signal is None:
            fail(f"missing required signal {name}")
        if signal["gpio"] != gpio or signal["active_level"] != active_level:
            fail(f"{name}: expected GPIO{gpio}, active level {active_level}")

    unresolved = data.get("unresolved", [])
    if not unresolved:
        fail("unresolved evidence must be recorded explicitly")

    validate_header_sync(signals)
    print(
        f"validated {data['profile_id']}: {len(signals)} signals, "
        f"{len(by_gpio)} GPIOs, {len(unresolved)} unresolved items"
    )


if __name__ == "__main__":
    main()
