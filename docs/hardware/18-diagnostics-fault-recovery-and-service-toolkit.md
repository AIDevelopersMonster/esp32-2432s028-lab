# Diagnostics, Fault Recovery and Service Toolkit — ESP32-2432S028

> **Document status:** sample-specific diagnostic and service architecture for the author's `ESP32-2432S028` board.  
> **Scope:** diagnostic firmware, safe-mode entry, serial and on-screen service interfaces, subsystem isolation, recovery images, test fixtures, evidence capture, guided troubleshooting, controlled repair, and field-service procedures.  
> **Evidence rule:** a diagnostic message is evidence only when the test conditions, hardware sample, firmware revision, measured result, criterion, and retained log are known. A displayed `OK` string is not proof that the underlying subsystem was exercised correctly.

## 1. Purpose

The repository already contains a compact board self-test that initializes the display, cycles the RGB LED, reads the LDR, reports the BOOT button, and prints status through UART0. That program is useful for initial observation, but it is not yet a complete service instrument.

A service toolkit must answer more demanding questions:

- Can the board enter a known diagnostic state even when application firmware is broken?
- Which subsystem failed first?
- Is the fault permanent, intermittent, environmental, configuration-related, or external?
- Can the device preserve logs before rebooting?
- Can firmware, configuration, calibration, and user data be recovered independently?
- Can a technician repeat the same test and obtain comparable evidence?
- Can destructive operations be prevented unless explicitly authorized?
- Can the board be returned to service without hiding an unresolved fault?

This chapter defines the architecture and operating discipline for such a toolkit.

## 2. Relationship to earlier chapters

This chapter depends on the preceding system documentation:

- Part 5 defines power-path and brownout concerns;
- Parts 6–13 define subsystem-specific tests;
- Part 14 defines enclosure and service access;
- Part 15 defines pin ownership and conflicts;
- Part 16 defines bring-up and acceptance criteria;
- Part 17 defines lifecycle, maintenance, recovery assets, and requalification triggers.

The toolkit should reuse those verified procedures rather than inventing unrelated service behavior.

## 3. Current repository baseline

The current PlatformIO self-test performs these operations:

```text
Serial.begin(115200)
initialize backlight and RGB outputs
initialize LDR and BOOT inputs
initialize TFT
draw static status screen
cycle RGB channels
read LDR every 500 ms
read BOOT state every 500 ms
print LDR and BOOT state to UART0
```

This baseline establishes:

- a working development path;
- basic TFT initialization;
- basic RGB output control;
- basic LDR acquisition;
- basic BOOT input reading;
- basic serial output.

It does not yet establish:

- a machine-readable result protocol;
- retained fault records;
- reset-reason decoding;
- staged power diagnostics;
- touch, SD, audio, Wi-Fi, Bluetooth, or expansion tests;
- recovery-mode entry;
- safe destructive-operation controls;
- automated fixture integration;
- pass/fail limits;
- service authentication;
- a formal recovery image.

## 4. Diagnostic design principles

The toolkit should follow these principles.

### 4.1 Diagnose before modifying

The first service action should preserve evidence.

Do not immediately:

- erase Flash;
- format microSD;
- clear NVS;
- rewrite calibration;
- replace firmware;
- reflow components;
- disconnect all wiring without recording it;
- change power supplies without noting the original condition.

### 4.2 Separate observation from repair

A diagnostic test should not silently repair the condition being measured.

Example:

```text
BAD: mount fails -> automatically format card -> report card OK
GOOD: mount fails -> record failure -> offer explicit authorized format action
```

### 4.3 Prefer reversible actions

Initial tests should use:

- reads before writes;
- low current limits;
- conservative bus speeds;
- short duration;
- removable test loads;
- configuration backups;
- temporary overrides rather than permanent changes.

### 4.4 Preserve independent channels

If the TFT fails, UART diagnostics must remain useful.

If UART is inaccessible, on-screen status or LED codes may remain useful.

If application storage is corrupted, recovery firmware should not depend on that storage.

### 4.5 Report uncertainty

Use statuses such as:

```text
PASS
FAIL
WARN
SKIP
BLOCKED
NOT_PRESENT
NOT_CONFIGURED
UNSUPPORTED
INCONCLUSIVE
```

Do not convert an unperformed test into `PASS`.

## 5. Service modes

Recommended operating modes:

| Mode | Purpose | Write permission |
|---|---|---|
| NORMAL | ordinary application operation | application-defined |
| OBSERVE | collect health data without intrusive tests | no service writes |
| SAFE_DIAGNOSTIC | minimum-risk local tests | limited temporary writes |
| FULL_DIAGNOSTIC | complete subsystem test | declared test writes allowed |
| RECOVERY | restore firmware/configuration | authorized writes |
| FACTORY_TEST | production or fixture-driven test | controlled scripted writes |
| QUARANTINE | preserve evidence after serious fault | writes disabled by default |
| DECOMMISSION | clear credentials and data before disposal | explicit destructive writes |

The active mode should be visible in both serial output and on-screen UI.

## 6. Safe-mode entry paths

A serviceable design should provide more than one route into diagnostics.

Possible entry paths:

1. BOOT held during reset for a declared interval;
2. repeated failed-boot counter;
3. watchdog-reset threshold;
4. application-requested reboot into service mode;
5. serial command during an early boot window;
6. recovery flag stored in a protected configuration area;
7. fixture-controlled GPIO only after verifying pin safety;
8. ROM bootloader entry for firmware recovery.

### 6.1 BOOT-button service entry

GPIO0 is also a boot-strapping pin. A long press must be distinguished from the level sampled during reset.

A safe sequence may be:

```text
normal reset sampling completes
application starts
firmware waits briefly
BOOT remains pressed for declared duration
service mode is selected
```

Do not design a sequence that unintentionally traps the board in the ROM bootloader.

### 6.2 Failed-boot counter

A failed-boot counter should distinguish:

- power removal before startup completes;
- watchdog reset;
- brownout reset;
- panic reset;
- intentional software restart;
- firmware update restart.

Reset the counter only after the application reaches a defined healthy checkpoint.

### 6.3 Safe-mode minimum hardware

Safe mode should start with only essential resources:

- UART0 at a conservative baud rate;
- RGB LED off or in a known state;
- backlight in a defined state;
- no audio output;
- no SD writes;
- no Wi-Fi association until requested;
- no expansion output drive;
- no high-frequency stress test.

## 7. Boot-stage diagnostic markers

Use monotonic stage identifiers so a partial boot can be localized.

Example:

```text
B00 reset vector reached
B10 serial initialized
B20 reset reason recorded
B30 configuration header read
B40 safe GPIO states applied
B50 TFT initialization attempted
B60 local UI available
B70 storage checks complete
B80 network checks complete
B90 healthy checkpoint reached
```

The last retained marker helps identify where boot stopped.

### 7.1 Marker storage

Possible storage locations include:

- RTC memory for short reset sequences;
- NVS for persistent history;
- reserved program-Flash partition;
- microSD log when healthy;
- external service host log through UART.

Persistent writes must be rate-limited to avoid creating a diagnostic-induced wear problem.

## 8. Reset-reason capture

At every boot record:

```text
boot sequence number
reset reason per core where available
wake source
brownout indication
watchdog indication
panic or exception information
previous boot stage
firmware build ID
uptime before previous reset if retained
```

### 8.1 Reset classification

Recommended categories:

- POWER_ON;
- EXTERNAL_RESET;
- SOFTWARE_RESET;
- WATCHDOG_RESET;
- BROWNOUT_RESET;
- PANIC_RESET;
- DEEP_SLEEP_WAKE;
- UNKNOWN_RESET.

Keep the raw platform value in addition to the interpreted category.

## 9. Diagnostic result model

Each test result should include:

```text
test ID
test version
start timestamp or monotonic time
duration
board asset ID
firmware build ID
board profile version
mode
preconditions
stimulus
raw observations
calculated values
criterion
status
error code
operator or fixture ID
log reference
```

### 9.1 Stable test IDs

Use stable identifiers independent of UI wording.

Examples:

```text
PWR-5V-STATIC
PWR-3V3-STATIC
USB-ENUM
UART0-LOOP
TFT-COLOR-BARS
TOUCH-RAW-CORNERS
SD-READ-HASH
WIFI-SCAN
WIFI-DHCP
AUDIO-DIFF-LOAD
EXP-GPIO22-HIZ
```

### 9.2 Error-code structure

A structured code may use:

```text
SUBSYSTEM-STAGE-CAUSE
```

Examples:

```text
SD-MOUNT-NO_CARD
SD-READ-HASH_MISMATCH
WIFI-DHCP-TIMEOUT
TFT-INIT-NO_RESPONSE
USB-UART-FRAMING
PWR-3V3-UNDERVOLTAGE
```

Human-readable text may change; the code should remain stable.

## 10. Serial service console

UART0 is the primary universal diagnostic channel because it remains usable even when the TFT or touch subsystem fails.

### 10.1 Initial console configuration

Start with:

```text
115200 baud
8 data bits
no parity
1 stop bit
line-oriented UTF-8 or ASCII commands
bounded input length
command timeout
```

Higher rates may be supported after the basic channel is qualified.

### 10.2 Console banner

Print:

```text
product/profile name
asset ID
firmware version and hash
build date if reproducible policy permits
reset reason
active service mode
configuration version
available commands
write-protection state
```

### 10.3 Command categories

Recommended groups:

```text
info
health
test
log
config
backup
restore
network
storage
power
reboot
factory
```

### 10.4 Safe command parser

The parser should:

- reject overlong lines;
- reject ambiguous abbreviations;
- validate numeric ranges;
- require confirmation tokens for destructive actions;
- time out inactive authorization;
- never execute partial commands after serial noise;
- record every state-changing command.

### 10.5 Example read-only commands

```text
info board
info firmware
info reset
health summary
health power
log tail 50
test tft quick
test touch raw
test wifi scan
storage sd status
config show-redacted
```

### 10.6 Destructive command controls

Commands such as these must require explicit authorization:

```text
storage sd format
config erase
flash restore
factory reset
credentials clear
auxflash write
```

Require a generated challenge or exact confirmation phrase rather than a single `y`.

## 11. On-screen service console

The TFT can provide local diagnostics when a serial host is unavailable.

### 11.1 Minimum screen

Show:

- service mode;
- firmware build ID;
- reset reason;
- test progress;
- overall status;
- first active fault;
- instructions for serial connection;
- QR code only if its content is safe and useful.

### 11.2 Touch-independent navigation

Because touch may be the failed subsystem, critical service navigation should not depend exclusively on touch.

Alternatives:

- BOOT-button short/long press;
- timed page rotation;
- serial commands;
- fixture control;
- restart with selected mode flag.

### 11.3 Screen-failure fallback

If TFT initialization fails:

- keep UART active;
- use RGB fault codes if safe;
- continue non-display tests;
- record the TFT failure rather than blocking all diagnostics.

## 12. RGB diagnostic codes

The RGB LED can indicate early boot state before the display is ready.

Example policy:

| Indication | Meaning |
|---|---|
| brief blue pulse | diagnostic firmware started |
| slow blue blink | waiting for console or operator |
| green steady | selected quick test passed |
| yellow blink | warning or incomplete test |
| red blink pattern | fault code group |
| LED dark | intentional safe idle or power/firmware failure |

The exact code table must be versioned and printed in service documentation.

### 12.1 Active-LOW caution

The repository treats all RGB channels as active LOW. Initialization must write the inactive state before enabling output behavior.

### 12.2 Strap-pin caution

GPIO4 is strap-sensitive and drives the red channel. Do not use an external low-impedance fixture connection that changes its reset-time state.

