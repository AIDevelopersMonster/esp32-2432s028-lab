#!/usr/bin/env python3
import argparse
import hashlib
import json
import shutil
from pathlib import Path


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(65536), b""):
            digest.update(chunk)
    return digest.hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--firmware", required=True)
    parser.add_argument("--output", required=True)
    parser.add_argument("--commit", required=True)
    parser.add_argument("--board-id", required=True)
    parser.add_argument("--matrix", default="release/hardware-acceptance-matrix-v1.0.0-rc1.json")
    parser.add_argument("--baseline", default="release/baseline-v1.0.0-rc1.json")
    parser.add_argument("--qualification-pack")
    args = parser.parse_args()

    firmware = Path(args.firmware)
    if not firmware.is_file():
        raise SystemExit("firmware file not found")

    output = Path(args.output)
    output.mkdir(parents=True, exist_ok=True)
    copied = []
    for source in [Path(args.baseline), Path(args.matrix), firmware]:
        target = output / source.name
        shutil.copy2(source, target)
        copied.append(target)

    if args.qualification_pack:
        source = Path(args.qualification_pack)
        if not source.exists():
            raise SystemExit("qualification pack not found")
        target = output / "qualification-pack"
        if target.exists():
            shutil.rmtree(target)
        shutil.copytree(source, target)

    manifest = {
        "schema": 1,
        "release": "v1.0.0-rc1",
        "firmware_commit": args.commit,
        "board_id": args.board_id,
        "files": [{"name": path.name, "sha256": sha256(path), "bytes": path.stat().st_size} for path in copied],
        "qualification_pack_included": bool(args.qualification_pack),
        "authenticated": False,
        "claim": "Inspectable laboratory release bundle; not a signed production release."
    }
    (output / "release-manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(f"B8 bundle PASS output={output} files={len(copied)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
