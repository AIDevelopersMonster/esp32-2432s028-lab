# System Bring-Up and Acceptance Test — ESP32-2432S028

> **Document status:** sample-specific bring-up and acceptance procedure for the author's `ESP32-2432S028` board.  
> **Scope:** receiving inspection, controlled first power-up, electrical screening, staged subsystem activation, golden firmware, automated self-test, fault isolation, stress testing, records, regression, release acceptance and requalification.  
> **Evidence rule:** a visible response, successful build or single boot is not equivalent to subsystem qualification. Each acceptance statement must name the board sample, firmware revision, test conditions, measured result and pass criterion.

## 1. Purpose

This chapter converts the preceding hardware-reference material into a repeatable operational test process.

The process is intended for:

- first activation of an unknown board;
- acceptance of a newly purchased sample;
- verification after repair or rework;
- regression testing after firmware changes;
- qualification after enclosure integration;
- comparison of different board batches;
- creation of a known-good reference unit;
- release of a board-and-firmware combination for a project.

The main rule is staged activation. Do not begin by connecting every peripheral, installing the final enclosure, enabling Wi-Fi, writing to storage and driving audio simultaneously.

## 2. Acceptance levels

Use explicit levels rather than one ambiguous word such as “working.”

| Level | Meaning |
|---|---|
| L0 — received | board identified and photographed, no power applied |
| L1 — electrically screened | no obvious short, rails and current are plausible under controlled power |
| L2 — programmable | USB-UART path, bootloader and firmware upload are repeatable |
| L3 — core functional | display, controls, RGB and LDR pass basic tests |
| L4 — peripheral functional | touch, microSD, audio and expansion interfaces pass declared tests |
| L5 — connected-system functional | Wi-Fi/Bluetooth and application communication pass |
| L6 — integrated | complete enclosure, cables and intended peripherals pass |
| L7 — stress qualified | duration, thermal, power, reset and coexistence matrices pass |
| L8 — release accepted | records complete, deviations approved and golden artifacts archived |

A board at L3 must not be described as fully accepted if storage, radio, audio, power transients and enclosure behavior remain untested.

## 3. Test assets

Prepare the following before first power-up:

- current-limited bench supply or USB power monitor with suitable protection;
- known-good USB data cable;
- direct host USB port or qualified powered hub;
- digital multimeter;
- oscilloscope for rail and signal investigation;
- logic analyzer where needed;
- ESD-safe workspace;
- magnification and adequate lighting;
- known-good microSD cards of documented type;
- verified speaker load and cable;
- mating cables for P1, P3, CN1 and P4;
- series resistors and protected test leads for expansion GPIO tests;
- Wi-Fi access point with known 2.4-GHz configuration;
- Bluetooth peer if required;
- temperature measurement equipment;
- repository checkout at a known commit;
- PlatformIO and/or Arduino toolchain version record;
- test-result template and sample identifier.

## 4. Golden artifacts

A controlled acceptance process should maintain:

```text
golden firmware source commit
golden compiled binary
golden bootloader and partition artifacts where relevant
SHA-256 hashes
toolchain and library versions
known-good USB cable and host setup
known-good microSD card
known-good speaker
known-good access point configuration
golden board sample
expected serial log
expected display screens
expected measured ranges
```

The golden board is a comparison reference, not proof that every later board is identical.

## 5. Sample identity and chain of evidence

Assign each board a sample ID before testing.

Record:

```text
sample ID
purchase source and date
seller listing or lot reference
PCB marking
visible module marking
notable component markings
board revision clues
serial number if available
photograph set
physical damage or rework
operator
date and test location
```

Use the same sample ID in photographs, logs, oscilloscope files, firmware output and acceptance records.

## 6. Receiving inspection — no power

### 6.1 Packaging and contamination

Inspect for:

- bent board or display;
- crushed connector housings;
- conductive foam contacting exposed circuitry;
- loose screws or metal debris;
- moisture;
- flux residue or contamination;
- damaged antistatic packaging;
- obvious prior use.

### 6.2 PCB inspection

Inspect both sides for:

- cracked PCB;
- delamination;
- lifted pads;
- solder bridges;
- missing components;
- tombstoned passives;
- cold or incomplete joints;
- damaged USB connector;
- displaced microSD socket;
- cracked module shield;
- antenna damage;
- flex-cable damage;
- display glass cracks;
- touch-overlay bubbles or pressure marks.

