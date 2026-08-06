# Milestone B2 — Unified SPI Integration and Hardware-in-the-Loop Smoke Test

## Status

`IMPLEMENTED_IN_REPOSITORY / PHYSICAL_HIL_CAPTURE_REQUIRED / QUALIFICATION_NOT_CLAIMED`

B2 integrates the TFT, resistive touch controller and microSD paths into one firmware image with explicit ownership and machine-parseable smoke-test output. The repository implementation is complete when it builds and the host parser passes its fixtures. Hardware acceptance still requires a serial capture from the documented sample.

## Delivered topology

The documented sample exposes three different SPI pin groups, but the ESP32 does not provide three independent general-purpose hardware SPI controllers. B2 therefore uses:

- TFT: TFT_eSPI on the declared TFT/HSPI pin set;
- microSD: an explicit `SPIClass(VSPI)` on GPIO18/19/23 with CS GPIO5;
- XPT2046 touch: bounded software SPI on GPIO25/32/39 with CS GPIO33 and IRQ GPIO36.

This is a controlled reference topology, not a claim that it is the only valid architecture.

## Ownership model

`SpiCoordinator` provides a single logical ownership contract across display, touch and storage operations. It records:

- display transaction count;
- touch transaction count;
- storage transaction count;
- overlapping acquisition or mismatched-release violations.

A zero ownership-error count demonstrates software-level sequencing. It does not electrically prove that every chip-select signal remained at the intended voltage or that no peripheral drove MISO unexpectedly.

## Firmware smoke cycle

Each B2 cycle:

1. samples XPT2046 raw X, Y and pressure-related values through software SPI;
2. records whether touch IRQ is asserted;
3. checks the software-visible idle state of touch and microSD chip-select outputs;
4. updates the TFT status screen;
5. emits one stable `B2` serial record;
6. reports cumulative transaction and ownership counters.

microSD is probed during startup. Card absence is reported explicitly and is not silently treated as a functioning storage subsystem.

## Serial contract

A data record begins with:

```text
B2 profile=esp32-2432s028r-sample-a
```

Required fields include:

- `seq` and `uptime_ms`;
- `display`;
- `touch_sample`, `touch_pressed`, `touch_x`, `touch_y`, `touch_z`;
- `sd_present`, `sd_bytes`;
- `cs_idle`;
- `owner`, `owner_errors`;
- transaction counters for display, touch and storage.

The format is intentionally flat and whitespace-delimited so a serial capture can be checked without firmware-specific Python packages.

## Host HIL parser

`tools/parse_b2_hil.py` verifies:

- a minimum number of records;
- strictly increasing sequence and uptime;
- display pass state;
- chip-select idle observations;
- released ownership at record boundaries;
- zero ownership violations;
- evidence of display and touch transactions;
- optional mandatory microSD presence.

CI checks the parser against `tests/fixtures/b2-hil-pass.log`. That fixture validates parser behavior only; it is not hardware evidence.

## Physical HIL procedure

1. Insert a known-good FAT-formatted microSD card.
2. Upload the B2 firmware to the documented sample.
3. Open the serial port at 115200 baud and capture at least 20 `B2` records.
4. Confirm the TFT shows the B2 status screen without corruption.
5. Touch multiple screen regions and confirm raw coordinates and pressure-related values change.
6. Remove and reinsert the card only under a declared test procedure; the current implementation probes at startup.
7. Confirm `owner_errors=0`, `owner=none` and `cs_idle=1` in every accepted record.
8. Save the raw log with board sample ID, firmware commit, card identity, power source and test date.
9. Run:

```text
python tools/parse_b2_hil.py capture.log --min-records 20 --require-sd
```

## Required external observations

The parser cannot determine whether:

- the TFT image is visually correct;
- touch coordinates correspond to the contacted location;
- a card read returned correct file data;
- chip-select edges are electrically clean;
- MISO contention occurred below software visibility;
- SPI clocks meet signal-integrity margins.

These require operator observation, logic-analyser capture or oscilloscope evidence.

## Acceptance boundary

Repository acceptance requires:

- profile validation passes;
- B2 parser fixture passes;
- PlatformIO build passes;
- no undeclared SPI ownership path is introduced.

Hardware acceptance requires:

- successful display rendering;
- responsive raw touch samples;
- detected known-good microSD card;
- parseable monotonic B2 capture;
- zero recorded ownership violations;
- saved test evidence.

## Claim ceiling

B2 permits the claim:

> The repository contains one explicit coexistence topology for TFT, XPT2046 touch and microSD, plus a serial-parsed hardware smoke-test contract.

B2 does not claim electrical bus qualification, DMA safety, concurrent multi-task access, calibrated touch, filesystem endurance, hot-swap safety, universal card compatibility, universal CYD compatibility or production readiness.

## Next milestone

`B3 — Touch Calibration, Persistent Configuration and Integrated Diagnostic UI`
