#!/usr/bin/env python3
import argparse
import hashlib
import json
import shutil
from pathlib import Path


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open('rb') as fh:
        for chunk in iter(lambda: fh.read(65536), b''):
            h.update(chunk)
    return h.hexdigest()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('capture')
    ap.add_argument('--output', default='b7-qualification-pack')
    ap.add_argument('--board-id', required=True)
    ap.add_argument('--firmware-commit', required=True)
    ap.add_argument('--card-id', default='not-recorded')
    args = ap.parse_args()

    source = Path(args.capture)
    if not source.is_file():
        raise SystemExit(f'missing capture: {source}')
    out = Path(args.output)
    out.mkdir(parents=True, exist_ok=True)
    capture_out = out / 'serial-capture.log'
    shutil.copyfile(source, capture_out)
    manifest = {
        'schema': 1,
        'milestone': 'B7',
        'board_profile': 'esp32-2432s028r-sample-a',
        'board_id': args.board_id,
        'firmware_commit': args.firmware_commit,
        'card_id': args.card_id,
        'capture_file': capture_out.name,
        'capture_sha256': sha256(capture_out),
        'reproducibility_note': 'Operator-supplied identities; no cryptographic device attestation claimed.'
    }
    (out / 'manifest.json').write_text(json.dumps(manifest, indent=2, sort_keys=True) + '\n', encoding='utf-8')
    (out / 'README.txt').write_text(
        'B7 qualification evidence pack\nRun parse_b7_soak.py against serial-capture.log and retain the untouched source capture.\n',
        encoding='utf-8')
    print(out)

if __name__ == '__main__':
    main()