## 13. Diagnostic firmware architecture

Recommended layers:

```text
hardware profile
safe GPIO initialization
hardware abstraction
measurement drivers
subsystem tests
result engine
log store
serial console
screen UI
fixture protocol
recovery actions
policy and authorization layer
```

### 13.1 Hardware profile

The profile should contain:

- GPIO assignments;
- active levels;
- input-only restrictions;
- strap-sensitive flags;
- bus ownership;
- connector labels;
- expected device identities;
- declared test limits;
- unsupported features.

### 13.2 Test isolation

Each subsystem test should expose:

```text
prepare()
run()
collect()
evaluate()
cleanup()
```

Cleanup should return outputs and buses to a defined safe state.

### 13.3 Dependency declaration

A test should state its dependencies.

Example:

```text
WIFI-NTP depends on:
  PWR-3V3-DYNAMIC
  WIFI-SCAN
  WIFI-ASSOC
  WIFI-DHCP
  DNS-RESOLVE
```

If DHCP fails, NTP should be `BLOCKED`, not independently marked `FAIL` without context.

## 14. Quick, standard, and extended suites

### 14.1 QUICK

Target: a few minutes.

Typical coverage:

- identity;
- reset reason;
- basic power reading where instrumentation exists;
- USB/UART communication;
- TFT basic pattern;
- RGB sequence;
- BOOT input;
- LDR plausibility;
- touch presence;
- SD card presence;
- Wi-Fi scan.

### 14.2 STANDARD

Target: service bench verification.

Adds:

- repeated resets;
- touch corners and pressure;
- SD read/hash and controlled write;
- Wi-Fi association, DHCP, DNS, NTP, reconnect;
- audio differential test with approved load;
- expansion-input checks;
- short thermal observation;
- log export.

### 14.3 EXTENDED

Target: requalification or intermittent fault investigation.

Adds:

- long-duration combined load;
- repeated power cycles;
- SD stress with bounded test area;
- network loss and recovery;
- temperature trend;
- connector movement observation;
- enclosure comparison;
- repeated touch matrix;
- fault injection where safe.

## 15. Power diagnostics

### 15.1 Required external instruments

Firmware alone cannot establish all power conditions.

Useful instruments:

- current-limited bench supply;
- digital multimeter;
- oscilloscope;
- differential probe where required;
- current probe or shunt;
- thermal camera or contact probe.

### 15.2 Firmware-visible power evidence

Possible firmware evidence includes:

- reset reason;
- brownout events;
- boot count;
- failure under known load;
- ADC readings only if a verified measurement path exists;
- regulator temperature only if an actual sensor or external measurement is used.

Do not invent rail telemetry from unrelated ADC pins.

### 15.3 Power test stages

```text
P0 unpowered resistance and inspection
P1 current-limited power with no external loads
P2 idle firmware
P3 TFT/backlight load
P4 Wi-Fi load
P5 SD write load
P6 audio load
P7 combined declared maximum load
```

### 15.4 Fault signatures

| Symptom | Possible causes |
|---|---|
| immediate current-limit hit | short, reversed connection, damaged regulator |
| boots with backlight off only | supply drop, backlight path overload, regulator heating |
| resets on Wi-Fi transmit | cable drop, regulator headroom, rail transient |
| resets on SD write | card current pulse, rail impedance, filesystem fault |
| audio causes reset | invalid speaker load, amplifier current, supply weakness |
| temperature rises over time | regulator dissipation, enclosure airflow, overload |

## 16. USB and UART diagnostics

### 16.1 Layered checks

1. USB power present;
2. host detects USB device;
3. CH340G driver binds;
4. serial port opens;
5. ESP32 boot text is received;
6. application console responds;
7. upload handshake works;
8. repeated upload succeeds at qualified speed.

### 16.2 UART loop testing

A direct loopback may be performed only with a fixture and topology that does not create contention with the ESP32 or CH340G.

Safer options:

- host sends a framed command and verifies the application response;
- test application echoes a nonce;
- fixture observes TX/RX timing through high-impedance inputs.

### 16.3 P1 interaction

Because P1 shares UART0, test separately:

- USB only;
- P1 external adapter only if safely powered;
- both attached with transmitters controlled;
- board powered from USB;
- board powered from approved external 5 V source.

Never connect two actively driven TX outputs together.

## 17. TFT diagnostic suite

Recommended tests:

- initialization result;
- controller readback where supported;
- solid red, green, blue, white, black;
- color bars;
- checkerboard;
- pixel-address boundaries;
- orientation markers;
- text rendering;
- repeated full-screen writes;
- optional readback comparison;
- backlight on/off and PWM only after topology verification.

### 17.1 Visual versus automated evidence

Visual inspection can identify:

- dead regions;
- color swap;
- rotation error;
- flicker;
- uneven backlight;
- intermittent contact.

Automated pixel correctness requires a valid readback path or external imaging system.

### 17.2 Display-safe fallback

Do not repeatedly reinitialize the display at uncontrolled rates when investigating a fault. Record SPI and control-line evidence first.

## 18. Touch diagnostic suite

Tests should separate:

- controller communication;
- IRQ behavior;
- raw X/Y/Z values;
- coordinate stability;
- corner reach;
- pressure threshold;
- calibration transform;
- rotation mapping;
- enclosure preload;
- false-touch rate.

### 18.1 Raw mode

Raw mode should report samples without applying calibration.

### 18.2 Calibration mode

Calibration writes must be explicit and versioned.

Record:

```text
board asset ID
display rotation
touch-driver version
raw corner samples
pressure thresholds
calibration coefficients
enclosure state
operator/date
```

### 18.3 Touch isolation

When touch fails:

1. disconnect or disable unrelated SPI activity where possible;
2. check IRQ idle level;
3. verify CS behavior;
4. lower touch SPI clock;
5. observe raw values without screen updates;
6. compare bare-board and enclosed behavior.

