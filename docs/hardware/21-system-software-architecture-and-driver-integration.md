# System Software Architecture and Driver Integration — ESP32-2432S028

> **Document status:** sample-specific software architecture and integration plan for the author's `ESP32-2432S028` board.  
> **Scope:** hardware abstraction, board profiles, subsystem ownership, initialization phases, task model, bus arbitration, event handling, configuration schemas, diagnostics, portability, testing, and migration from the current single-file self-test.  
> **Evidence rule:** an architectural pattern is not considered implemented merely because it appears in this document. Every claimed software capability requires source code, repeatable builds, hardware tests, versioned configuration, and retained evidence.

## 1. Purpose

The current repository demonstrates a compact PlatformIO application that initializes basic board hardware, starts `TFT_eSPI`, draws a status screen, cycles the RGB LED, reads the LDR and BOOT button, and prints values to UART0.

That firmware is a useful bring-up baseline, but it does not yet provide a scalable architecture for:

- several hardware revisions;
- multiple SPI-connected subsystems;
- diagnostic and production modes;
- safe startup and shutdown;
- concurrent display, touch, SD, audio and wireless activity;
- configuration migration;
- fault isolation;
- trusted update integration;
- automated regression tests;
- reusable application services.

This chapter defines a target architecture that preserves the current working baseline while separating hardware facts, drivers, services, policy, application behavior and diagnostics.

## 2. Current repository baseline

The current codebase has three important architectural elements:

1. `include/CYD_Board.h` centralizes a first set of GPIO constants and basic helper functions;
2. `platformio.ini` selects the ESP32 Arduino environment and injects the current TFT profile through compile definitions;
3. `src/main.cpp` contains board initialization, UI rendering and the polling loop.

The current implementation is intentionally small. It does not yet establish:

- a formal board-profile schema;
- compile-time conflict checking;
- explicit bus ownership;
- separate drivers for touch, SD, audio or wireless;
- task and queue policy;
- structured events;
- persistent configuration;
- runtime capability discovery;
- application-independent diagnostics;
- mockable interfaces;
- host-side unit tests;
- multi-profile CI.

## 3. Architectural objectives

The target software architecture should:

1. keep pin assignments in one authoritative profile;
2. prevent drivers from silently remapping shared resources;
3. separate hardware access from application policy;
4. allow subsystems to fail independently;
5. expose stable interfaces to diagnostics and applications;
6. support multiple board revisions without scattered `#ifdef` logic;
7. make initialization order explicit;
8. make destructive operations explicit;
9. bound memory, queue and log usage;
10. support reproducible builds;
11. permit hardware-independent unit tests where practical;
12. retain enough runtime evidence to diagnose failures;
13. integrate security, update and lifecycle requirements;
14. avoid unnecessary framework lock-in;
15. allow gradual migration from the current self-test.

## 4. Non-goals

This architecture does not by itself claim:

- hard real-time behavior;
- safety certification;
- full portability to every ESP32 family;
- driver compatibility with every CYD revision;
- automatic resolution of electrical conflicts;
- zero-copy operation everywhere;
- complete abstraction of framework-specific behavior;
- immunity to third-party library defects;
- production readiness without hardware qualification.

## 5. Layer model

Recommended layers:

```text
L0 silicon and board hardware
L1 board profile and pin/resource description
L2 low-level hardware adapters
L3 device drivers
L4 subsystem managers and shared services
L5 diagnostics, recovery and provisioning services
L6 application domain logic
L7 UI, network API and external integrations
```

### 5.1 Dependency direction

Dependencies should point downward.

```text
application -> services -> drivers -> adapters -> board profile
```

A low-level driver should not call application UI code.

### 5.2 Cross-cutting services

Cross-cutting services include:

- logging;
- time;
- configuration;
- metrics;
- security policy;
- watchdog supervision;
- power state;
- update state;
- test-result reporting.

These services require controlled interfaces rather than unrestricted global access.

## 6. Proposed repository structure

A practical migration target:

```text
include/
  board/
    BoardProfile.h
    BoardCapabilities.h
    BoardValidation.h
  drivers/
    DisplayDriver.h
    TouchDriver.h
    StorageDriver.h
    AudioDriver.h
    LedDriver.h
    LightSensorDriver.h
    ButtonDriver.h
  services/
    EventBus.h
    ConfigService.h
    DiagnosticService.h
    LogService.h
    HealthService.h
    UpdateService.h
    NetworkService.h
  platform/
    SpiBus.h
    Gpio.h
    Clock.h
    StorageBackend.h
src/
  board/
  drivers/
  services/
  modes/
  app/
config/
  board_profiles/
  schemas/
tests/
  host/
  hardware/
tools/
  service/
```

The exact directory layout may evolve, but the ownership boundaries should remain explicit.

## 7. Board profile as the source of truth

The board profile should contain all sample-specific facts required by software.

Recommended fields:

```text
profile ID
profile schema version
PCB marking
known component population
GPIO assignments
active levels
input-only flags
strap-sensitive flags
bus assignments
chip-select lines
expected controller identities
connector exposure
power-domain notes
capability flags
known conflicts
unverified resources
```

### 7.1 Profile identity