### 6.3 Marking verification

Confirm that the physical board matches the intended documentation baseline. Do not accept a seller title as the board identity.

At minimum record:

- `ESP32-2432S028` or other PCB text;
- ESP-WROOM module family marking;
- CH340 suffix;
- XPT2046 marking if readable;
- W25Q32 marking if readable;
- 8002A marking if readable;
- regulator markings;
- connector labels.

## 7. Unpowered electrical screening

### 7.1 Ground continuity

Verify that known ground points are mutually continuous:

- USB ground or shell as appropriate;
- P1 GND;
- P3 GND;
- CN1 GND;
- known PCB ground area.

Do not use speaker outputs as ground references.

### 7.2 Rail resistance

With all power removed, measure resistance from:

- USB 5 V to GND;
- P1 5 V to GND;
- CN1 3.3 V to GND;
- each accessible regulator output to GND.

Resistance values may charge capacitors and drift. Record the behavior, polarity and stabilization time rather than one isolated number.

A very low stable resistance requires investigation before power-up.

### 7.3 Connector short screening

Check for unintended shorts between adjacent contacts on P1, P3, CN1 and P4.

### 7.4 Speaker-output caution

P4 is a bridge-tied output. Do not interpret lack of continuity to ground as a fault, and do not short either output to ground.

## 8. First-power strategy

### 8.1 Minimal configuration

For first power-up disconnect:

- microSD card;
- speaker;
- all expansion cables;
- external sensors;
- external power sources;
- any enclosure metalwork that could contact the PCB.

Use only the board, a known-good USB data cable and measurement equipment.

### 8.2 Current limitation

When using a bench supply, begin with a conservative current limit that allows startup but limits damage. The exact value must be selected from measured board behavior and intended loads; this document does not assert a universal limit.

### 8.3 Observe before firmware upload

On initial application of power observe:

- supply current profile;
- display/backlight behavior;
- RGB LED behavior;
- regulator temperature;
- smoke, odor or sound;
- USB enumeration;
- serial boot output;
- repeated resets.

Immediately remove power if current rises abnormally, a component heats rapidly or visible damage occurs.

## 9. First-power record

```text
Sample ID:
Power source:
Current limit:
Cable:
Input voltage:
Initial current peak:
Steady current:
Backlight state:
RGB state:
USB enumeration:
Serial output:
Reset pattern:
U1 temperature after 1 min:
U7 temperature after 1 min:
Abnormal odor/noise/heat:
Result:
```

## 10. USB and bootloader acceptance

The board must pass the programming path before peripheral testing.

### 10.1 Enumeration

Record:

- host OS;
- driver version;
- VID/PID if available;
- assigned port;
- enumeration time;
- behavior after disconnect/reconnect;
- behavior through the intended hub if used.

### 10.2 Boot log

Capture the complete reset log at the expected boot baud. Record reset reason and any brownout message.

### 10.3 Manual bootloader entry

Verify the documented BOOT/RESET sequence.

### 10.4 Automatic bootloader entry

Run repeated uploads using the CH340G automatic control path.

Suggested acceptance sequence:

1. erase or upload a harmless test image;
2. upload at 115200;
3. reset and verify execution;
4. upload repeatedly at the project default speed;
5. repeat after USB reconnect;
6. repeat with intended cable and host;
7. record failures and recovery actions.

### 10.5 Upload-speed qualification

The repository currently configures `upload_speed = 921600`. This is a software setting, not a guaranteed electrical capability.

Accept 921600 only after repeated error-free uploads. Keep a lower validated fallback speed.

## 11. Build and artifact verification

Before upload, record:

```text
repository commit
branch or tag
PlatformIO version
Espressif platform version
framework version
library versions
build flags
binary size
firmware hash
CI result
local build result
```

The repository CI currently builds with PlatformIO, but a green build verifies compilation only. It does not exercise physical hardware.

## 12. Golden self-test firmware

The golden self-test should be deterministic, bounded and non-destructive.

It should:

- print firmware identity and source commit;
- print reset reason and uptime;
- initialize safe GPIO states early;
- test display colors and geometry;
- cycle RGB channels separately;
- read LDR repeatedly;
- report BOOT state;
- test touch raw channels without requiring calibration for basic presence;
- identify microSD without destructive formatting;
- test audio with a bounded low-level pattern only when a speaker is connected;
- scan or connect to Wi-Fi only in the appropriate test phase;
- report pass/fail/skip for every subsystem;
- avoid exposing credentials;
- emit a machine-readable summary;
- remain responsive after a subsystem failure.

## 13. Result-state vocabulary

Use these states consistently:

```text
PASS      requirement met
FAIL      requirement not met
SKIP      intentionally not executed
BLOCKED   prerequisite failed
WARN      outside preferred range but accepted by approved deviation
OPEN      insufficient evidence
NOT_FITTED hardware not present on this revision
```

Do not report `PASS` merely because no exception occurred.

## 14. Machine-readable result example

```text
TEST_BEGIN sample=CYD-001 firmware=abc123
POWER PASS input_mv=5008 idle_ma=...
USB PASS port=...
DISPLAY PASS controller_profile=ili9341-working
RGB PASS red=1 green=1 blue=1
LDR PASS min=... max=...
BOOT PASS released=1 pressed=0
TOUCH PASS irq=... raw_x=... raw_y=... raw_z=...
SD SKIP reason=no_card
AUDIO SKIP reason=no_speaker
WIFI SKIP reason=phase_not_enabled
TEST_END overall=PASS_WITH_SKIPS
```

Values and keys should be stable enough for automated parsing.

## 15. Core-board test sequence

Recommended order:

1. power and reset stability;
2. USB enumeration;
3. serial boot log;
4. firmware upload;
5. backlight control;
6. TFT initialization;
7. RGB LED;
8. BOOT input;
9. LDR ADC;
10. touch controller;
11. microSD;
12. audio;
13. expansion connectors;
14. Wi-Fi;
15. Bluetooth if required;
16. combined-load tests;
17. enclosure and thermal tests.

This order minimizes the number of active variables during fault isolation.

## 16. Display acceptance

### 16.1 Basic presence

Verify:

- backlight turns on and off under GPIO21 control;
- display initializes repeatedly after cold boot and reset;
- no persistent all-white or all-black failure;
- no visible cracked region or missing rows/columns.

### 16.2 Pattern set

Display:

- solid red, green, blue, white and black;
- grayscale steps;
- color bars;
- one-pixel border;
- horizontal and vertical lines;
- text in each rotation;
- checkerboard or fine pattern.

### 16.3 Acceptance record

Record:

- working controller profile;
- SPI frequency;
- rotation mapping;
- color-order behavior;
- observed defects;
- initialization success count;
- backlight active level;
- current and temperature impact.

A working ILI9341 profile remains an implementation result unless controller identity is independently confirmed.

## 17. RGB LED acceptance

Test each channel independently:

```text
all off
red only
green only
blue only
combinations
PWM sweep if required
```

Verify active-LOW behavior and confirm that startup does not leave an unintended high-brightness state.

Acceptance should include visible channel identity and, where needed, current measurement.

## 18. BOOT and RESET acceptance

### 18.1 BOOT

Verify:

- released state;
- pressed state;
- debounce behavior;
- manual bootloader entry;
- no stuck-low condition after enclosure assembly.

### 18.2 RESET

Verify:

- clean reset from the physical button;
- reset through the automatic programming circuit;
- expected boot log after reset;
- no reset loop;
- no mechanical plunger preload.

## 19. LDR acceptance

Test under at least three conditions:

- covered/dark;
- normal ambient light;
- controlled bright illumination.

Record raw ADC distribution rather than one sample.

Acceptance should verify:

- readings change monotonically in the expected direction;
- no constant zero/full-scale result;
- noise is within the project-defined range;
- Wi-Fi and TFT activity do not make the signal unusable;
- final enclosure optical path is qualified separately.

## 20. Touch acceptance

### 20.1 Basic electrical test

Verify:

- controller communication;
- IRQ response where used;
- raw X/Y/Z change on touch;
- release detection;
- repeated operation after reset.

### 20.2 Spatial test

Touch:

- four corners;
- center;
- edge points;
- multiple pressure levels.

### 20.3 Calibration acceptance

Calibration must be sample- and rotation-aware.

Record:

- raw minima/maxima;
- transform coefficients;
- screen rotation;
- residual error at test points;
- pressure threshold;
- filter settings.

