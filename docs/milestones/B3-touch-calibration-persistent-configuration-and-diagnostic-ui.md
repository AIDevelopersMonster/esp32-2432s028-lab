# Milestone B3 — Touch Calibration, Persistent Configuration and Integrated Diagnostic UI

## Status

`IMPLEMENTED_IN_REPOSITORY / HARDWARE_CALIBRATION_CAPTURE_REQUIRED / QUALIFICATION_NOT_CLAIMED`

B3 turns the raw XPT2046 readings from B2 into a versioned, checksum-protected calibration record and a screen-space diagnostic interface. It preserves the B2 SPI ownership model and does not convert software-visible state into an electrical qualification claim.

## Delivered artifacts

- `include/b3/TouchCalibration.h` and `src/b3/TouchCalibration.cpp`;
- `include/b3/DiagnosticApp.h` and `src/b3/DiagnosticApp.cpp`;
- Preferences-backed calibration storage;
- four-point calibration workflow for rotation 1 and 320 x 240 landscape coordinates;
- integrated TFT diagnostic UI;
- stable `B3` serial records;
- `tools/parse_b3_diagnostics.py` and a passing fixture;
- CI validation of the B1 profile, B2 parser, B3 parser and firmware build.

## Calibration record

The stored record includes:

- schema version;
- raw minimum and maximum values for both mapped axes;
- display rotation;
- axis-swap and inversion flags;
- checksum.

The record is rejected when its schema, rotation, ranges or checksum are invalid. Invalid or missing data causes calibration mode rather than silent use of defaults.

## Calibration workflow

The UI presents four targets near the corners of the landscape display. Each sample is retained while the panel is pressed and accepted after release. After four samples, the firmware:

1. derives raw mapped-axis ranges;
2. validates minimum span;
3. computes the checksum;
4. writes the record to ESP32 Preferences/NVS;
5. switches to diagnostic mode only after successful persistence.

Holding BOOT during reset forces recalibration without erasing unrelated configuration.

## Diagnostic UI

Diagnostic mode shows:

- calibration validity;
- raw X/Y/Z;
- mapped X/Y;
- microSD presence;
- software-visible CS idle state;
- SPI ownership-error count;
- a marker at the mapped touch position.

The marker is an interaction aid, not a quantified accuracy certificate.

## Serial contract

Records begin with `B3` and include:

- profile, sequence and uptime;
- mode and calibration validity;
- calibration step;
- pressed state;
- raw and mapped coordinates;
- microSD state and capacity;
- CS idle state;
- current SPI owner and ownership errors.

The parser checks monotonicity, required fields, bounds, calibration validity in diagnostic mode, released ownership and optional mandatory storage presence.

## Physical acceptance procedure

On the documented sample:

1. upload the B3 firmware;
2. start with no valid B3 calibration or hold BOOT during reset;
3. touch and release each of the four targets;
4. confirm a `calibration_saved` event;
5. reset without holding BOOT and confirm direct entry into diagnostics mode;
6. touch all four corners and the centre;
7. confirm the marker follows the touch and remains within the display;
8. capture at least 30 `B3` records;
9. run:

```text
python tools/parse_b3_diagnostics.py capture.log --min-records 30 --require-sd --require-diagnostics
```

10. retain the raw log with board ID, firmware commit, display rotation and operator notes.

## Evidence boundary

B3 proves that the repository contains a calibration workflow, validates and persists its record, maps raw samples into declared display bounds and exposes integrated diagnostics.

It does not yet prove:

- calibrated absolute touch accuracy;
- repeatability across temperature, pressure, stylus or users;
- long-term NVS endurance;
- atomic persistence under every power-loss point;
- cross-unit calibration portability;
- edge linearity;
- gesture recognition;
- accessibility or production UI readiness;
- electrically measured SPI isolation;
- field qualification.

## Remaining work

- record per-target residual error against known screen coordinates;
- add median or trimmed-mean sampling during each press;
- add explicit calibration review, reject and retry controls;
- test power interruption during persistence;
- test corrupted and older-schema records;
- define migration when the calibration schema changes;
- capture HIL evidence on multiple board samples.

## Next milestone

`B4 — Configuration Transactions, Recovery Modes and Persistent Diagnostic Evidence`