Example conceptual identity:

```text
cyd-esp32-2432s028-ch340g-dual-ams1117-cn1-nc-v1
```

The name should describe verified distinctions, not seller marketing alone.

### 7.2 Compile-time versus runtime fields

Compile-time fields:

- GPIO numbers;
- static bus controller selection;
- compile-time driver inclusion;
- memory-layout selection;
- display geometry where fixed.

Runtime fields:

- calibration;
- feature enablement;
- network settings;
- log level;
- deployment mode;
- optional peripheral presence.

### 7.3 Unverified fields

Unknown values should use explicit states such as:

```text
UNKNOWN
NOT_PRESENT
PRESENT_UNMAPPED
CANDIDATE
VERIFIED
UNSUPPORTED
```

Do not encode an unresolved hypothesis as an ordinary constant without a confidence tag.

## 8. Current pin profile

The current repository uses:

```text
TFT:   MISO 12, MOSI 13, SCLK 14, CS 15, DC 2, RST -1, BL 21
Touch: IRQ 36, MOSI 32, MISO 39, SCLK 25, CS 33
SD:    MISO 19, MOSI 23, SCLK 18, CS 5
RGB:   red 4, green 16, blue 17, active LOW
LDR:   GPIO34
Audio: GPIO26
BOOT:  GPIO0
UART0: TX GPIO1, RX GPIO3
```

The architecture must preserve these assignments until sample-specific evidence approves a change.

## 9. Static profile validation

Compile-time checks should reject invalid profiles.

Examples:

- duplicate exclusive GPIO ownership;
- output assignment to GPIO34, 35, 36 or 39;
- allocation of module-Flash GPIO6–11;
- undeclared sharing of GPIO21;
- missing active-level declaration;
- chip-select duplication without explicit sharing policy;
- unsupported SPI controller assignment;
- strap-sensitive output without startup policy;
- audio output configuration that assumes grounded BTL terminal.

### 9.1 Validation categories

```text
ERROR: impossible or unsafe configuration
WARNING: legal but high-risk or unverified
INFO: intentional declared sharing
```

Warnings should be visible in build output and release manifests.

## 10. Capability model

Applications should query capabilities rather than infer them from profile names.

Example conceptual capabilities:

```text
hasDisplay
hasResistiveTouch
hasMicroSd
hasAudioAmplifier
hasRgbLed
hasLightSensor
hasUsbUart
hasAuxFlash
hasWifi
hasBluetoothClassic
hasBle
hasExpansionP3
hasExpansionCn1
```

Each capability should also expose status:

```text
SUPPORTED
PRESENT_UNVERIFIED
VERIFIED
FAILED
DISABLED_BY_POLICY
```

## 11. Hardware abstraction boundary

The hardware abstraction layer should wrap framework-specific primitives used by higher layers.

Candidate interfaces:

```cpp
class IGpio;
class ISpiBus;
class IUart;
class IClock;
class IRandom;
class IResetInfo;
class IKeyValueStore;
class IBlockStorage;
```

This does not require abstracting every single Arduino call. Abstract only where it improves ownership, testing or portability.

## 12. Driver interface principles

A driver should:

- receive dependencies explicitly;
- avoid hidden global initialization;
- expose initialization state;
- return structured errors;
- document blocking behavior;
- bound operation duration;
- provide cleanup or suspend behavior;
- not own unrelated policy;
- expose raw observations needed by diagnostics;
- preserve thread-safety rules.

### 12.1 Driver lifecycle

Recommended lifecycle:

```text
UNINITIALIZED
INITIALIZING
READY
DEGRADED
FAILED
SUSPENDED
STOPPED
```

### 12.2 Result type

Prefer structured results:

```text
status
subsystem
operation
error code
raw platform code
retryability
timestamp
```

Avoid reducing every failure to `bool`.

## 13. Dependency injection

Drivers should receive buses and services through constructors or explicit `begin()` parameters.

Conceptual example:

```cpp
DisplayDriver display(board.display, spiDisplay, gpio, clock, log);
```

Benefits:

- ownership is visible;
- tests can use fakes;
- global initialization order is reduced;
- profile differences are localized;
- resource conflicts can be validated before activation.

## 14. Global-object policy

Global objects may be acceptable for static embedded systems, but their constructors should not touch hardware.

Rules:

- no GPIO access before controlled startup;
- no bus initialization in global constructors;
- no logging before logging backend is ready;
- no dynamic allocation hidden in global construction where avoidable;
- initialization order must not depend on translation-unit order.

## 15. Initialization phases

Recommended startup phases:

```text
S0 reset capture
S1 safe GPIO state
S2 minimal serial logging
S3 profile validation
S4 configuration load and validation
S5 service-mode decision
S6 essential buses
S7 local UI and indicators
S8 storage
S9 network
S10 application services
S11 healthy checkpoint
```

### 15.1 S0 reset capture

Capture reset reason and retained boot marker before unrelated initialization changes evidence.

### 15.2 S1 safe GPIO state

Apply safe inactive levels for:

- RGB LED;
- backlight;
- audio source;
- chip selects;
- external outputs.

Strap-sensitive pins require special care.

