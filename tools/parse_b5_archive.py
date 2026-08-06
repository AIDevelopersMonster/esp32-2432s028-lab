#!/usr/bin/env python3
from __future__ import annotations
import argparse
from pathlib import Path

REQUIRED = {"seq","uptime_ms","mode","boot_count","unhealthy_boots","containment_entries","reset_reason","healthy_marked","sd_present","archive_writable","archive_records","archive_rotations","archive_bytes","cs_idle","owner","owner_errors"}


def parse_line(line: str) -> dict[str, str] | None:
    if not line.startswith("B5 profile="):
        return None
    fields: dict[str, str] = {}
    for token in line.strip().split()[1:]:
        if "=" in token:
            key, value = token.split("=", 1)
            fields[key] = value
    return fields


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("log", type=Path)
    parser.add_argument("--min-records", type=int, default=4)
    parser.add_argument("--require-sd", action="store_true")
    parser.add_argument("--allow-containment", action="store_true")
    args = parser.parse_args()

    rows = [row for line in args.log.read_text(encoding="utf-8").splitlines() if (row := parse_line(line))]
    if len(rows) < args.min_records:
        raise SystemExit(f"need {args.min_records} B5 records, found {len(rows)}")
    previous_seq = previous_uptime = -1
    previous_records = previous_bytes = -1
    for index, row in enumerate(rows, 1):
        missing = REQUIRED - row.keys()
        if missing:
            raise SystemExit(f"record {index}: missing {sorted(missing)}")
        seq, uptime = int(row["seq"]), int(row["uptime_ms"])
        archive_records, archive_bytes = int(row["archive_records"]), int(row["archive_bytes"])
        if seq <= previous_seq or uptime <= previous_uptime:
            raise SystemExit(f"record {index}: non-monotonic sequence or uptime")
        if archive_records < previous_records:
            raise SystemExit(f"record {index}: archive record counter regressed")
        if int(row["archive_rotations"]) == 0 and archive_bytes < previous_bytes:
            raise SystemExit(f"record {index}: archive bytes regressed without rotation")
        if row["owner"] != "none" or int(row["owner_errors"]) != 0 or row["cs_idle"] != "1":
            raise SystemExit(f"record {index}: SPI invariant failed")
        if args.require_sd and (row["sd_present"] != "1" or row["archive_writable"] != "1"):
            raise SystemExit(f"record {index}: required archive unavailable")
        if not args.allow_containment and row["mode"] != "normal":
            raise SystemExit(f"record {index}: unexpected mode {row['mode']}")
        if int(row["boot_count"]) < 1 or int(row["unhealthy_boots"]) < 0:
            raise SystemExit(f"record {index}: invalid ledger counters")
        previous_seq, previous_uptime = seq, uptime
        previous_records, previous_bytes = archive_records, archive_bytes
    print(f"validated {len(rows)} B5 records")

if __name__ == "__main__":
    main()
