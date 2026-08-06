#!/usr/bin/env python3
from __future__ import annotations

import argparse
import pathlib
import sys

REQUIRED = {
    "profile", "seq", "uptime_ms", "mode", "config_state", "config_generation",
    "recovery_count", "calibration_valid", "sd_present", "cs_idle", "owner",
    "owner_errors", "reset_reason",
}


def parse_line(line: str) -> dict[str, str] | None:
    if not line.startswith("B4 ") or "event=" in line:
        return None
    fields: dict[str, str] = {}
    for token in line.strip().split()[1:]:
        if "=" in token:
            key, value = token.split("=", 1)
            fields[key] = value
    missing = REQUIRED - fields.keys()
    if missing:
        raise ValueError(f"missing fields: {sorted(missing)}")
    return fields


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("log", type=pathlib.Path)
    parser.add_argument("--min-records", type=int, default=20)
    parser.add_argument("--require-sd", action="store_true")
    parser.add_argument("--allow-recovery", action="store_true")
    args = parser.parse_args()

    records = []
    for line in args.log.read_text(encoding="utf-8").splitlines():
        item = parse_line(line)
        if item:
            records.append(item)
    if len(records) < args.min_records:
        raise ValueError(f"expected at least {args.min_records} records, got {len(records)}")

    previous_seq = previous_uptime = -1
    for item in records:
        seq = int(item["seq"])
        uptime = int(item["uptime_ms"])
        generation = int(item["config_generation"])
        recoveries = int(item["recovery_count"])
        errors = int(item["owner_errors"])
        if seq <= previous_seq or uptime <= previous_uptime:
            raise ValueError("sequence and uptime must be strictly monotonic")
        previous_seq, previous_uptime = seq, uptime
        if generation < 1 or recoveries < 0:
            raise ValueError("invalid generation or recovery count")
        if item["owner"] != "none" or errors != 0 or item["cs_idle"] != "1":
            raise ValueError("SPI ownership or chip-select invariant failed")
        if item["config_state"] not in {"valid", "recovered"}:
            raise ValueError(f"unacceptable config state {item['config_state']}")
        if not args.allow_recovery and item["mode"] != "normal":
            raise ValueError(f"unexpected mode {item['mode']}")
        if args.require_sd and item["sd_present"] != "1":
            raise ValueError("microSD required but absent")

    print(f"validated B4 evidence: {len(records)} records, generation={records[-1]['config_generation']}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError) as exc:
        print(f"B4 evidence error: {exc}", file=sys.stderr)
        raise SystemExit(1)