## 19. microSD diagnostic suite

Recommended stages:

1. card-detect observation if a verified signal exists;
2. initialize at conservative SPI speed;
3. report card type and capacity;
4. list root directory;
5. read a known file;
6. verify a stored hash;
7. write a uniquely named temporary file;
8. flush and close;
9. remount;
10. read and hash the temporary file;
11. delete only the diagnostic file;
12. report filesystem and timing data.

### 19.1 Non-destructive default

The default service test must not format the card or overwrite user files.

### 19.2 Known test object

Use a versioned test file with known:

- size;
- content generator;
- SHA-256;
- expected directory;
- creation policy.

### 19.3 Failure separation

Distinguish:

- no card;
- electrical initialization failure;
- unsupported card behavior;
- filesystem mount failure;
- file not found;
- read failure;
- write failure;
- hash mismatch;
- removal during operation.

## 20. Program-Flash diagnostics

Safe checks include:

- chip identity;
- detected size;
- partition table parse;
- application image metadata;
- OTA slot state;
- NVS initialization status;
- filesystem mount status;
- checksum of recovery-critical files.

### 20.1 Readback preservation

Before partition changes or uncertain recovery:

- capture full Flash image;
- record image size;
- compute SHA-256;
- record tool and command versions;
- keep the original read-only copy.

### 20.2 Flash-write caution

Repeated diagnostic writes can consume endurance. Store high-frequency logs in RAM or aggregate them before persistent commit.

## 21. Auxiliary W25Q32 diagnostics

The board contains a photo-confirmed W25Q32JV-class auxiliary Flash whose routing and purpose remain open.

The first diagnostic sequence should be read-only:

1. establish pin mapping by continuity or controlled observation;
2. verify supply and idle states;
3. read JEDEC ID;
4. read status registers;
5. read multiple address regions;
6. capture the complete image;
7. calculate SHA-256;
8. repeat read and compare hashes.

Do not erase, program, alter protection bits, or enable quad mode before preserving the original contents.

## 22. RGB LED and LDR diagnostics

### 22.1 RGB LED

Test each channel independently:

- set all channels off first;
- activate red only;
- activate green only;
- activate blue only;
- activate combinations;
- verify active-LOW behavior;
- observe current if required;
- restore all channels off.

### 22.2 LDR

A useful test requires at least two light conditions.

Record:

```text
dark or covered reading
ambient reading
illuminated reading
noise range
sample interval
ADC configuration
```

Do not declare sensor polarity until the response direction is measured.

### 22.3 Optical crosstalk

Repeat LDR readings with:

- TFT backlight off/on;
- RGB channels off/on;
- enclosure open/closed.

## 23. BOOT and RESET diagnostics

### 23.1 BOOT

Test:

- released level;
- pressed level;
- debounce behavior;
- short press;
- long press;
- press during application runtime;
- controlled reset-time behavior.

### 23.2 RESET

Test:

- manual reset;
- serial-log restart evidence;
- display restart;
- preserved reset reason;
- no stuck external plunger;
- repeated operation.

### 23.3 Service-entry conflict

Ensure the service-mode gesture cannot accidentally invoke factory reset or destructive recovery.

## 24. Audio diagnostic suite

The 8002A output is differential BTL. Neither speaker output should be treated as ground.

### 24.1 Preconditions

Before enabling audio:

- verify approved speaker impedance;
- verify both speaker wires;
- confirm no output terminal is grounded;
- use differential measurement;
- start at low duty/amplitude;
- confirm amplifier temperature.

### 24.2 Test signals

Possible bounded tests:

- silence baseline;
- low-amplitude tone;
- frequency sweep within declared limits;
- stepped amplitude;
- left/right terminology should not be used for a mono path;
- distortion or output-power measurement only with proper instruments.

### 24.3 Fault separation

Distinguish:

- no source waveform on GPIO26;
- amplifier not enabled or unpowered;
- one output open;
- invalid speaker;
- enclosure rattle;
- supply collapse;
- thermal shutdown;
- software timer conflict.

## 25. Wi-Fi diagnostic suite

Use independent stages:

```text
radio initialization
scan
SSID visibility
association
authentication
DHCP
gateway reachability
DNS resolution
NTP or application-service access
sustained traffic
reconnect
```

### 25.1 Scan report

Record:

- SSID with privacy policy applied;
- BSSID only when needed and protected;
- channel;
- RSSI;
- security mode;
- scan duration;
- board orientation;
- enclosure state.

### 25.2 Credential handling

Diagnostic logs must not print plaintext passwords or private keys.

### 25.3 Local-only fallback

The toolkit should remain useful without Internet access. Scan, association, DHCP, local gateway, and local test-server checks can be performed independently.

### 25.4 RF comparison

For enclosure or degradation diagnosis, compare against a controlled baseline:

- same access point;
- same distance;
- same orientation;
- same channel;
- same firmware;
- same power source;
- same test duration.

## 26. Bluetooth diagnostic suite

Bluetooth tests should run only when required by the application.

Possible checks:

- controller initialization;
- device address reporting;
- BLE advertising;
- BLE scan;
- GATT read/write with a known peer;
- Bluetooth Classic discovery or link where used;
- coexistence with Wi-Fi;
- reconnect after peer loss.

A module-family capability is not equivalent to a board-level qualified application.

## 27. Expansion-interface diagnostics

### 27.1 GPIO35

GPIO35 is input-only and requires external bias when a defined idle state is needed.

Safe tests:

- high-impedance voltage observation;
- externally driven 0/3.3-V through current-limited fixture;
- interrupt count;
- ADC reading if appropriate.

Never configure it as an output test.

### 27.2 GPIO22 and GPIO27

Begin with:

- unpowered resistance checks;
- boot-time observation;
- input-only monitoring;
- controlled drive through series resistance;
- low-frequency toggling;
- interaction checks with display, SD, audio, Wi-Fi, and boot.

### 27.3 GPIO21

