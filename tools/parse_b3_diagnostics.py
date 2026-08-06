#!/usr/bin/env python3
"""Validate B3 calibration and diagnostic serial records."""

from __future__ import annotations

import argparse
import pathlib
import sys


def parse_record(line: str) -> dict[str, str] | None:
    if not line.startswith("B3 profile="):
        return None
    record: dict[str, str] = {}
    for token in line.strip().split()[1:]:
        if "=" in token:
            key, value = token.split("=", 1)
            record[key] = value
    return record


def fail(message: str) -> None:
    print(f"B3 parser error: {message}", file=sys.stderr)
    raise SystemExit(1)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("log", type=pathlib.Path)
    parser.add_argument("--min-records", type=int, default=4)
    parser.add_argument("--require-sd", action="store_true")
    parser.add_argument("--require-diagnostics", action="store_true")
    args = parser.parse_args()

    records = [record for line in args.log.read_text(encoding="utf-8").splitlines()
               if (record := parse_record(line)) is not None]
    if len(records) < args.min_records:
        fail(f"expected at least {args.min_records} records, got {len(records)}")

    required = {
        "profile", "seq", "uptime_ms", "mode", "calibration_valid", "cal_step",
        "pressed", "raw_x", "raw_y", "raw_z", "mapped_valid", "mapped_x",
        "mapped_y", "sd_present", "cs_idle", "owner", "owner_errors"
    }
    previous_seq = -1
    previous_uptime = -1
    diagnostics_seen = False
    mapped_seen = False
    for index, record in enumerate(records):
        missing = required - record.keys()
        if missing:
            fail(f"record {index}: missing {sorted(missing)}")
        seq = int(record["seq"])
        uptime = int(record["uptime_ms"])
        if seq <= previous_seq or uptime <= previous_uptime:
            fail(f"record {index}: sequence or uptime is not monotonic")
        previous_seq, previous_uptime = seq, uptime
        if record["cs_idle"] != "1":
            fail(f"record {index}: chip selects not idle")
        if record["owner"] != "none" or int(record["owner_errors"]) != 0:
            fail(f"record {index}: SPI ownership failure")
        if args.require_sd and record["sd_present"] != "1":
            fail(f"record {index}: microSD required but absent")
        if record["mode"] == "diagnostics":
            diagnostics_seen = True
            if record["calibration_valid"] != "1":
                fail(f"record {index}: diagnostics mode without valid calibration")
            if record["mapped_valid"] == "1":
                x, y = int(record["mapped_x"]), int(record["mapped_y"])
                if not (0 <= x < 320 and 0 <= y < 240):
                    fail(f"record {index}: mapped point outside display")
                mapped_seen = True

    if args.require_diagnostics and not diagnostics_seen:
        fail("no diagnostics-mode record found")
    if args.require_diagnostics and not mapped_seen:
        fail("no valid mapped touch point found")

    print(f"validated B3 diagnostics: {len(records)} records, diagnostics={diagnostics_seen}, mapped={mapped_seen}")


if __name__ == "__main__":
    main()