### 20.4 Enclosure retest

Repeat touch acceptance after bezel, gasket and protective-window installation.

## 21. microSD acceptance

### 21.1 Non-destructive first test

Use a known-good card with noncritical data.

Verify:

- card detection;
- card type and capacity report;
- directory listing;
- read of a known test file;
- creation of a dedicated test file;
- flush and close;
- readback and hash comparison;
- deletion of only the test file.

### 21.2 Repetition

Repeat mount/read/write cycles and cold boots with card inserted.

### 21.3 Card matrix

Test multiple documented cards if the application depends on broad compatibility.

### 21.4 Failure conditions

Test:

- no card;
- unformatted or unsupported filesystem where safe;
- full filesystem;
- write-protected adapter where applicable;
- interrupted operation only using a controlled sacrificial test card.

Do not perform destructive power-loss tests on valuable media.

## 22. Audio acceptance

### 22.1 Preconditions

Connect only a verified speaker load between the two P4 outputs.

Do not connect either speaker output to ground or a ground-referenced oscilloscope input without an appropriate differential method.

### 22.2 Functional sequence

- silence/idle;
- low-level tone;
- frequency sweep within intended range;
- increasing level up to the approved limit;
- repeated start/stop;
- simultaneous display and Wi-Fi load if required.

### 22.3 Acceptance observations

Record:

- audible output;
- distortion or clipping;
- idle noise;
- enclosure rattle;
- amplifier temperature;
- supply droop;
- reset behavior;
- speaker current or differential voltage where measured.

## 23. Expansion-connector acceptance

### 23.1 Orientation verification

Confirm P3 and CN1 labels directly on the board.

### 23.2 GPIO35

Verify input-only behavior using externally biased logic levels within 3.3-V limits.

### 23.3 GPIO22 and GPIO27

Before driving:

- measure unpowered resistance;
- observe boot waveform;
- use a series resistor;
- test input first;
- then test low-rate output;
- verify no effect on all onboard systems.

### 23.4 GPIO21

Treat as backlight-owned. External connector testing must not fight the onboard circuit.

### 23.5 CN1 3.3 V

Measure rail voltage unloaded and under a small controlled load. Do not infer the maximum available current from a short successful test.

## 24. Wi-Fi acceptance

Test in stages:

1. scan for known 2.4-GHz networks;
2. connect in station mode;
3. obtain DHCP address;
4. verify gateway reachability;
5. verify DNS;
6. synchronize NTP;
7. run latency and packet-loss test;
8. run bounded TCP/UDP transfer;
9. reboot access point and verify reconnect;
10. repeat in final enclosure.

Record channel, RSSI statistics, orientation, access point, enclosure and power source.

A successful NTP display alone is not full wireless acceptance.

## 25. Bluetooth acceptance

Execute only if Bluetooth is part of the intended product.

For BLE record:

- advertising visibility;
- connection success;
- GATT discovery;
- read/write/notify behavior;
- reconnect;
- range points;
- coexistence with Wi-Fi.

For Bluetooth Classic define the exact profile and peer before testing.

## 26. Power-rail acceptance

### 26.1 Static measurements

Measure:

- USB input voltage;
- board 5-V node;
- both 3.3-V regulator outputs;
- idle current;
- subsystem current deltas.

### 26.2 Dynamic measurements

Capture rail minima during:

- boot;
- TFT initialization;
- full backlight;
- RGB transitions;
- SD initialization and write;
- audio peaks;
- Wi-Fi association and transmit;
- combined workload.

### 26.3 Reset correlation

When a reset occurs, capture rail waveform and reset reason. Do not classify it as a software failure without power evidence.

## 27. Thermal acceptance

Measure temperatures in defined conditions:

```text
ambient
board orientation
enclosure state
power source
backlight level
Wi-Fi load
SD load
audio load
external load
duration
```

Record U1, U7, ESP-WROOM shield, audio amplifier, display rear and internal enclosure air.

Use project-specific maximum temperatures based on component data and product requirements. This chapter does not invent universal limits.

## 28. Reset and power-cycle matrix

Test:

- cold power-on;
- warm reset;
- BOOT held during reset;
- rapid power cycle;
- slow supply ramp if relevant;
- USB disconnect/reconnect;
- access-point outage during boot;
- card inserted and absent;
- speaker connected and absent;
- expansion peripherals connected and absent;
- final enclosure closed and open.

