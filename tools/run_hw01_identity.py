#!/usr/bin/env python3
import argparse
import hashlib
import json
import shutil
import subprocess
from datetime import datetime, timezone
from pathlib import Path


def ask(label: str, default: str = "") -> str:
    suffix = f" [{default}]" if default else ""
    value = input(f"{label}{suffix}: ").strip()
    return value or default


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def git_commit() -> str:
    return subprocess.check_output(["git", "rev-parse", "HEAD"], text=True).strip()


def run_build(log_path: Path) -> None:
    result = subprocess.run(["pio", "run"], text=True, capture_output=True)
    log_path.write_text(result.stdout + "\n" + result.stderr, encoding="utf-8")
    if result.returncode != 0:
        raise SystemExit(f"PlatformIO build failed; see {log_path}")


def main() -> int:
    parser = argparse.ArgumentParser(description="Create the B8 HW-01 identity evidence package")
    parser.add_argument("--campaign", default="qualification/boards/cyd-r-sample-01")
    parser.add_argument("--skip-build", action="store_true", help="Use an already-built firmware.bin")
    args = parser.parse_args()

    root = Path(args.campaign)
    manifest_path = root / "campaign-manifest.json"
    matrix_path = root / "hardware-acceptance-matrix.json"
    if not manifest_path.exists() or not matrix_path.exists():
        raise SystemExit(f"Campaign files not found under {root}")

    evidence = root / "evidence" / "HW-01"
    evidence.mkdir(parents=True, exist_ok=True)
    build_log = evidence / "build.log"

    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    board_id = ask("Permanent board ID", manifest.get("board_id", "cyd-r-sample-01"))
    board_markings = ask("Board markings")
    power_id = ask("Power supply ID/model")
    power_rating = ask("Power supply rating", "5 V")
    uart_id = ask("USB-UART adapter/onboard bridge")
    microsd_id = ask("microSD ID/model")
    microsd_capacity = ask("microSD capacity")
    operator = ask("Operator")
    ambient = ask("Ambient conditions", "room temperature")

    if not args.skip_build:
        print("Running PlatformIO build...")
        run_build(build_log)
    elif not build_log.exists():
        build_log.write_text("Build skipped by operator.\n", encoding="utf-8")

    firmware = Path(".pio/build/esp32-2432s028r/firmware.bin")
    if not firmware.exists():
        raise SystemExit(f"Firmware not found: {firmware}")

    commit = git_commit()
    firmware_hash = sha256(firmware)
    copied_firmware = evidence / "firmware.bin"
    shutil.copy2(firmware, copied_firmware)
    (evidence / "firmware.sha256").write_text(f"{firmware_hash}  firmware.bin\n", encoding="utf-8")

    now = datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")
    protocol = {
        "schema": 1,
        "acceptance_item": "HW-01",
        "release": manifest["release"],
        "campaign": manifest["campaign"],
        "board_profile": manifest["board_profile"],
        "board_id": board_id,
        "board_markings": board_markings,
        "firmware_commit": commit,
        "firmware_sha256": firmware_hash,
        "firmware_file": "evidence/HW-01/firmware.bin",
        "power_supply_id": power_id,
        "power_supply_rating": power_rating,
        "usb_uart_adapter_id": uart_id,
        "microsd_id": microsd_id,
        "microsd_capacity": microsd_capacity,
        "operator": operator,
        "recorded_utc": now,
        "ambient_conditions": ambient,
        "evidence": [
            "evidence/HW-01/firmware.bin",
            "evidence/HW-01/firmware.sha256",
            "evidence/HW-01/build.log",
            "evidence/HW-01/board-front.jpg",
            "evidence/HW-01/board-back.jpg"
        ],
        "status": "EVIDENCE_CAPTURED_REVIEW_REQUIRED",
        "operator_declaration": "Values were entered for the identified physical sample. Photos must be added and independently reviewed before HW-01 is marked PASS."
    }
    protocol_path = evidence / "hw-01-protocol.json"
    protocol_path.write_text(json.dumps(protocol, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")

    manifest.update({
        "board_id": board_id,
        "board_markings": board_markings,
        "firmware_commit": commit,
        "firmware_sha256": firmware_hash,
        "power_supply_id": power_id,
        "power_supply_rating": power_rating,
        "usb_uart_adapter_id": uart_id,
        "microsd_id": microsd_id,
        "microsd_capacity": microsd_capacity,
        "operator": operator,
        "start_utc": now,
        "ambient_conditions": ambient,
        "status": "HW01_EVIDENCE_CAPTURED_REVIEW_REQUIRED"
    })
    manifest_path.write_text(json.dumps(manifest, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")

    print(f"HW-01 evidence created: {protocol_path}")
    print("Add board-front.jpg and board-back.jpg, review the values, then update HW-01 status manually.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