GPIO21 is the current TFT backlight control and is not a free generic test pin.

### 27.4 Connector power

Before powering an external fixture from CN1 3.3 V or P1 5 V, verify current demand, return path, back-powering behavior, and regulator thermal margin.

## 28. SPI isolation strategy

The board uses separate pin groups for TFT, touch, and microSD, but the ESP32 provides a limited number of general-purpose SPI controllers.

Diagnostic firmware should explicitly document:

- controller assignment;
- pin assignment;
- transaction frequency;
- SPI mode;
- chip-select idle state;
- library ownership;
- cleanup behavior.

### 28.1 One-at-a-time mode

A valuable isolation mode initializes only one SPI subsystem at a time.

### 28.2 Combined mode

After independent success, run:

- TFT + touch;
- TFT + SD;
- touch + SD;
- TFT + touch + SD;
- combined operation with Wi-Fi.

### 28.3 Conflict indicators

- display corruption when SD starts;
- touch readings fail during TFT writes;
- SD mount changes after display initialization;
- chip selects overlap;
- one library calls global `SPI.begin()` and remaps pins.

## 29. I2C diagnostic strategy

No default free I2C pair should be assumed.

The candidate pair GPIO22/GPIO27 requires sample-specific verification.

A bus test should include:

- pull-up voltage;
- pull-up resistance;
- idle levels;
- clock and data rise time;
- address scan;
- stuck-low recovery;
- interaction with Wi-Fi if GPIO27 ADC or other use is considered;
- backlight independence from GPIO21.

## 30. Fault-injection tests

Fault injection is useful only when bounded and reversible.

Possible safe injections:

- remove microSD before boot;
- use invalid Wi-Fi credentials;
- disable access point temporarily;
- block DNS while preserving local network;
- disconnect approved speaker;
- cover LDR;
- press BOOT during runtime;
- force a controlled software restart;
- use a weak but still safe USB cable for comparison;
- introduce known file-hash mismatch in a disposable test file.

Do not inject overvoltage, output contention, antenna damage, hard shorts, or unsafe thermal stress.

## 31. Log architecture

### 31.1 Log channels

Recommended channels:

- live UART text;
- machine-readable UART frames;
- on-screen summary;
- retained boot/event ring buffer;
- microSD export when healthy;
- host-side captured session file.

### 31.2 Severity levels

```text
TRACE
DEBUG
INFO
WARN
ERROR
FATAL
AUDIT
```

`AUDIT` should identify state-changing service actions.

### 31.3 Required context

Every log session should start with:

```text
asset ID
firmware build ID
board profile version
configuration version
reset reason
service mode
uptime origin
time synchronization status
```

### 31.4 Secret redaction

Never log:

- plaintext Wi-Fi passwords;
- private keys;
- full authentication tokens;
- recovery secrets;
- personal data not required for diagnosis.

### 31.5 Ring-buffer behavior

Use bounded logs. Define:

- maximum records;
- overwrite policy;
- persistence policy;
- wear-limiting behavior;
- crash-safe record format;
- export and clear procedures.

## 32. Machine-readable service protocol

A fixture-friendly protocol may use JSON Lines, CBOR, or another framed representation.

Example conceptual record:

```json
{"type":"test_result","id":"WIFI-DHCP","status":"PASS","duration_ms":842,"value":{"address":"redacted"},"build":"<hash>"}
```

### 32.1 Protocol requirements

- framing survives partial reads;
- each record has a schema version;
- command and response IDs correlate;
- timeouts are explicit;
- binary payloads are bounded;
- invalid input cannot trigger writes;
- human-readable console and fixture protocol do not interfere.

## 33. Diagnostic bundle export

A service bundle should contain:

```text
manifest
board identity
firmware and profile versions
configuration with secrets redacted
reset history
test results
selected raw measurements
serial log
screen photographs if relevant
power and oscilloscope captures
thermal images if relevant
operator notes
hash list
```

### 33.1 Bundle integrity

Generate a hash list and retain the original unmodified bundle.

### 33.2 Privacy

Before sharing externally, review:

- SSIDs;
- MAC addresses;
- IP addresses;
- credentials;
- user files;
- location data;
- personal identifiers.

## 34. Recovery-image hierarchy

Maintain distinct recovery artifacts:

1. ROM serial bootloader — silicon-provided minimal recovery path;
2. known-good diagnostic application image;
3. known-good full program-Flash image;
4. partition-table and bootloader artifacts;
5. configuration backup;
6. calibration backup;
7. application-data backup;
8. auxiliary-Flash original image;
9. microSD content backup where required.

Do not treat these artifacts as interchangeable.

## 35. Golden diagnostic firmware

A golden diagnostic build should be:

- versioned;
- reproducibly buildable where practical;
- stored as source and binary;
- hashed;
- associated with a board-profile version;
- tested on known-good hardware;
- free of private credentials;
- able to run without Internet access;
- conservative in clock speeds and loads;
- read-only by default.

### 35.1 Golden-image record

```text
source commit
binary filename
binary SHA-256
build environment
platform version
framework version
library versions
partition table
Flash mode and size
supported board revision
known limitations
validation boards
```

## 36. Firmware recovery procedure

Recommended sequence:

1. photograph and identify the board;
2. capture current boot log;
3. try normal diagnostic entry;
4. try manual ROM bootloader entry;
5. read chip and Flash identity;
6. back up the full current Flash if readable;
7. hash the backup;
8. preserve existing partition-table information;
9. flash the minimal golden diagnostic image using declared addresses;
10. verify written data;
11. boot without external peripherals;
12. run QUICK diagnostics;
13. restore configuration or application only after hardware health is understood;
14. document every write.

### 36.1 Failed recovery

If ROM bootloader communication fails, investigate:

- USB cable and host;
- CH340G enumeration;
- EN and GPIO0 behavior;
- UART0 contention through P1;
- board power;
- Flash rail and module damage;
- physical connector damage.