Record every reset reason and whether the system recovers autonomously.

## 29. Combined-load stress matrix

| Scenario | TFT | Touch | SD | Audio | Wi-Fi | Expansion | Duration | Result |
|---|---|---|---|---|---|---|---|---|
| baseline idle | static | idle | absent | off | off | none | | |
| UI activity | redraw | active | absent | off | off | none | | |
| storage stress | active | active | write/read | off | off | none | | |
| network stress | active | active | idle | off | TCP/UDP | none | | |
| audio stress | active | active | idle | active | off | none | | |
| full system | active | active | write | active | transfer | intended | | |
| enclosure worst case | active | active | write | active | transfer | intended | | |

## 30. Duration tests

Suggested staged durations:

- 5-minute smoke/functional test;
- 30-minute subsystem test;
- 2-hour combined-load test;
- overnight idle/reconnect test;
- overnight realistic workload;
- longer endurance only after earlier stages pass.

During duration tests log:

- uptime;
- reset count and reason;
- heap metrics;
- task watchdog events;
- Wi-Fi disconnect count;
- SD errors;
- touch anomalies;
- display corruption;
- temperatures;
- supply minima where instrumented.

## 31. Fault isolation method

When a test fails:

1. stop the acceptance sequence;
2. preserve logs and physical state;
3. identify the first failed stage;
4. return to the last known passing configuration;
5. remove optional loads;
6. replace one known-good external item at a time;
7. lower bus or upload speed where appropriate;
8. compare with the golden board;
9. capture electrical evidence;
10. classify root cause before rework.

Do not erase evidence by repeatedly flashing unrelated firmware or changing many variables simultaneously.

## 32. Fault classification

Use categories such as:

- mechanical damage;
- assembly defect;
- power integrity;
- USB/host/cable;
- boot-strapping;
- firmware configuration;
- pin/resource conflict;
- display initialization;
- touch calibration;
- storage/media;
- audio load;
- RF/environment;
- enclosure interaction;
- test-fixture error;
- unknown/open.

## 33. Quarantine and rework

A failed board should be labeled and isolated from accepted stock.

Record:

- failure symptom;
- failed test step;
- evidence files;
- suspected cause;
- rework performed;
- parts replaced;
- operator;
- post-rework full regression result.

A repaired board must not resume testing only from the failed step if the rework could affect earlier systems.

## 34. Regression tiers

### Tier A — build regression

- CI build;
- local clean build;
- binary-size check;
- static pin/resource checks.

### Tier B — smoke regression

- upload;
- boot;
- display;
- RGB;
- BOOT;
- LDR;
- no reset loop.

### Tier C — subsystem regression

- touch;
- microSD;
- audio;
- expansion;
- Wi-Fi/Bluetooth as applicable.

### Tier D — integration regression

- complete application;
- intended peripherals;
- enclosure;
- power and thermal checks.

### Tier E — release regression

- full acceptance matrix;
- long-duration test;
- recovery scenarios;
- archival review.

Choose the tier based on the change impact, not developer convenience.

## 35. Change-impact examples

| Change | Minimum regression consideration |
|---|---|
| text/UI change | build, display smoke, memory impact |
| TFT driver or SPI clock | display, touch coexistence, boot straps, thermal |
| pin-map change | full pin/resource and affected-subsystem regression |
| SD library update | card matrix, corruption and coexistence tests |
| Wi-Fi stack update | reconnect, security, throughput, power, ADC2 interactions |
| audio PWM change | speaker, distortion, power and timer conflicts |
| enclosure change | touch, RF, thermal, access and strain relief |
| regulator or power modification | full electrical and thermal acceptance |
| board batch change | receiving, identity and full qualification sample set |

## 36. Acceptance criteria discipline

Every test should define before execution:

```text
purpose
preconditions
instrumentation
firmware revision
procedure
measured variables
pass threshold
warning threshold
fail threshold
repeat count
required evidence
```

Avoid post hoc acceptance such as “it seems good enough.”

## 37. Deviations

A deviation must state:

- requirement not met;
- measured result;
- risk assessment;
- temporary or permanent status;
- affected samples/releases;
- compensating controls;
- approval authority;
- expiration or review date.

A warning is not automatically an approved deviation.

## 38. Release package

Archive:

