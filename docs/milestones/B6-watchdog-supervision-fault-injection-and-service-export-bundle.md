# Milestone B6 — Watchdog Supervision, Fault Injection and Service Export Bundle

## Status

`IMPLEMENTED_IN_REPOSITORY / WATCHDOG_AND_FAULT_HIL_REQUIRED / QUALIFICATION_NOT_CLAIMED`

B6 adds task-watchdog supervision, deliberately gated fault injection and a bounded service-export bundle. It extends the B5 boot ledger and archive without treating software counters or parser fixtures as physical reliability proof.

## Delivered artifacts

- `include/b6/WatchdogSupervisor.h`, `src/b6/WatchdogSupervisor.cpp`;
- `include/b6/ServiceExport.h`, `src/b6/ServiceExport.cpp`;
- `include/b6/ServiceApp.h`, `src/b6/ServiceApp.cpp`;
- `tools/parse_b6_service.py` and a passing fixture;
- CI parser validation and PlatformIO build.

## Watchdog policy

The current laboratory policy uses an eight-second ESP task-watchdog timeout. The active Arduino loop task is registered and fed after each completed B6 service cycle. Telemetry records whether watchdog initialization succeeded, feed count, armed fault and whether execution began.

The timeout is a declared test value, not a universal production recommendation. Successful feed counting does not prove scheduler health, interrupt health or recovery from every deadlock class.

## Fault-injection gate

Fault commands are accepted only when the board entered recovery mode by holding BOOT during reset. Normal and automatic containment modes reject them.

Supported exact commands:

```text
B6 ARM STALL
B6 ARM RESTART
B6 ARM ARCHIVE_FAILURE
B6 EXPORT
```

`STALL` intentionally stops useful progress and should lead to a watchdog reset. `RESTART` invokes a controlled software restart. `ARCHIVE_FAILURE` suppresses archive/export writes without damaging the card. Each fault must be run separately with captured serial output and reset reason.

## Service export bundle

When a known-good microSD is present, `B6 EXPORT` recreates a bounded `/service` directory containing:

- `manifest.txt` with profile, firmware milestone, mode, boot ledger state, uptime and reset reason;
- a copy of `/diag/b5-current.log` when present;
- a copy of `/diag/b5-previous.log` when present.

The implementation replaces these three paths instead of accumulating unbounded service bundles. Export status reports success, export count, copied-file count and copied bytes.

## Machine-readable evidence

Each `B6` record includes sequence and uptime, service mode, boot counters, healthy marker, watchdog state and feeds, armed/executed fault, storage and archive state, service-export counters, CS idle state and SPI ownership invariants.

The host parser rejects missing fields, counter regressions, disabled watchdog, SPI invariant failures, unexpected recovery/fault states and missing required export evidence.

## Required HIL matrix

1. Normal boot: observe increasing watchdog feeds and healthy marker.
2. Recovery boot: issue `B6 EXPORT`, inspect all bundle files and run parser with `--require-export`.
3. Recovery boot: issue `B6 ARM STALL`, confirm watchdog reset and resulting reset reason.
4. Recovery boot: issue `B6 ARM RESTART`, confirm software reset and ledger update.
5. Recovery boot: issue `B6 ARM ARCHIVE_FAILURE`, confirm serial continues while archive/export reports failure.
6. Repeat with missing, removed, full and write-protected cards where practical.
7. Interrupt power during manifest creation and each source-file copy.
8. Retain raw serial, exported files, board/card IDs, firmware commit and power-control evidence.

Example normal/export validation:

```bash
python tools/parse_b6_service.py capture.log --min-records 30 --require-sd --require-export --allow-recovery
```

Fault captures additionally require `--allow-fault` and must be reviewed with the subsequent boot/reset record.

## Claim ceiling

B6 permits the claim that the repository contains a gated laboratory watchdog/fault-injection workflow and a bounded, inspectable service-export bundle.

It does not establish production watchdog coverage, deterministic recovery latency, NVS or filesystem atomicity, archive authenticity, secure command authorization, tamper resistance, exhaustive fault coverage, microSD endurance or field qualification.

## Next milestone

`B7 — Long-Run Soak Harness, Resource-Watermark Telemetry and Reproducible Qualification Pack`
