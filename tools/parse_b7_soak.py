#!/usr/bin/env python3
import argparse
import sys

REQUIRED = {"profile","seq","uptime_ms","boot_count","healthy_marked","free_heap","min_free_heap","largest_block","stack_watermark","resource_samples","watchdog_feeds","sd_present","archive_writable","archive_records","archive_rotations","archive_bytes","cs_idle","owner","owner_errors"}

def parse(line):
    fields = {}
    for token in line.strip().split()[1:]:
        if "=" in token:
            key, value = token.split("=", 1)
            fields[key] = value
    return fields

def fail(message):
    print(f"B7 SOAK FAIL: {message}", file=sys.stderr)
    raise SystemExit(1)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("path")
    ap.add_argument("--min-records", type=int, default=4)
    ap.add_argument("--min-uptime-ms", type=int, default=0)
    ap.add_argument("--require-sd", action="store_true")
    ap.add_argument("--max-heap-drop", type=int, default=65536)
    args = ap.parse_args()
    records = []
    with open(args.path, encoding="utf-8") as fh:
        for line in fh:
            if line.startswith("B7 ") and "event=" not in line:
                records.append(parse(line))
    if len(records) < args.min_records:
        fail(f"records={len(records)} expected>={args.min_records}")
    previous_seq = previous_uptime = previous_samples = previous_feeds = -1
    initial_heap = int(records[0]["free_heap"])
    for index, record in enumerate(records):
        missing = REQUIRED - record.keys()
        if missing: fail(f"record {index} missing {sorted(missing)}")
        if record["profile"] != "esp32-2432s028r-sample-a": fail("unexpected profile")
        seq = int(record["seq"]); uptime = int(record["uptime_ms"])
        samples = int(record["resource_samples"]); feeds = int(record["watchdog_feeds"])
        if seq <= previous_seq or uptime <= previous_uptime: fail("non-monotonic sequence or uptime")
        if samples <= previous_samples or feeds <= previous_feeds: fail("resource or watchdog counter stalled")
        if record["owner"] != "none" or int(record["owner_errors"]) != 0 or record["cs_idle"] != "1": fail("SPI invariant failed")
        if args.require_sd and (record["sd_present"] != "1" or record["archive_writable"] != "1"): fail("SD archive required")
        free_heap = int(record["free_heap"])
        if initial_heap - free_heap > args.max_heap_drop: fail("heap drop exceeded declared limit")
        if int(record["min_free_heap"]) > free_heap: fail("minimum heap exceeds current heap")
        if int(record["largest_block"]) <= 0 or int(record["stack_watermark"]) <= 0: fail("invalid resource watermark")
        previous_seq, previous_uptime, previous_samples, previous_feeds = seq, uptime, samples, feeds
    if int(records[-1]["uptime_ms"]) < args.min_uptime_ms: fail("soak duration below requirement")
    print(f"B7 SOAK PASS records={len(records)} uptime_ms={records[-1]['uptime_ms']} heap_drop={initial_heap-int(records[-1]['free_heap'])}")

if __name__ == "__main__": main()
