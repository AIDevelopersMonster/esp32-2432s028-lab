# Milestone B4 — Configuration Transactions, Recovery Modes and Persistent Diagnostic Evidence

## Status

`IMPLEMENTED_IN_REPOSITORY / POWER-LOSS_TEST_REQUIRED / QUALIFICATION_NOT_CLAIMED`

B4 adds a controlled configuration journal, explicit recovery states and a stable diagnostic-evidence stream. It does not claim fully atomic persistence under every interruption point. Hardware power-cut testing remains mandatory.

## Delivered artifacts

- `include/b4/ConfigJournal.h` and `src/b4/ConfigJournal.cpp` — versioned dual-slot configuration storage;
- `include/b4/RecoveryApp.h` and `src/b4/RecoveryApp.cpp` — recovery-mode application and evidence output;
- `tools/parse_b4_evidence.py` — host-side evidence validator;
- `tests/fixtures/b4-evidence-pass.log` — parser fixture;
- B4 fixture validation in CI.

## Configuration transaction model

The journal stores two complete slots plus an active-slot marker. A commit:

1. validates the candidate in RAM;
2. increments generation;
3. writes the inactive slot;
4. reads it back and validates checksum and generation;
5. changes the active marker only after verification.

Boot scans both slots. The valid record with the highest generation is selected. If the active marker disagrees, the boot is reported as `recovered` and the recovery counter is incremented.

This is a recovery-oriented two-slot protocol, not a proof of filesystem- or NVS-level atomicity.

## Runtime configuration

The initial schema includes:

- schema version;
- generation;
- display brightness percentage;
- diagnostic sampling interval;
- diagnostics-enabled flag;
- checksum.

Invalid bounds, incompatible schema and checksum failure are rejected.

## Recovery modes

- `normal` — valid or recovered configuration can be used;
- `safe` — corrupt or failed configuration prevents normal assumptions;
- `recovery` — entered by holding BOOT during startup;
- `factory_reset_pending` — long BOOT hold arms the destructive workflow but does not erase automatically.

A destructive reset still requires a separately controlled confirmation path. A button hold alone is intentionally insufficient.

## Persistent diagnostic evidence

Each `B4` record contains:

- profile ID;
- sequence and monotonic uptime;
- recovery mode;
- configuration state and generation;
- recovery count;
- calibration validity;
- microSD presence;
- chip-select observation;
- SPI owner and ownership-error count;
- ESP32 reset reason.

The serial stream is the primary raw evidence. Later milestones may add bounded SD archival, rotation and export, but B4 does not claim durable evidence storage on microSD.

## Host validation

Run:

```bash
python tools/parse_b4_evidence.py capture.log --min-records 20 --require-sd
```

The parser requires monotonic records, valid or recovered configuration, generation at least one, released SPI ownership, zero ownership errors and idle chip-select observations.

Use `--allow-recovery` only for a test intentionally exercising recovery or safe modes.

## Hardware acceptance procedure

1. Upload the B4 firmware to the documented sample.
2. Complete or retain a valid B3 calibration.
3. Boot normally and capture at least 20 records.
4. Hold BOOT during reset and verify `mode=recovery`.
5. Perform controlled interruption tests around configuration commits using an instrumented procedure.
6. Confirm that either the previous or new valid generation is selected; a corrupt payload must not be accepted.
7. Confirm that recovery count changes only when marker repair occurs.
8. Verify long BOOT hold enters `factory_reset_pending` without immediate erase.
9. Preserve raw logs, supply waveform or power-controller timestamps, board ID and firmware commit.

## Required fault matrix

- interruption before inactive-slot write;
- interruption during inactive-slot write;
- interruption after slot verification but before marker change;
- interruption during marker change;
- corrupted active slot with valid alternate slot;
- corrupted marker with one or two valid slots;
- both slots invalid;
- incompatible schema;
- repeated reset during recovery.

## Claim ceiling

B4 permits the claim:

> The repository contains a checksum-validated, generation-controlled dual-slot configuration journal, explicit recovery modes and machine-validated diagnostic evidence records for the documented sample profile.

B4 does not permit claims of guaranteed power-loss atomicity, unlimited NVS endurance, tamper-proof evidence, secure factory reset, complete crash-loop handling, production recovery certification or universal CYD compatibility.

## Next milestone

`B5 — Boot Ledger, Crash-Loop Containment and Bounded Diagnostic Archive`
