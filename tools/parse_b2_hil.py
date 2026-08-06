#!/usr/bin/env python3
"""Validate captured B2 serial output from the ESP32-2432S028R smoke test."""

from __future__ import annotations

import argparse
import pathlib
import sys

REQUIRED = {
    "profile",
    "seq",
    "uptime_ms",
    "display",
    "touch_sample",
    "touch_pressed",
    "touch_x",
    "touch_y",
    "touch_z",
    "sd_present",
    "sd_bytes",
    "cs_idle",
    "owner",
    "owner_errors",
    "tx_display",
    "tx_touch",
    "tx_storage",
}


def parse_line(line: str) -> dict[str, str] | None:
    line = line.strip()
    if not line.startswith("B2 profile="):
        return None
    fields: dict[str, str] = {}
    for token in line.split()[1:]:
        if "=" not in token:
            continue
        key, value = token.split("=", 1)
        fields[key] = value
    missing = REQUIRED - fields.keys()
    if missing:
        raise ValueError(f"missing fields: {', '.join(sorted(missing))}")
    return fields


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("log", type=pathlib.Path)
    parser.add_argument("--min-records", type=int, default=3)
    parser.add_argument("--require-sd", action="store_true")
    args = parser.parse_args()

    records: list[dict[str, str]] = []
    for number, line in enumerate(args.log.read_text(encoding="utf-8").splitlines(), 1):
        try:
            record = parse_line(line)
        except ValueError as error:
            raise SystemExit(f"line {number}: {error}") from error
        if record:
            records.append(record)

    if len(records) < args.min_records:
        raise SystemExit(f"expected at least {args.min_records} B2 records, got {len(records)}")

    previous_seq = -1
    previous_uptime = -1
    for index, record in enumerate(records, 1):
        seq = int(record["seq"])
        uptime = int(record["uptime_ms"])
        if seq <= previous_seq or uptime <= previous_uptime:
            raise SystemExit(f"record {index}: sequence and uptime must be strictly monotonic")
        previous_seq, previous_uptime = seq, uptime

        if record["display"] != "PASS":
            raise SystemExit(f"record {index}: display did not pass")
        if record["cs_idle"] != "1":
            raise SystemExit(f"record {index}: chip selects were not idle")
        if record["owner"] != "none" or int(record["owner_errors"]) != 0:
            raise SystemExit(f"record {index}: SPI ownership violation")
        if int(record["tx_display"]) < 1 or int(record["tx_touch"]) < 1:
            raise SystemExit(f"record {index}: expected display and touch transactions")
        if args.require_sd and record["sd_present"] != "1":
            raise SystemExit(f"record {index}: microSD required but absent")

    print(
        f"validated {len(records)} B2 records: seq {records[0]['seq']}..{records[-1]['seq']}, "
        f"sd_present={records[-1]['sd_present']}"
    )


if __name__ == "__main__":
    main()
