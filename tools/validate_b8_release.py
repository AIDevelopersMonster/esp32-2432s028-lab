#!/usr/bin/env python3
import argparse
import json
from pathlib import Path

REQUIRED_IDS = {f"HW-{i:02d}" for i in range(1, 13)}


def load(path: Path):
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--baseline", default="release/baseline-v1.0.0-rc1.json")
    parser.add_argument("--matrix", default="release/hardware-acceptance-matrix-v1.0.0-rc1.json")
    parser.add_argument("--require-all-pass", action="store_true")
    args = parser.parse_args()

    baseline = load(Path(args.baseline))
    matrix = load(Path(args.matrix))
    errors = []

    if baseline.get("schema") != 1 or matrix.get("schema") != 1:
        errors.append("schema must be 1")
    if baseline.get("release") != matrix.get("release"):
        errors.append("release identifiers differ")
    if baseline.get("status") != "RC_FROZEN_HARDWARE_ACCEPTANCE_PENDING":
        errors.append("unexpected baseline status")

    items = matrix.get("items", [])
    ids = {item.get("id") for item in items}
    if ids != REQUIRED_IDS:
        errors.append(f"acceptance IDs differ: {sorted(ids)}")

    allowed = set(matrix.get("allowed_statuses", []))
    for item in items:
        if item.get("status") not in allowed:
            errors.append(f"invalid status for {item.get('id')}")
        for field in ("area", "requirement", "evidence"):
            if not item.get(field):
                errors.append(f"missing {field} for {item.get('id')}")

    if args.require_all_pass:
        not_pass = [item["id"] for item in items if item.get("status") != "PASS"]
        if not_pass:
            errors.append("not PASS: " + ",".join(not_pass))

    if errors:
        for error in errors:
            print("B8 release FAIL", error)
        return 1

    counts = {status: 0 for status in allowed}
    for item in items:
        counts[item["status"]] += 1
    print(f"B8 release PASS release={baseline['release']} items={len(items)} statuses={counts}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