Do not repeatedly erase Flash when the transport itself is unreliable.

## 37. Configuration recovery

Configuration should be restorable independently of firmware.

Recommended elements:

- schema version;
- migration path;
- default values;
- CRC or hash;
- redundant copy if justified;
- secret handling;
- export/import tools;
- rollback copy;
- validation before activation.

### 37.1 Corrupt configuration behavior

On invalid configuration:

- preserve the corrupt copy;
- start safe diagnostic mode;
- report the validation error;
- offer read-only export;
- load defaults only through an explicit policy;
- do not silently overwrite evidence.

## 38. Calibration recovery

Treat display orientation, touch coefficients, LDR interpretation, and any application sensor calibration as versioned data.

A calibration backup should include:

```text
asset ID
calibration type
algorithm version
raw data
coefficients
units
conditions
enclosure state
firmware compatibility
```

Replacement boards require new sample-specific calibration unless equivalence is demonstrated.

## 39. microSD recovery

Before repair:

- image the card where data matters;
- use read-only host access if possible;
- calculate image hash;
- preserve filesystem metadata;
- separate card faults from board socket faults;
- test a known-good disposable card independently.

Formatting is a recovery choice, not a diagnostic default.

## 40. Network recovery

Network recovery should distinguish:

- wrong credentials;
- unavailable SSID;
- unsupported band;
- authentication failure;
- DHCP failure;
- DNS failure;
- time-service failure;
- certificate/time problem;
- server outage;
- antenna/enclosure degradation.

A factory reset should not be the first response to a temporary access-point outage.

## 41. Safe factory reset

A factory reset policy must define exactly what is cleared:

- user configuration;
- Wi-Fi credentials;
- application data;
- logs;
- touch calibration;
- asset identity;
- lifecycle history;
- security keys;
- auxiliary-Flash contents;
- microSD files.

Asset identity and service history usually should not be erased by an ordinary user reset.

### 41.1 Confirmation sequence

Require:

- authenticated service mode or physical presence;
- explicit scope display;
- challenge confirmation;
- backup option;
- audit record;
- reboot into verification mode.

## 42. Diagnostic test fixture

A bench fixture can improve repeatability.

Potential fixture elements:

- current-limited 5-V source;
- USB data connection;
- switchable UART adapter;
- pogo pins only on verified safe pads;
- controllable BOOT/RESET actuators;
- known speaker load or differential audio load;
- known microSD card;
- controlled light source for LDR;
- touch actuator only if force is controlled;
- Wi-Fi test access point;
- temperature probes;
- fixture controller and log capture.

### 42.1 Fixture safety

The fixture must not:

- drive input-only pins as outputs;
- force strap pins during reset unintentionally;
- connect UART transmitters together;
- ground either BTL speaker output;
- exceed 3.3-V logic levels;
- back-power the board through GPIO;
- block the antenna without documenting it;
- press on the touch panel outside controlled limits.

## 43. Fixture identification

Record:

```text
fixture ID
fixture revision
wiring revision
software version
instrument serial numbers
calibration dates
known limitations
operator
```

A result without fixture revision can be difficult to reproduce.

## 44. Guided troubleshooting tree

Recommended top-level branches:

```text
A no power
B no boot
C no USB/serial
D display failure
E touch failure
F storage failure
G audio failure
H network failure
I reset/instability
J thermal problem
K expansion/peripheral fault
L mechanical/environmental damage
```

### 44.1 No power

Check in order:

1. source voltage;
2. cable continuity;
3. connector damage;
4. unpowered resistance;
5. current-limited startup behavior;
6. 5-V rail;
7. regulator input/output;
8. shorted load domain;
9. thermal anomaly.

### 44.2 No boot

Check:

1. power stability;
2. EN level;
3. GPIO0 state;
4. other strap-pin loading;
5. UART boot log;
6. ROM bootloader response;
7. program-Flash identity;
8. external connector influence;
9. module damage.

### 44.3 Display failure

Check:

1. backlight versus image generation;
2. power rails;
3. TFT SPI pin map;
4. CS/DC waveform;
5. reset topology;
6. library profile;
7. display flex and mechanical pressure;
8. alternate known-good firmware.

### 44.4 Intermittent reset

Check:

1. reset reason;
2. input voltage at event;
3. 3.3-V transient;
4. Wi-Fi/SD/audio correlation;
5. regulator temperature;
6. USB movement;
7. enclosure closure;
8. watchdog logs;
9. external peripheral current.

## 45. Binary-search isolation

For complex faults, disable half the active subsystems and repeat.

Example:

```text
full application fails
-> disable audio and SD
-> if stable, re-enable one
-> if unstable, disable Wi-Fi and TFT updates
-> continue until the smallest reproducing set is found
```

Keep the firmware build and test conditions recorded for each step.

## 46. Known-good substitutions

Useful controlled substitutions include:

- known-good USB cable;
- known-good host port;
- known-good microSD card;
- known-good speaker load;
- known-good power source;
- known-good board with same verified revision;
- known-good firmware binary;
- known-good access point.

Change one variable at a time.

## 47. Intermittent-fault capture

Intermittent faults require triggers and pre-event context.

Capture:

- reset reason;
- last boot stage;
- recent event ring;
- subsystem state;
- network state;
- SD operation state;
- audio state;
- uptime;
- temperature if actually measured;
- supply waveform with oscilloscope trigger where possible.

### 47.1 Trigger examples

- 3.3-V rail below declared threshold;
- EN pulse;
- UART panic text;
- USB disconnect;
- SD error counter increment;
- Wi-Fi disconnect burst;
- watchdog warning;
- temperature threshold from external logger.

## 48. Crash and panic capture

Where framework support permits, retain:

- exception cause;
- program counter;
- backtrace;
- task name;
- free heap;
- stack high-water information;
- reset reason;
- firmware hash;
- recent event IDs.

Do not promise symbolized analysis unless the exact ELF and build artifacts are retained.

