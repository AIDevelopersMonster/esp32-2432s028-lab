# Milestone B5 — Boot Ledger, Crash-Loop Containment and Bounded Diagnostic Archive

## Status

`IMPLEMENTED_IN_REPOSITORY / RESET_AND_ARCHIVE_HIL_REQUIRED / QUALIFICATION_NOT_CLAIMED`

B5 adds persistent boot accounting, a conservative crash-loop containment decision and a bounded microSD diagnostic archive. It extends B4 recovery-oriented configuration without claiming a complete production watchdog, forensic system or guaranteed storage durability.

## Delivered artifacts

- `include/b5/BootLedger.h` and `src/b5/BootLedger.cpp`;
- `include/b5/DiagnosticArchive.h` and `src/b5/DiagnosticArchive.cpp`;
- `include/b5/ContainmentApp.h` and `src/b5/ContainmentApp.cpp`;
- `tools/parse_b5_archive.py` and a passing fixture;
- B5 CI parser validation and PlatformIO build integration.

## Boot ledger

The NVS record stores schema, total boot count, consecutive boots not yet marked healthy, last reset reason, last healthy uptime, containment-entry count and checksum. Every boot increments `bootCount` and `consecutiveUnhealthyBoots`. A normal boot is marked healthy only after 15 seconds of uninterrupted runtime.

The ledger distinguishes created, valid, recovered and corrupt states. Checksum recovery means an invalid prior record is replaced with a new controlled record; it does not reconstruct lost history.

## Crash-loop containment

Three consecutive boots without reaching the healthy marker select `containment` mode. Holding BOOT during startup selects explicit `recovery` mode. Containment is intentionally conservative: it exposes state and avoids silently claiming that all crash-loop causes are isolated.

The threshold is a laboratory policy, not a universal reliability value. Reset cadence, brownout behavior and watchdog integration require physical validation.

## Bounded archive

When microSD is present and writable, B5 appends the exact machine-readable diagnostic line to `/diag/b5-current.log`. The active file is bounded to 64 KiB. At the limit, the previous archive is replaced and the current file is renamed to `/diag/b5-previous.log`.

This two-file policy bounds growth and exposes record, byte and rotation counters. It does not guarantee write atomicity, filesystem integrity after power loss, wear leveling, retention duration or evidentiary tamper resistance.

## Serial evidence contract

Each record begins with `B5` and includes profile, sequence, uptime, mode, boot count, unhealthy count, containment entries, reset reason, healthy marker, storage and archive state, archive counters, chip-select state, SPI owner and ownership errors.

## CI gates

```text
python tools/validate_board_profile.py
python tools/parse_b2_hil.py tests/fixtures/b2-hil-pass.log --require-sd
python tools/parse_b3_diagnostics.py tests/fixtures/b3-diagnostics-pass.log --require-sd --require-diagnostics
python tools/parse_b4_evidence.py tests/fixtures/b4-evidence-pass.log --min-records 4 --require-sd
python tools/parse_b5_archive.py tests/fixtures/b5-archive-pass.log --min-records 4 --require-sd
pio run
```

The fixture validates the parser only.

## Hardware acceptance

1. Capture a normal boot through the healthy marker.
2. Confirm the unhealthy count becomes zero after the declared uptime.
3. Interrupt or reset before the marker on three consecutive boots and confirm containment mode.
4. Hold BOOT at reset and confirm recovery mode remains distinct.
5. Run long enough to rotate the archive and verify bounded file sizes and counters.
6. Interrupt power during append and rotation; inspect both files and filesystem recovery.
7. Remove or write-protect the card and confirm serial diagnostics continue without pretending archive success.
8. Run `python tools/parse_b5_archive.py capture.log --min-records 30 --require-sd` for a normal archived run.
9. Preserve board identity, card identity, firmware commit, raw serial log and archive files.

## Claim ceiling

B5 permits the claim that the repository contains a checksum-protected boot ledger, a declared crash-loop containment threshold and a bounded two-file diagnostic archive with machine-checked serial evidence.

B5 does not establish complete crash-loop diagnosis, guaranteed NVS or filesystem atomicity, watchdog correctness, tamper-proof evidence, unlimited endurance, secure logs, field retention guarantees or production qualification.

## Next milestone

`B6 — Watchdog Supervision, Fault Injection and Service Export Bundle`