```text
release identifier
source commit/tag
binary and SHA-256
toolchain manifest
pin/resource manifest
accepted board revisions
sample IDs tested
full result logs
photographs
rail captures
thermal records
RF records
known limitations
approved deviations
recovery procedure
rollback image
operator and approval
```

## 39. Acceptance report template

```text
Project:
Release:
Sample ID:
PCB marking:
Module marking:
Firmware commit:
Binary SHA-256:
Toolchain:
Operator:
Date:

L0 Receiving inspection: PASS/FAIL
L1 Electrical screening: PASS/FAIL
L2 Programming: PASS/FAIL
L3 Core functional: PASS/FAIL
L4 Peripheral functional: PASS/FAIL/SKIP
L5 Wireless: PASS/FAIL/SKIP
L6 Integrated enclosure: PASS/FAIL/SKIP
L7 Stress qualified: PASS/FAIL/SKIP

Power summary:
Thermal summary:
Display summary:
Touch summary:
Storage summary:
Audio summary:
Expansion summary:
Wireless summary:
Reset/recovery summary:

Warnings:
Failures:
Approved deviations:
Open questions:
Evidence paths:
Overall disposition: ACCEPT / REJECT / QUARANTINE / CONDITIONAL
Approval:
```

## 40. Evidence levels

- **Observed** — visual or serial behavior seen once;
- **Repeated** — reproduced across declared repetitions;
- **Measured** — instrumented value with conditions;
- **Compared** — checked against golden unit or baseline;
- **Stress-tested** — passed declared duration/load matrix;
- **Integrated** — passed in final mechanical and cable configuration;
- **Release-accepted** — complete report and artifacts approved;
- **Open** — insufficient evidence.

## 41. Claims that must not be made from a basic self-test

A basic self-test does not prove:

- long-term stability;
- safe maximum current;
- power-rail margin under RF and audio load;
- microSD corruption resistance;
- calibrated touch accuracy;
- display controller identity;
- Bluetooth operation;
- enclosure thermal margin;
- antenna performance;
- production-batch consistency;
- environmental or ESD compliance;
- suitability for unattended deployment.

## 42. Completion criteria

The system bring-up and acceptance process is complete when:

1. sample identity and photographs are archived;
2. unpowered inspection passes;
3. rail resistance screening is recorded;
4. controlled first power-up passes;
5. USB enumeration and serial logging are repeatable;
6. manual and automatic bootloader entry pass;
7. validated upload speeds are recorded;
8. golden firmware and hashes are archived;
9. display patterns pass;
10. RGB, BOOT, RESET and LDR pass;
11. touch passes raw and calibrated tests;
12. microSD passes read/write/hash tests;
13. audio passes with verified load if required;
14. expansion pins pass the approved safe tests;
15. Wi-Fi/Bluetooth tests pass if required;
16. rail minima and reset behavior are captured;
17. thermal tests pass in intended enclosure;
18. combined-load matrix passes;
19. recovery from expected outages passes;
20. duration tests pass;
21. failures and deviations are resolved or approved;
22. report, logs and release artifacts are complete;
23. overall disposition is explicitly signed off.

## 43. Open questions

- What current-limit profile is appropriate for first power on this exact sample?
- What are acceptable idle and active current ranges?
- What rail minima are required under full load?
- What are the project-specific thermal limits?
- Which upload speed is repeatably reliable across hosts and cables?
- What display-controller identification method will be accepted?
- What touch residual error is acceptable?
- Which microSD cards and filesystems must be supported?
- What audio load and output level are required?
- What external peripherals define the expansion acceptance matrix?
- Which Wi-Fi and Bluetooth modes are release-critical?
- What duration test represents the real deployment?
- How many boards per batch must be sampled?
- Which tests can be automated by fixture?
- What deviations are acceptable for prototype versus production use?

## 44. Summary

Bring-up must progress from observation to measurement and from isolated subsystems to the integrated worst-case workload. The current repository self-test is a useful starting point because it exercises the TFT, RGB LED, LDR and BOOT input, while PlatformIO CI verifies that the firmware compiles. Neither result alone constitutes hardware acceptance.

A release-ready board requires traceable sample identity, controlled first power-up, repeatable programming, subsystem-specific tests, power and thermal evidence, recovery tests, final-enclosure validation, documented deviations and archived golden artifacts.