### 15.3 Failure behavior

Each phase should define:

- whether failure is fatal;
- whether safe mode remains possible;
- which later phases are blocked;
- what status is shown;
- what evidence is retained.

## 16. Startup dependency graph

Example:

```text
profile validation
  -> GPIO safety
  -> logging
  -> display SPI -> display driver -> local UI
  -> touch SPI -> touch driver
  -> SD SPI -> storage driver -> config/data services
  -> Wi-Fi -> network services
  -> application
```

A dependency graph prevents arbitrary initialization order hidden across files.

## 17. Subsystem ownership

Every exclusive resource needs one owner.

| Resource | Recommended owner |
|---|---|
| TFT bus transaction policy | display bus manager |
| touch bus | touch driver or dedicated bus owner |
| microSD bus | storage bus owner |
| UART0 console | console service |
| RGB LED | indicator service |
| GPIO21 backlight | display/backlight service |
| GPIO26 audio source | audio service |
| NVS/config store | configuration service |
| Wi-Fi connection | network service |
| OTA state | update service |

Other modules request operations through the owner rather than manipulating the resource directly.

## 18. SPI architecture

The current profile has three distinct SPI pin groups. Software must not assume three independent hardware SPI controllers.

Recommended approach:

- declare physical controller allocation explicitly;
- create named bus instances;
- prevent third-party libraries from calling uncontrolled global `SPI.begin()`;
- serialize transactions per controller;
- set mode, frequency and bit order for every transaction;
- return chip selects to inactive state;
- verify library behavior during integration.

### 18.1 Bus descriptor

```text
bus ID
controller
MISO/MOSI/SCLK
maximum qualified frequency
DMA policy
mutex policy
allowed clients
startup state
```

### 18.2 Transaction descriptor

```text
client ID
chip select
mode
frequency
bit order
timeout
priority
expected duration
```

### 18.3 Bus locking

A bus lock should:

- be bounded by timeout;
- identify the owner;
- support diagnostic tracing;
- avoid recursive use unless explicitly designed;
- not be held across unrelated waits;
- release on all error paths.

### 18.4 Library adapters

If a library owns bus creation internally, wrap or configure it so pin and controller selection remain profile-controlled.

## 19. TFT driver integration

The display layer should separate:

- controller transport;
- panel geometry and rotation;
- backlight control;
- drawing API;
- UI rendering;
- diagnostics.

### 19.1 `TFT_eSPI` integration

The current `platformio.ini` injects the ILI9341-compatible configuration with build flags. This is practical for the existing sample, but creates compile-time coupling.

The architecture should:

- keep these flags generated or profile-linked;
- prevent contradictory definitions elsewhere;
- record the resolved build configuration;
- expose the assumed controller as a profile hypothesis until verified;
- provide a path for alternate display profiles.

### 19.2 Display service ownership

Only the display service should perform ordinary rendering. Other services submit view models, messages or drawing requests.

### 19.3 Frame/update policy

Define:

- maximum update rate;
- partial versus full redraw;
- bus budget;
- touch coexistence;
- memory policy for sprites/buffers;
- diagnostic override behavior.

## 20. Backlight service

GPIO21 is both backlight control and physically exposed on P3.

The backlight service should own:

- startup state;
- on/off behavior;
- optional PWM only after verification;
- brightness policy;
- idle dimming;
- diagnostic override;
- sleep transitions.

No generic GPIO API should allow ordinary application code to repurpose GPIO21 silently.

## 21. Touch driver integration

Separate:

- raw X/Y/Z acquisition;
- IRQ handling;
- filtering;
- debounce/release detection;
- calibration;
- rotation transform;
- gesture interpretation;
- UI events.

### 21.1 Interrupt policy

The ISR should do minimal work:

- capture timestamp or set a flag;
- notify a task;
- avoid SPI transactions in the ISR;
- avoid logging large messages;
- avoid dynamic allocation.

### 21.2 Calibration service

Calibration belongs in a versioned configuration object linked to:

- board asset ID;
- profile ID;
- display rotation;
- driver version;
- enclosure state.

## 22. microSD and filesystem integration

The storage stack should separate:

```text
SPI transport
card driver
block device
filesystem
file repository
application data policy
```

### 22.1 Storage service responsibilities

- mount state;
- retry policy;
- safe unmount;
- file locking;
- path policy;
- free-space monitoring;
- write batching;
- corruption reporting;
- diagnostic read/hash/write tests.

### 22.2 Removal behavior

When card access fails:

- stop new writes;
- finish or abort current operation safely;
- mark storage unavailable;
- notify interested services;
- retain error context;
- avoid formatting automatically.

### 22.3 Application isolation

Applications should use a storage repository interface rather than arbitrary paths spread through the codebase.

## 23. Program Flash and NVS integration

Create separate logical stores for:

- immutable release identity;
- board identity;
- protected credentials;
- ordinary configuration;
- calibration;
- retained diagnostics;
- update state;
- application state.

### 23.1 Write policy

- validate before commit;
- use schema versioning;
- use atomic replacement where possible;
- rate-limit frequent counters;
- preserve corrupt copies for diagnosis;
- separate factory defaults from active configuration.