## 49. Watchdog diagnostics

Distinguish:

- task watchdog;
- interrupt watchdog;
- application timeout mistaken for watchdog;
- brownout causing restart before a watchdog report.

Tests should identify which task or operation stopped making progress.

Avoid “fixing” watchdog resets by simply increasing every timeout without finding the blocked path.

## 50. Brownout diagnostics

A brownout investigation should correlate:

- reset reason;
- input supply voltage;
- 3.3-V waveform;
- load event;
- USB cable and source;
- regulator temperature;
- enclosure state;
- external peripheral load.

Disabling brownout detection is not a valid repair for an inadequate power system.

## 51. Thermal fault diagnostics

Record:

- ambient temperature;
- enclosure state;
- board orientation;
- workload;
- power source;
- U1 temperature;
- U7 temperature;
- ESP-WROOM-32 shield temperature;
- audio amplifier temperature;
- time to steady state;
- fault onset temperature and time.

Use safe external measurements. No onboard temperature sensor is assumed for regulator qualification.

## 52. Mechanical fault diagnostics

Inspect for:

- cracked PCB;
- lifted USB connector;
- loose microSD socket;
- bent connector pins;
- cable strain;
- enclosure pressure on touch panel;
- stuck BOOT or RESET plunger;
- loose fasteners;
- speaker vibration;
- corrosion;
- conductive debris;
- antenna obstruction.

Photograph before moving suspect parts.

## 53. Liquid and contamination response

After suspected liquid exposure:

1. remove power;
2. do not repeatedly attempt startup;
3. disconnect external power sources;
4. photograph contamination;
5. preserve removable storage;
6. follow an approved cleaning and drying process;
7. inspect under magnification;
8. measure unpowered resistance;
9. use current-limited first power-up;
10. quarantine until full requalification.

## 54. Field-service package

A field technician should receive:

- board identification guide;
- connector orientation guide;
- approved cables and adapters;
- golden diagnostic binary;
- flashing instructions;
- console-command reference;
- safe-mode instructions;
- fault-code table;
- test checklist;
- backup/export procedure;
- escalation criteria;
- ESD and BTL-audio cautions;
- replacement and return labels.

## 55. Field-service levels

| Level | Allowed work |
|---|---|
| S0 | observation and log capture |
| S1 | cable, card, approved peripheral substitution |
| S2 | diagnostic firmware and configuration restore |
| S3 | enclosure opening and board replacement |
| S4 | component-level rework by qualified technician |
| S5 | engineering failure analysis |

Authorization and training should match the service level.

## 56. Repair versus replacement

Replacement is preferable when:

- PCB is cracked;
- repeated intermittent faults remain unlocalized;
- corrosion is extensive;
- critical connector pads are damaged;
- module or display replacement exceeds economic limits;
- safety or reliability evidence cannot be restored.

Repair may be reasonable when:

- the fault is localized;
- the repair process is controlled;
- required parts are identified;
- post-repair requalification is available;
- repair history is retained.

## 57. Rework documentation

Record:

```text
asset ID
fault report
pre-rework photographs
diagnosis
affected components or nets
removed parts
replacement part identity
materials and process
operator
date
inspection result
cleaning result
post-rework tests
requalification level
remaining limitations
```

## 58. Return-to-service gate

A repaired or recovered board should not return directly to deployment.

Require:

1. fault cause documented or explicitly unresolved;
2. recovery actions recorded;
3. configuration and credentials validated;
4. QUICK suite passed;
5. affected subsystem STANDARD tests passed;
6. relevant combined-load regression passed;
7. enclosure and connector checks passed;
8. lifecycle state updated;
9. service report linked to the asset;
10. unresolved risks accepted by an authorized owner.

## 59. Quarantine criteria

Quarantine the board when:

- identity is uncertain;
- liquid exposure occurred;
- unexplained overcurrent occurred;
- repeated brownouts remain unresolved;
- Flash contents must be preserved for analysis;
- unsafe external voltage was applied;
- component temperature exceeded a declared limit;
- structural damage is visible;
- credentials may be compromised;
- repair was performed but requalification is incomplete.

## 60. Service security

Diagnostic interfaces can expose sensitive functions.

Controls may include:

- physical-presence requirement;
- authenticated service session;
- signed recovery images where supported;
- command authorization levels;
- time-limited destructive permissions;
- audit logging;
- secret redaction;
- disabled remote console by default;
- network service available only on trusted local interfaces.

Do not expose an unauthenticated remote factory-reset or Flash-write command.

## 61. Remote diagnostics

Remote collection may report:

- firmware version;
- reset count and reasons;
- uptime;
- application health counters;
- Wi-Fi status;
- storage error counts;
- last maintenance date;
- non-sensitive test summaries.

Remote diagnostics must not replace local electrical inspection for power, thermal, connector, or contamination faults.

## 62. Service API versioning

Version:

- command set;
- result schema;
- error codes;
- test definitions;
- hardware profile;
- recovery-image format;
- configuration schema.

A host tool should reject incompatible major versions rather than silently misinterpret data.

## 63. Automation and CI

Repository CI currently verifies that the PlatformIO firmware builds. Future diagnostic CI can additionally verify:

- all test IDs are unique;
- result-schema examples validate;
- command parser tests pass;
- destructive commands require authorization;
- secrets are absent from test fixtures;
- pin conflicts are statically checked;
- golden binary hash is generated;
- documentation references existing tests;
- host-side parsers handle incomplete frames.

CI cannot replace hardware execution.

## 64. Host-side service utility

A future host utility may provide:

- serial-port discovery;
- device identity readout;
- command execution;
- live log capture;
- diagnostic-suite orchestration;
- recovery flashing;
- binary backup;
- hash verification;
- report generation;
- fixture control.

### 64.1 Host utility safety

The utility should:

- show the selected port and asset ID;
- refuse ambiguous multiple-device selection;
- display exact write addresses;
- preserve backups before erase;
- validate image hashes;
- record tool versions and commands;
- keep secrets outside logs.

## 65. Service report template

```text
Service report ID:
Date/time:
Operator:
Service level:
Asset ID:
PCB marking/revision:
Enclosure/application:
Reported symptom:
Conditions at failure:
Power source/cable:
External peripherals:
Firmware build ID:
Configuration version:
Reset reason/history:

Initial visual inspection:
Initial unpowered measurements:
Initial boot/serial evidence:
Diagnostic mode used:
Tests executed:
Raw measurements:
Fault codes:
Substitution tests:
Root cause:
Confidence:

Backups captured:
Backup hashes:
Recovery or repair actions:
Parts replaced:
Configuration/calibration restored:
Post-action tests:
Regression tests:
Remaining warnings:
Lifecycle state:
Return-to-service decision:
Approver:
Evidence bundle location/hash:
```

## 66. Diagnostic test record template

```text
Test ID:
Test version:
Asset ID:
Firmware build:
Board profile:
Fixture ID/revision:
Instrument IDs:
Mode:
Preconditions:
Stimulus:
Start time:
Duration:
Raw observations:
Calculated value:
Criterion:
Status:
Error code:
Cleanup result:
Log reference:
Operator:
Notes:
```

## 67. Recovery record template

```text
Asset ID:
Recovery reason:
Original firmware identity:
Original full-Flash backup:
Original backup SHA-256:
Original partition table:
Auxiliary-Flash backup:
Configuration backup:
Calibration backup:
Recovery image:
Recovery image SHA-256:
Tool and version:
Exact write addresses:
Verification result:
Post-recovery firmware ID:
Post-recovery diagnostics:
Data restored:
Data intentionally cleared:
Operator/date:
Approver:
```

## 68. Evidence levels

Use explicit labels:

- **Console-observed** — message seen once;
- **Protocol-captured** — machine-readable record retained;
- **Instrument-measured** — external measurement retained;
- **Repeated** — repeated on one sample;
- **Fixture-verified** — passed on a controlled fixture;
- **Multi-sample verified** — passed on multiple boards;
- **Recovery-verified** — backup and restore both demonstrated;
- **Field-service verified** — procedure successfully used outside the development bench;
- **Open** — not yet demonstrated.

## 69. Claims that must not be made yet

Until implemented and tested, do not claim:

- automatic recovery from every firmware fault;
- complete hardware diagnosis from firmware alone;
- retained logs across every power-loss scenario;
- secure remote service access;
- safe unattended factory reset;
- verified auxiliary-Flash recovery;
- automated TFT pixel verification;
- calibrated onboard power or temperature telemetry;
- safe arbitrary GPIO fixture access;
- universal compatibility across CYD revisions;
- guaranteed recovery of corrupted microSD data;
- component-level fault localization without instruments;
- production-ready fixture coverage;
- field-service readiness.

## 70. Completion criteria

The diagnostic and service toolkit can be considered sample-qualified when:

1. a versioned diagnostic firmware exists;
2. a golden binary and SHA-256 are retained;
3. the build/toolchain manifest is retained;
4. normal, safe-diagnostic, full-diagnostic, and recovery modes are distinguishable;
5. at least two independent diagnostic entry paths work;
6. reset reason and boot-stage markers are captured;
7. UART console works at a conservative qualified baud rate;
8. on-screen summary works without requiring touch;
9. stable test IDs and result statuses are implemented;
10. machine-readable result export works;
11. QUICK suite is automated or procedurally controlled;
12. TFT, touch, SD, RGB, LDR, BOOT/RESET, audio, Wi-Fi, and expansion tests are separately documented;
13. destructive actions require explicit authorization;
14. logs redact secrets;
15. program-Flash backup and restore are demonstrated on a test board;
16. configuration and calibration export/import are demonstrated;
17. auxiliary-Flash read-only backup is demonstrated before write support is considered;
18. a diagnostic bundle can be exported and hash-verified;
19. fixture wiring and revision are documented;
20. no fixture output violates input-only or strap-pin restrictions;
21. audio fixture preserves BTL isolation;
22. fault-injection tests demonstrate expected detection and recovery;
23. intermittent-fault logging captures pre-reset context;
24. service and recovery reports are linked to asset identity;
25. repaired boards pass the return-to-service gate;
26. security and authorization behavior is reviewed;
27. procedures are exercised by someone other than the original developer;
28. all unsupported claims remain explicit.

## 71. Open implementation tasks

- Refactor the current self-test into independent test modules.
- Add firmware/build identity to serial and TFT output.
- Add reset-reason decoding.
- Add boot-stage markers and a bounded event ring.
- Define the service-mode BOOT-button gesture.
- Define the serial command grammar.
- Define stable test IDs and error codes.
- Add JSON Lines or another machine-readable result mode.
- Add touch raw/calibration service pages.
- Add non-destructive SD read/hash/write test.
- Add Wi-Fi staged diagnostics without exposing credentials.
- Add bounded audio test with explicit BTL warnings.
- Add program-Flash backup instructions and golden image manifest.
- Map and preserve auxiliary W25Q32 contents.
- Design a safe fixture for BOOT, RESET, light, audio and connector tests.
- Add host-side log capture and report generation.
- Add parser and policy tests to CI.
- Perform recovery drills on a sacrificial or fully backed-up board.
- Validate field-service instructions with an independent operator.

## 72. Summary

The service toolkit should transform the current visual self-test into a controlled diagnostic system with safe entry modes, independent communication channels, stable test identifiers, retained evidence, explicit authorization for destructive actions, and verified recovery assets.

Its central discipline is:

> preserve evidence, isolate one subsystem at a time, distinguish blocked tests from failed tests, and verify recovery before declaring the board serviceable.

A board is not recovered merely because it boots again. It is recovered only when the cause, actions, configuration, evidence, regression results, and remaining risks are documented.