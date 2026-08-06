# Milestone B1 — Electrical Truth and Modular Self-Test

## Status

`IMPLEMENTED_IN_REPOSITORY / HARDWARE_MEASUREMENT_PARTIAL / QUALIFICATION_NOT_CLAIMED`

B1 converts the earlier documentation baseline into executable artifacts. It does not claim that every electrical path has been measured. It establishes a controlled profile, automated consistency checks, a modular self-test runner and a measurement backlog that can be closed with board-level evidence.

## Delivered artifacts

- `config/board_profiles/esp32-2432s028r-sample-a.json` — machine-readable sample profile;
- `tools/validate_board_profile.py` — host-side structural and conflict validation;
- `include/b1/BoardProfile.h` — typed compile-time profile;
- `include/b1/SelfTest.h` and `src/b1/SelfTest.cpp` — modular self-test contract and implementation;
- `src/main.cpp` — thin composition root;
- CI validation for the profile and firmware build.

## Evidence boundary

The profile distinguishes:

- software-tested signals;
- component-plus-software evidence;
- board-function evidence;
- connector-label evidence;
- shared-resource conflicts;
- unresolved electrical questions.

A `PASS` from the current firmware means that the software command or observation completed within the declared interface. It does not prove optical RGB output, calibrated illumination, regulator margin, signal integrity, storage endurance, radio performance or environmental suitability.

## Current confirmed profile

The executable profile preserves these assignments:

- TFT: MISO 12, MOSI 13, SCLK 14, CS 15, DC 2, backlight 21;
- touch: IRQ 36, MOSI 32, MISO 39, SCLK 25, CS 33;
- microSD: MISO 19, MOSI 23, SCLK 18, CS 5;
- RGB LED: red 4, green 16, blue 17, active LOW;
- LDR: GPIO34;
- audio: GPIO26;
- BOOT: GPIO0, active LOW and boot-strapping;
- UART0/P1: TX GPIO1, RX GPIO3;
- expansion candidates: GPIO35, GPIO22, GPIO21 and GPIO27;
- GPIO21 conflict: TFT backlight and P3 exposure;
- GPIO6–11: reserved for module Flash.

## Electrical measurement backlog

The following work still requires physical access to the documented sample and controlled instruments:

1. Preserve factory Flash and configuration before destructive experiments.
2. Measure continuity from P1, P3 and CN1 to ESP32/module pads.
3. Trace all U4 W25Q32 pins and determine whether it is connected, populated but unused, or revision-dependent.
4. Determine the actual split between the two AMS1117-3.3 regulator domains.
5. Measure 5-V and 3.3-V rails at idle and under display, Wi-Fi, SD and audio load combinations.
6. Record brownout and reset behavior during controlled supply ramps and interruptions.
7. Verify GPIO21 loading and external-drive hazards at P3.
8. Verify GPIO22 and GPIO27 startup state, pull behavior and external-load tolerance.
9. Measure TFT, touch and microSD chip-select idle levels and bus isolation.
10. Record thermal rise at regulators and high-load components.

Measurements must include instrument identity, setup, board serial/sample ID, firmware commit, ambient conditions, uncertainty and raw evidence references.

## Modular self-test behavior

The firmware now separates:

- composition and scheduling in `main.cpp`;
- sample profile and compile-time invariants;
- acquisition of LDR and BOOT state;
- commanded RGB sequence;
- TFT rendering;
- machine-readable serial reporting.

The serial line begins with `B1` and includes profile ID, schema version, sequence, uptime, raw LDR reading, BOOT state and subsystem states. This stable prefix is intended for later HIL parsing.

## CI gates

A B1 pull request must pass:

```text
python tools/validate_board_profile.py
pio run
```

The validator rejects:

- invalid GPIO numbers;
- use of GPIO6–11;
- output assignments on input-only GPIOs;
- duplicate signal names;
- undeclared shared ownership;
- loss of required active-level declarations;
- removal of the unresolved-evidence list.

## Acceptance state

B1 repository acceptance requires:

- the machine-readable and typed profiles agree by review;
- profile validation passes;
- PlatformIO builds the modular self-test;
- the self-test starts on the documented sample;
- TFT output is visible;
- RGB commands cycle;
- LDR values change when illumination changes;
- BOOT state changes while pressed;
- serial records are parseable and monotonic;
- failures and unmeasured properties remain explicit.

## Claim ceiling

B1 permits the claim:

> The repository contains a sample-specific, internally checked pin/resource profile and a modular firmware self-test for the documented ESP32-2432S028R baseline.

B1 does not permit claims of complete electrical reverse engineering, production qualification, universal CYD compatibility, calibrated sensing, safe external GPIO loading, secure firmware, environmental qualification or field readiness.

## Next milestone

`B2 — Unified SPI Integration and Hardware-in-the-Loop Smoke Test`

The next implementation step should add controlled TFT, touch and microSD coexistence, stable resource ownership, serial-parsed HIL smoke checks and captured hardware evidence.