## 24. Auxiliary W25Q32 integration

U4 remains present but unmapped.

Software architecture should represent it as:

```text
PRESENT_UNMAPPED
```

No production service should depend on it until:

- pins are verified;
- controller ownership is declared;
- original image is preserved;
- access policy is designed;
- read/write tests are qualified.

## 25. Audio driver integration

Separate:

- waveform generation;
- timer or DAC resource;
- volume/amplitude policy;
- amplifier/load assumptions;
- playback queue;
- fault handling;
- diagnostic tone generation.

### 25.1 Resource ownership

The audio service should exclusively own GPIO26 and any timer, DAC or PWM resources it uses.

### 25.2 Safe default

At startup and failure:

- output must enter a defined quiet state;
- no uncontrolled full-scale waveform;
- no assumption that either speaker terminal is ground;
- diagnostic tones require explicit request.

## 26. RGB indicator service

Application code should request semantic states rather than raw channel writes.

Examples:

```text
BOOTING
READY
WARNING
FAULT
UPDATING
RECOVERY
NETWORK_CONNECTING
```

The indicator service maps states to LED patterns while respecting active-LOW behavior and GPIO4 strap sensitivity.

### 26.1 Priority

A fault or update indication may override a normal application animation.

Define explicit priorities and restoration behavior.

## 27. LDR service

The light-sensor service should provide:

- raw samples;
- filtered value;
- sample timestamp;
- noise estimate;
- optional relative-light interpretation;
- calibration metadata;
- diagnostic raw mode.

Do not expose fabricated lux values without calibration.

## 28. Button service

The BOOT button service should distinguish:

- raw level;
- debounced press;
- short press;
- long press;
- startup gesture;
- ROM bootloader strap behavior.

Runtime button semantics must not compromise reset-time boot behavior.

## 29. UART and console architecture

UART0 is shared by:

- ROM boot messages;
- flashing;
- application logs;
- diagnostic console;
- P1 service access.

A console service should own application use of UART0.

### 29.1 Stream separation

Possible strategies:

- human-readable text mode;
- machine-readable framed mode;
- explicit mode negotiation;
- tagged log lines;
- bounded command parser.

Do not interleave binary fixture frames with uncontrolled debug prints.

## 30. Network service

The network service should own:

- radio initialization;
- Wi-Fi state machine;
- credential access;
- scan and association;
- DHCP and DNS state;
- reconnect policy;
- time synchronization;
- application socket lifecycle;
- telemetry of failures.

### 30.1 State machine

```text
DISABLED
STARTING
SCANNING
ASSOCIATING
AUTHENTICATING
ADDRESSING
ONLINE
DEGRADED
BACKOFF
STOPPING
```

### 30.2 Backoff

Reconnect attempts should use bounded backoff rather than a tight loop.

### 30.3 Application boundary

Application services should consume a connectivity abstraction, not call Wi-Fi setup routines directly.

## 31. Bluetooth service

Bluetooth should be compiled or enabled only when required.

The service should own:

- mode selection;
- advertising;
- pairing/bonding policy;
- GATT or Classic profile lifecycle;
- coexistence state;
- shutdown and memory release where supported.

## 32. Event architecture

Use typed events for asynchronous changes.

Examples:

```text
BootCompleted
ButtonPressed
TouchSampleReady
StorageMounted
StorageRemoved
NetworkOnline
NetworkOffline
UpdateAvailable
UpdateProgress
UpdateFailed
SubsystemFault
ConfigChanged
```

### 32.1 Event properties

Each event should define:

- type ID;
- payload schema;
- timestamp;
- producer;
- delivery policy;
- queue behavior;
- ownership/lifetime.

### 32.2 Avoid event abuse

Not every function call should become an event. Use direct calls for synchronous ownership relationships and events for state changes or fan-out.

## 33. Queue policy

Every queue should declare:

```text
capacity
item size
overflow policy
producer context
consumer context
latency target
telemetry counters
```

Possible overflow policies:

- reject newest;
- drop oldest;
- coalesce;
- block with timeout;
- escalate fault.

Silent unbounded growth is not allowed.

## 34. Task model

A possible task decomposition:

```text
system supervisor
UI/render task
touch/input task
storage task
network task
audio task
diagnostics/service task
application task
```

The final model should be driven by measured need, not by creating one task per class.

### 34.1 Task ownership

Each task should declare:

- responsibility;
- core affinity if used;
- priority;
- stack size;
- watchdog policy;
- queues;
- blocking calls;
- shutdown behavior.

### 34.2 Priority inversion

Shared bus locks and filesystem locks must be reviewed for priority inversion.

### 34.3 Single-thread option

A cooperative event loop may remain appropriate for simple applications. Architecture boundaries are still useful even without multiple tasks.

## 35. Supervisor service

The supervisor should track:

- subsystem states;
- healthy checkpoint;
- watchdog progress;
- boot phase;
- degraded-mode entry;
- restart requests;
- fatal-fault policy;
- update trial state.

It should not directly implement every subsystem.

## 36. Watchdog design

A watchdog design should identify progress, not merely task existence.

Each supervised task may publish a heartbeat containing:

```text
task ID
progress counter
last successful operation
last error
queue depth
timestamp
```

Increasing watchdog timeouts is not a substitute for resolving blocked operations.

## 37. Time service

Separate:

- monotonic time;
- wall-clock time;
- synchronized time validity;
- timezone/display formatting.

Drivers and timeouts should use monotonic time.

Logs should identify whether wall-clock time is synchronized.

## 38. Memory policy

The project should define:

- static versus dynamic allocation rules;
- maximum framebuffer/sprite sizes;
- queue memory budget;
- log buffer limits;
- network buffer limits;
- fragmentation monitoring;
- behavior on allocation failure.

### 38.1 Dynamic allocation

Dynamic allocation may be acceptable, but long-lived and high-frequency allocation should be measured.

### 38.2 Large buffers

Large buffers should have clear ownership and lifetime. Avoid duplicate full-screen buffers unless memory budget proves them acceptable.

## 39. Error model

Use a common error taxonomy:

```text
INVALID_ARGUMENT
NOT_INITIALIZED
NOT_PRESENT
UNSUPPORTED
BUSY
TIMEOUT
IO_ERROR
CRC_ERROR
AUTH_ERROR
CONFIG_ERROR
RESOURCE_CONFLICT
POWER_FAULT
INTERNAL_ERROR
```

Subsystem-specific codes may extend the base taxonomy.

### 39.1 Retryability

Every error should indicate whether retry is:

- safe immediately;
- safe after backoff;
- safe after reinitialization;
- unsafe without operator action;
- destructive.

## 40. Degraded modes

Examples:

- display failed, serial service remains;
- touch failed, BOOT/serial navigation remains;
- SD unavailable, internal configuration remains;
- network unavailable, local operation continues;
- audio disabled after thermal or configuration fault;
- application blocked while diagnostics remain available.

Degraded behavior must be explicit and tested.

## 41. Configuration architecture

Separate configuration domains:

```text
board profile
factory identity
security/provisioning
network
UI
calibration
application
service policy
update policy
```

### 41.1 Schema requirements

Each schema should include:

- schema version;
- defaults;
- validation rules;
- migration path;
- secret classification;
- ownership;
- persistence location;
- rollback behavior.

### 41.2 Configuration transaction

Recommended flow:

```text
read candidate
parse
validate syntax
validate semantics
validate resource conflicts
stage
apply to test context
commit atomically
publish ConfigChanged
```

### 41.3 Secret separation

Do not export secrets through ordinary configuration dumps.

## 42. Build-time configuration

Build-time options should be centralized.

Examples:

- board profile;
- feature set;
- diagnostic capability;
- logging ceiling;
- partition table;
- security profile;
- release channel.

The resolved options should be embedded in a build manifest.

## 43. Runtime mode model

Recommended modes:

```text
NORMAL
SAFE_DIAGNOSTIC
FULL_DIAGNOSTIC
FACTORY_TEST
PROVISIONING
RECOVERY
UPDATE_TRIAL
QUARANTINE
DECOMMISSION
```

Mode transitions should pass through a policy service rather than scattered flags.

## 44. Diagnostics integration

Diagnostics should use the same drivers as normal operation wherever possible.

A diagnostic test may request raw access, but should not maintain a completely separate hidden hardware stack unless necessary.

### 44.1 Test interface

```text
prepare
execute
collect raw evidence
evaluate criterion
cleanup
report
```

### 44.2 Test isolation

Tests should declare:

- required resources;
- conflicting services;
- destructive level;
- expected duration;
- cleanup guarantee.

## 45. Logging architecture

The log service should support:

- severity;
- subsystem tag;
- event ID;
- monotonic timestamp;
- optional wall time;
- UART sink;
- bounded memory ring;
- optional SD export;
- secret redaction;
- machine-readable output.

### 45.1 ISR logging

ISRs should not perform formatted blocking logging. Use counters or lightweight deferred records.

### 45.2 Log backpressure

When sinks are slow:

- preserve high-severity records;
- drop or coalesce lower-severity records;
- expose dropped-record counters;
- never block critical control indefinitely.

## 46. Metrics and health model

Useful metrics:

```text
boot count
reset reason counts
uptime
free/minimum heap
queue high-water marks
SPI timeout counts
SD error counts
Wi-Fi reconnect counts
touch IRQ/sample counts
watchdog warnings
config migration results
update attempts and rollbacks
```

Metrics are evidence, not automatic root-cause conclusions.

## 47. Update-service integration

The update service should coordinate with:

- network service;
- storage/Flash backend;
- security verification;
- configuration migration;
- supervisor;
- UI and indicators;
- watchdog;
- power policy;
- diagnostics.

### 47.1 Update ownership

Only the update service should write application update partitions during normal operation.

### 47.2 Trial boot

A new image should declare healthy only after required services reach defined checkpoints.

## 48. Security integration

Security must not be a late wrapper.

Architectural integration points:

- credential provider interface;
- authenticated command authorization;
- signed-update verification;
- redacted logging;
- privilege distinction between modes;
- audit events;
- provisioning state;
- revocation state;
- secure configuration domain.

## 49. Power-state integration

Define states such as:

```text
ACTIVE
IDLE
DISPLAY_DIMMED
RADIO_OFF
SLEEP_PREPARE
SLEEPING
SHUTDOWN_FAULT
```

Before sleep or controlled restart:

- stop new storage writes;
- flush required state;
- quiet audio;
- set chip selects safely;
- record transition;
- coordinate wake sources.

No deep-sleep behavior is claimed until wake and peripheral-state tests are performed.

## 50. UI architecture

Separate:

- UI model/state;
- rendering;
- input events;
- navigation;
- service/diagnostic views;
- localization where needed.

### 50.1 Model-view separation

Subsystems should publish state. UI code should not poll raw drivers indiscriminately.

### 50.2 Failure-safe UI

Critical recovery functions must remain available without touch.

## 51. Application-domain boundary

Application logic should use interfaces such as:

```text
IDisplayView
IInputEvents
IStorageRepository
INetworkClient
IAudioNotifier
IHealthQuery
```

It should not own GPIO numbers or call low-level bus initialization.

## 52. Third-party library policy

For each library record:

```text
name and version
license
source
maintainer status
memory behavior
thread-safety
bus ownership behavior
known conflicts
security history
qualified profile
```

### 52.1 Pin and bus audit

Before integration verify whether the library:

- uses global `SPI` or `Wire`;
- calls `begin()` with default pins;
- changes frequency globally;
- assumes a free interrupt pin;
- allocates large buffers;
- blocks indefinitely;
- creates hidden tasks;
- writes persistent storage.

## 53. Arduino-framework boundary

The initial implementation may remain Arduino-based.

Framework-dependent code should be concentrated in:

- GPIO adapter;
- SPI/UART adapters;
- timing;
- Wi-Fi/Bluetooth adapters;
- NVS/Flash backends;
- task primitives.

This enables later migration or selective use of ESP-IDF APIs without rewriting application logic.

## 54. ESP-IDF interoperation

ESP32 Arduino runs on ESP-IDF components. Direct ESP-IDF use may be appropriate for:

- reset reason;
- watchdog control;
- partition and OTA state;
- NVS details;
- heap diagnostics;
- event groups and queues;
- security features.

Document every direct-IDF dependency and its framework-version compatibility.

## 55. Portability strategy

Portability should be defined by layers.

- application portability across board profiles;
- service portability across ESP32 variants;
- driver portability where controller and transport match;
- adapter portability across frameworks.

Do not promise full portability from interface names alone.

## 56. Profile variants

Future profiles may differ in:

- USB-UART bridge;
- TFT controller;
- backlight polarity;
- touch routing;
- SD routing;
- RGB population;
- regulator arrangement;
- connector wiring;
- auxiliary Flash use.

A profile difference should not require editing application source.

## 57. Feature flags

Feature flags should be typed and auditable.

Examples:

```text
FEATURE_TOUCH
FEATURE_SD
FEATURE_AUDIO
FEATURE_WIFI
FEATURE_BLE
FEATURE_AUX_FLASH
FEATURE_DIAGNOSTICS
FEATURE_OTA
```

Release manifests should list enabled features.

Avoid deeply nested preprocessor conditionals inside application logic.

## 58. Interface versioning

Version:

- board-profile schema;
- configuration schemas;
- diagnostic protocol;
- service API;
- event payloads;
- update metadata;
- host-tool protocol.

Breaking changes require explicit major-version handling.

## 59. Unit testing

Host-side tests can cover:

- configuration parsing;
- schema validation;
- state machines;
- event routing;
- queue overflow policy;
- command parser;
- update metadata validation;
- calibration transforms;
- error mapping;
- resource-conflict checks.

## 60. Fake and mock adapters

Useful fakes:

- fake clock;
- fake GPIO;
- scripted SPI bus;
- in-memory key/value store;
- fake network state;
- deterministic random source for tests only;
- fault-injecting storage backend.

Production code must not accidentally use test credentials or deterministic randomness.

## 61. Hardware-in-the-loop testing

HIL suites should verify:

- boot phases;
- pin safe states;
- TFT initialization;
- touch acquisition;
- SD operations;
- RGB/LDR/buttons;
- audio safety and output;
- Wi-Fi state machine;
- reset and recovery;
- update trial/rollback;
- combined resource use.

Results should include profile ID, firmware hash and fixture revision.

## 62. Integration-test matrix

Important combinations:

```text
TFT + touch
TFT + SD
TFT + Wi-Fi
SD + Wi-Fi
SD + audio
TFT + touch + SD
TFT + touch + SD + Wi-Fi
full declared application load
```

Independent subsystem success does not prove combined success.

## 63. CI evolution

Current CI runs `pio run` only.

Recommended additions:

1. build all supported profiles;
2. treat warnings selectively;
3. run host unit tests;
4. validate schemas;
5. validate unique test/event/error IDs;
6. run static analysis;
7. check formatting;
8. generate build manifest;
9. scan dependencies and secrets;
10. archive firmware artifacts and hashes;
11. verify documentation links;
12. publish HIL results separately.

CI build success remains different from hardware acceptance.

## 64. Static analysis

Possible checks:

- uninitialized values;
- incorrect signed/unsigned conversions;
- unchecked return values;
- unreachable code;
- duplicate ownership;
- ISR-unsafe calls;
- blocking calls in critical tasks;
- dangerous format strings;
- secret literals;
- stack usage where tooling permits.

Tool output must be reviewed rather than treated as automatically correct.

## 65. Coding rules

Recommended rules:

- no raw GPIO numbers outside profiles/adapters;
- no hidden bus initialization;
- no unbounded `while` waits;
- every blocking operation has timeout or documented reason;
- every persistent format has a version;
- destructive operations require explicit policy;
- errors retain subsystem context;
- logs redact secrets;
- cleanup paths are tested;
- comments distinguish verified facts from assumptions.

## 66. Concurrency review checklist

For every shared object ask:

- who owns it;
- which tasks call it;
- whether calls are reentrant;
- whether ISR access occurs;
- which lock protects it;
- maximum lock duration;
- timeout behavior;
- shutdown behavior;
- fault-injection coverage.

## 67. Performance budgets

Define budgets for:

- boot time;
- UI latency;
- touch response;
- display bus occupancy;
- SD write latency;
- network reconnect;
- audio scheduling;
- diagnostic command response;
- queue depth;
- memory headroom.

No universal numbers are assigned until application requirements and measurements exist.

## 68. Bus bandwidth budgeting

For each bus record:

```text
qualified clock
client traffic
transaction size
frequency
duty cycle
worst-case blocking time
priority
```

Avoid using theoretical SPI clock as proof of application throughput.

## 69. Startup-time budgeting

Track time spent in:

- serial delay;
- profile/config validation;
- TFT init;
- SD mount;
- Wi-Fi association;
- time sync;
- service discovery.

Nonessential network operations should not block local safe-mode access indefinitely.

## 70. Fault containment

A subsystem fault should not corrupt unrelated state.

Examples:

- SD failure does not block serial diagnostics;
- touch failure does not block display rendering;
- network failure does not trigger repeated Flash writes;
- display failure does not disable update rollback;
- malformed configuration does not erase its original copy.

## 71. Restart policy

Possible restart scopes:

- retry operation;
- reinitialize driver;
- restart service task;
- restart application mode;
- reboot device;
- enter safe mode;
- quarantine.

Escalation should be bounded to avoid reboot loops.

## 72. Boot-loop prevention

Maintain:

- failed-boot count;
- last boot phase;
- update-trial state;
- safe-mode threshold;
- reset-cause classification;
- operator-visible recovery path.

Persistent counters must be wear-limited.

## 73. State persistence

Classify state:

```text
volatile
retained across soft reset
persistent across power loss
backup-required
security-sensitive
```

Do not persist transient state merely because it is easy.

## 74. Data ownership

Each persistent record should have one schema owner.

Examples:

- configuration service owns configuration;
- calibration service owns coefficients;
- update service owns OTA state;
- diagnostic service owns test results;
- application owns domain data.

## 75. Factory-test integration

Factory-test mode should use production drivers but controlled fixtures and policies.

It may:

- run deterministic suites;
- provision identity after acceptance;
- emit machine-readable results;
- lock ordinary application behavior;
- require fixture authorization.

Factory-test shortcuts must not leak into deployed mode.

## 76. Provisioning integration

Provisioning should be a state machine coordinated by:

- identity service;
- credential provider;
- configuration service;
- network service;
- backend enrollment;
- audit log;
- security policy.

A partially provisioned device should remain distinguishable and recoverable.

## 77. Service-mode integration

Service mode should reuse:

- board profile;
- drivers;
- logs;
- diagnostics;
- configuration validators;
- update and recovery services.

Service-only capabilities should be protected by mode and authorization policy.

## 78. Release manifest

Every firmware release should record:

```text
source commit
build environment
board profile ID
feature flags
framework/platform versions
library versions
partition table
configuration schema versions
security profile
binary hashes
known limitations
qualified hardware revisions
```

## 79. Architecture decision records

Use short ADRs for decisions such as:

- one SPI controller versus remapped controllers;
- Arduino-only versus selective ESP-IDF use;
- queue/event design;
- configuration backend;
- OTA partition layout;
- logging format;
- display-library selection.

Each ADR should record context, decision, alternatives and consequences.

## 80. Migration from current code

A low-risk migration sequence:

### Phase 1 — preserve behavior

- move pin constants into a versioned `BoardProfile`;
- retain current `main.cpp` behavior;
- add compile-time validation;
- embed build/profile identity.

### Phase 2 — extract simple drivers

- RGB LED;
- backlight;
- LDR;
- BOOT button.

### Phase 3 — extract display service

- wrap `TFT_eSPI`;
- separate UI rendering;
- add display status/error reporting.

### Phase 4 — add event and configuration foundations

- typed events;
- configuration schema;
- log service;
- reset/boot state.

### Phase 5 — integrate touch and SD

- explicit SPI ownership;
- independent tests;
- combined-load tests.

### Phase 6 — add network and audio

- state-machine services;
- resource ownership;
- fault isolation.

### Phase 7 — diagnostics, provisioning and update

- reuse stable drivers;
- add authorization and recovery policy;
- add HIL regression.

