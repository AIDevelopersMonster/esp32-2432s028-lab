#!/usr/bin/env python3
import argparse
from pathlib import Path

REQUIRED = {
    "profile", "seq", "uptime_ms", "mode", "boot_count", "unhealthy_boots",
    "healthy_marked", "watchdog_enabled", "watchdog_feeds", "fault",
    "fault_executed", "sd_present", "archive_writable", "export_ok", "exports",
    "export_files", "export_bytes", "cs_idle", "owner", "owner_errors",
}


def parse_line(line: str) -> dict[str, str] | None:
    line = line.strip()
    if not line.startswith("B6 ") or " event=" in line:
        return None
    fields: dict[str, str] = {}
    for token in line.split()[1:]:
        if "=" in token:
            key, value = token.split("=", 1)
            fields[key] = value
    missing = REQUIRED - fields.keys()
    if missing:
        raise ValueError(f"missing fields: {sorted(missing)}")
    return fields


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("log", type=Path)
    parser.add_argument("--min-records", type=int, default=10)
    parser.add_argument("--require-sd", action="store_true")
    parser.add_argument("--require-export", action="store_true")
    parser.add_argument("--allow-recovery", action="store_true")
    parser.add_argument("--allow-fault", action="store_true")
    args = parser.parse_args()

    records = []
    for raw in args.log.read_text(encoding="utf-8").splitlines():
        record = parse_line(raw)
        if record:
            records.append(record)
    if len(records) < args.min_records:
        raise SystemExit(f"B6 FAIL records={len(records)} required={args.min_records}")

    previous_seq = previous_uptime = previous_feeds = previous_exports = -1
    for index, record in enumerate(records, 1):
        seq = int(record["seq"])
        uptime = int(record["uptime_ms"])
        feeds = int(record["watchdog_feeds"])
        exports = int(record["exports"])
        if seq <= previous_seq or uptime <= previous_uptime:
            raise SystemExit(f"B6 FAIL non-monotonic record={index}")
        if feeds < previous_feeds or exports < previous_exports:
            raise SystemExit(f"B6 FAIL counter regression record={index}")
        if record["watchdog_enabled"] != "1":
            raise SystemExit(f"B6 FAIL watchdog disabled record={index}")
        if record["owner"] != "none" or record["owner_errors"] != "0" or record["cs_idle"] != "1":
            raise SystemExit(f"B6 FAIL SPI invariant record={index}")
        if args.require_sd and (record["sd_present"] != "1" or record["archive_writable"] != "1"):
            raise SystemExit(f"B6 FAIL storage unavailable record={index}")
        if not args.allow_recovery and record["mode"] != "normal":
            raise SystemExit(f"B6 FAIL unexpected mode={record['mode']} record={index}")
        if not args.allow_fault and (record["fault"] != "none" or record["fault_executed"] != "0"):
            raise SystemExit(f"B6 FAIL unexpected fault record={index}")
        previous_seq, previous_uptime = seq, uptime
        previous_feeds, previous_exports = feeds, exports

    if args.require_export and not any(r["export_ok"] == "1" and int(r["exports"]) > 0 for r in records):
        raise SystemExit("B6 FAIL no successful export")
    print(f"B6 PASS records={len(records)} final_feeds={previous_feeds} final_exports={previous_exports}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