## 81. Refactoring safeguards

During migration:

- keep a known-good baseline binary;
- change one subsystem at a time;
- retain serial evidence;
- compare current consumption and boot behavior;
- run existing visual self-test after each phase;
- avoid simultaneous pin, library and architecture changes;
- preserve rollback branches/tags.

## 82. Example subsystem contract

Conceptual interface:

```cpp
struct SubsystemStatus {
  State state;
  ErrorCode lastError;
  uint32_t lastChangeMs;
};

class ISubsystem {
 public:
  virtual Result begin() = 0;
  virtual Result suspend() = 0;
  virtual Result resume() = 0;
  virtual SubsystemStatus status() const = 0;
  virtual ~ISubsystem() = default;
};
```

The actual implementation should avoid unnecessary virtual dispatch if static composition is preferable. The important point is a consistent lifecycle contract.

## 83. Example board-profile fragment

Conceptual only:

```cpp
struct PinSpec {
  int gpio;
  Direction capability;
  ActiveLevel activeLevel;
  bool strapSensitive;
  bool externallyExposed;
};

struct BoardProfile {
  const char* id;
  DisplaySpec display;
  TouchSpec touch;
  StorageSpec storage;
  AudioSpec audio;
  LedSpec rgb;
};
```

## 84. Example event fragment

```cpp
struct NetworkStateChanged {
  NetworkState previous;
  NetworkState current;
  NetworkError reason;
  uint32_t timestampMs;
};
```

Payloads should remain bounded and avoid owning ambiguous raw pointers.

## 85. Documentation generation

Generated documentation may include:

- resolved pin table;
- resource ownership table;
- profile capabilities;
- feature flags;
- schema versions;
- event/test/error ID catalog;
- release manifest.

Generated output should not replace explanatory engineering documentation.

## 86. Review checklist

Before merging architectural changes verify:

- profile and resource ownership remain consistent;
- no raw pin duplication was introduced;
- initialization phases remain safe;
- third-party library behavior is understood;
- failure paths preserve diagnostics;
- queues and buffers are bounded;
- persistent formats are versioned;
- security and secret handling are preserved;
- HIL impact is identified;
- release manifest changes are expected.

## 87. Unsupported claims

Until implemented and tested, do not claim:

- complete HAL separation;
- thread-safe drivers;
- deterministic task timing;
- conflict-free simultaneous SPI operation;
- automatic hardware-profile detection;
- portable operation across all CYD boards;
- transactional configuration under every power-loss condition;
- production-quality OTA integration;
- full host-test coverage;
- hardware-in-the-loop automation;
- memory-leak-free long-duration operation;
- bounded latency for all operations;
- safe deep sleep;
- robust remote diagnostics.

## 88. Completion criteria

The architecture can be considered sample-qualified when:

1. a versioned board-profile schema exists;
2. all current pin assignments originate from one profile;
3. compile-time validation rejects unsafe conflicts;
4. build manifest includes profile and feature identity;
5. basic GPIO/backlight/LDR/button drivers are separated from application code;
6. display access is owned by a display service;
7. touch, SD and display bus ownership is explicit;
8. driver errors use structured codes;
9. initialization phases and failure behavior are implemented;
10. configuration schemas and migration tests exist;
11. logging is bounded and secret-aware;
12. typed events have documented queue policy;
13. task ownership, priorities and watchdog policy are documented;
14. network behavior uses a state machine;
15. storage removal and corruption paths are tested;
16. diagnostics reuse production drivers;
17. update service integrates healthy checkpoint and rollback;
18. security/provisioning state is represented explicitly;
19. host-side tests cover non-hardware logic;
20. HIL tests cover independent and combined subsystems;
21. CI builds all supported profiles;
22. release artifacts include hashes and manifests;
23. migration preserves the current known-good self-test behavior;
24. architecture decisions are recorded;
25. unsupported claims remain explicit.

## 89. Open implementation tasks

- Replace raw constants with a versioned profile object.
- Add compile-time GPIO and resource validation.
- Generate TFT build flags from the selected profile.
- Extract RGB, backlight, LDR and button drivers.
- Wrap `TFT_eSPI` behind a display service.
- Define SPI controller and lock ownership.
- Add touch and SD driver adapters.
- Define common `Result` and error taxonomy.
- Add reset-reason and boot-phase services.
- Define event payloads and queue policies.
- Introduce configuration schemas and migrations.
- Add bounded logging and machine-readable records.
- Add network state machine.
- Add audio resource ownership.
- Add diagnostic-mode orchestration.
- Add host tests and fakes.
- Extend CI beyond a single build.
- Add HIL test definitions and fixture protocol.
- Generate release manifests and binary hashes.

## 90. Summary

The target architecture replaces scattered hardware access with a controlled chain:

```text
verified board profile
  -> validated resources
  -> explicit adapters and drivers
  -> owned subsystem services
  -> typed events and configuration
  -> diagnostics, security and update policy
  -> application behavior
```

The most important rule is:

> no subsystem should silently initialize, remap, share or persist a resource outside its declared ownership contract.

The current self-test should remain the behavioral reference during migration, while each new architectural layer is introduced and qualified incrementally.