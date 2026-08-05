# Application Framework, UI and Data-Flow Architecture — ESP32-2432S028

> **Document status:** sample-specific application architecture for the author's `ESP32-2432S028` board.  
> **Scope:** application modes, state models, screens, touch interaction, data acquisition, local persistence, networking, commands, alarms, user workflows, accessibility, localization, application testing, and integration with the lower software layers defined in Part 21.  
> **Evidence rule:** a screen that looks correct is not evidence that the underlying data, command, persistence, alarm, or recovery path is correct. Application claims require declared state transitions, validated input, retained test evidence, and verification under degraded and failure conditions.

## 1. Purpose

Part 21 defines the lower software architecture: board profiles, hardware abstraction, drivers, services, event transport, diagnostics, configuration, security, and task ownership. This chapter defines how an application should use those services without bypassing them.

The application framework must answer:

- What operating modes exist?
- Which state is authoritative?
- How does sensor or subsystem data reach the screen?
- How are local and remote commands validated?
- How are user actions confirmed and audited?
- How are alarms generated, acknowledged, silenced, cleared, and retained?
- What happens when touch, display, storage, time, or network services are unavailable?
- How are configuration changes made transactionally?
- How are screens tested independently of physical hardware?
- How is the application kept portable across verified board profiles?

## 2. Relationship to previous parts

The application layer must respect these lower-level contracts:

- Part 6: TFT capability, rotation, color, backlight, and display diagnostics;
- Part 7: raw touch, filtering, pressure, calibration, and rotation mapping;
- Part 9: program Flash, microSD, NVS, and auxiliary Flash boundaries;
- Part 10: BOOT button, RGB LED, and LDR semantics;
- Part 13: Wi-Fi and Bluetooth states;
- Part 15: pin ownership and conflicts;
- Part 18: diagnostic and recovery modes;
- Part 20: authentication, provisioning, update, and security state;
- Part 21: service ownership, event model, queues, configuration schemas, and degraded modes.

The application must not call low-level GPIO, SPI, UART, storage, or network primitives directly when an owning service exists.

## 3. Current baseline

The current repository application is a compact self-test:

- one global TFT object;
- one static screen;
- periodic LDR and BOOT reads;
- periodic RGB cycling;
- text output over UART0;
- a single Arduino `setup()` and `loop()` flow.

That baseline is useful for hardware confirmation but does not yet provide:

- an explicit application state model;
- screen navigation;
- command routing;
- configuration editing;
- alarms;
- local persistence;
- remote synchronization;
- accessibility or localization;
- independent UI tests;
- application-level recovery behavior.

## 4. Architectural position

Recommended layers:

```text
Application policy and use cases
Application state store
Command handlers and reducers
View models
Screen controllers
Widgets and rendering adapters
Input mapping
Domain services from Part 21
Hardware services and drivers
```

Dependencies should point downward. Rendering code must not become the source of truth.

## 5. Application design principles

### 5.1 One authoritative state

Every displayed value should be traceable to an authoritative state object or query result.

Avoid:

- separately maintained copies of the same setting in UI and service code;
- screen-local booleans that silently override system state;
- direct editing of persistent structures from widgets;
- inferring device health from display color alone.

### 5.2 Commands change state; views render state

A touch action should create an intent or command. The command handler validates it, invokes the required service, updates state, and emits a result. The view then renders the resulting state.

```text
Touch -> Intent -> Validation -> Command -> Service -> Result -> State update -> Render
```

### 5.3 Explicit degraded behavior

Each screen should declare which data and services are required and how it behaves when they are unavailable.

### 5.4 Bounded work

Rendering, event handling, storage, and network operations must be bounded. No screen callback should block indefinitely.

### 5.5 Recoverable navigation

A failed modal, missing touch input, or interrupted configuration flow must not trap the application permanently.

## 6. Application modes

Recommended application modes:

| Mode | Purpose |
|---|---|
| BOOTING | application services are starting |
| FIRST_RUN | required setup or provisioning is incomplete |
| NORMAL | ordinary operation |
| DEGRADED | core function continues with missing optional services |
| OFFLINE | network unavailable but local operation continues |
| MAINTENANCE | authorized maintenance operations |
| DIAGNOSTIC | service toolkit owns the interaction flow |
| UPDATE_PENDING | update staged or awaiting authorization |
| UPDATING | update process active |
| RECOVERY | recovery workflow active |
| LOCKED | security or policy prevents normal operation |
| QUARANTINED | operation restricted because integrity or identity is uncertain |

Mode transitions must be defined centrally and logged.

## 7. Mode transition model

Example transition rules:

```text
BOOTING -> FIRST_RUN       when provisioning/configuration incomplete
BOOTING -> NORMAL          when required services are healthy
BOOTING -> DEGRADED        when required minimum works but optional services fail
BOOTING -> RECOVERY        after repeated failed boots
NORMAL -> OFFLINE          when network is lost but local policy permits operation
NORMAL -> MAINTENANCE      after authorized local or remote request
NORMAL -> UPDATE_PENDING   when a valid update is available
UPDATE_PENDING -> UPDATING after explicit policy and readiness checks
UPDATING -> NORMAL         after trial image reaches healthy checkpoint
UPDATING -> RECOVERY       after repeated update boot failure
ANY -> QUARANTINED         on identity, integrity, or serious hardware uncertainty
```

Illegal transitions should be rejected with structured reasons.

## 8. Application state store

A central state store should expose a coherent snapshot.

Conceptual state:

```text
ApplicationState
  mode
  identity
  firmware
  provisioning
  security
  power
  display
  touch
  storage
  network
  time
  sensors
  outputs
  alarms
  configuration
  update
  diagnostics
  userSession
  uiNavigation
```

### 8.1 Immutable snapshots

Where practical, publish immutable or versioned snapshots to views. This prevents partial rendering of mixed-old-and-new fields.

### 8.2 State version

Include a monotonic state revision so views and logs can correlate changes.

### 8.3 Derived state

Derived values should be computed centrally, for example:

```text
isOperational
isOfflineCapable
hasUnacknowledgedAlarm
canStartUpdate
canEditConfiguration
shouldDimBacklight
```

Do not repeat the same policy in multiple screens.

## 9. Event-to-state flow

Recommended flow:

```text
Driver event
  -> owning service
  -> normalized domain event
  -> application reducer or use case
  -> state-store update
  -> subscribed view-model invalidation
  -> bounded redraw
```

### 9.1 Event classes

- lifecycle events;
- measurement events;
- connectivity events;
- storage events;
- user-input events;
- command results;
- alarm transitions;
- security events;
- update events;
- diagnostic events.

### 9.2 Event metadata

Include:

```text
event type
event schema version
monotonic timestamp
wall-clock timestamp if trusted
source
correlation ID
priority
payload
```

## 10. Command architecture

Commands represent requested changes.

Examples:

```text
SetBacklightLevel
AcknowledgeAlarm
SilenceAlarm
SaveConfiguration
StartWiFiProvisioning
ConnectNetwork
ExportLogs
StartDiagnosticTest
RequestUpdate
ConfirmUpdate
CancelUpdate
RebootDevice
FactoryReset
```

### 10.1 Command envelope

```text
command ID
command type
origin
actor/session
requested time
parameters
authorization context
idempotency key
correlation ID
```

### 10.2 Command origins

- local touch UI;
- BOOT-button gesture;
- serial service console;
- authenticated local network client;
- authenticated remote backend;
- scheduled local policy;
- factory-test fixture;
- recovery workflow.

The same command should pass through the same policy regardless of origin.

## 11. Command validation stages

1. schema validation;
2. range and format validation;
3. current-mode validation;
4. capability validation;
5. authorization validation;
6. conflict and busy-state validation;
7. safety preconditions;
8. execution;
9. result verification;
10. state update and audit record.

A UI button must not bypass these stages.

## 12. Command results

Recommended result statuses:

```text
ACCEPTED
COMPLETED
REJECTED
FAILED
TIMED_OUT
CANCELLED
CONFLICT
UNAUTHORIZED
UNSUPPORTED
BLOCKED
IN_PROGRESS
```

Return structured reasons, not only user-facing text.

## 13. Screen model

Recommended screen categories:

- boot and splash;
- dashboard;
- detailed status;
- measurements/history;
- alarms;
- network;
- storage;
- settings;
- maintenance;
- diagnostics;
- update;
- provisioning;
- recovery;
- about/system information.

Each screen should define:

```text
screen ID
required state fields
required capabilities
allowed modes
entry conditions
exit conditions
refresh triggers
maximum redraw rate
input actions
fallback screen
security level
```

## 14. Navigation model

Use an explicit navigation state rather than ad hoc function calls.

Conceptual model:

```text
NavigationState
  rootScreen
  stack
  modal
  focus
  returnTarget
  timeoutPolicy
```

### 14.1 Navigation invariants

- a valid root screen always exists;
- modal depth is bounded;
- every modal has cancel or timeout behavior;
- unavailable screens are not entered;
- privileged screens close when authorization expires;
- recovery screens remain reachable without touch where required.

## 15. Dashboard design

The dashboard should prioritize current operational meaning rather than raw implementation detail.

Possible regions:

- primary process or measurement value;
- operating mode;
- active alarm summary;
- network state;
- storage state;
- time quality;
- update indicator;
- maintenance indicator.

Avoid showing `OK` merely because a subsystem object exists.

## 16. Status representation

Use text, icon, and shape in addition to color.

Recommended status vocabulary:

```text
NORMAL
ACTIVE
INACTIVE
STARTING
DEGRADED
OFFLINE
WARNING
FAULT
BLOCKED
UNKNOWN
NOT_CONFIGURED
NOT_PRESENT
```

Do not rely on green/red alone.

## 17. Touch input pipeline

```text
raw X/Y/Z
 -> pressure qualification
 -> filtering
 -> calibration transform
 -> rotation transform
 -> screen coordinate
 -> contact lifecycle
 -> gesture recognition
 -> hit testing
 -> intent
```

### 17.1 Contact states

- DOWN;
- MOVE;
- HOLD;
- UP;
- CANCELLED.

### 17.2 Gesture set

Keep the initial gesture set small:

- tap;
- press-and-hold;
- bounded drag;
- optional swipe after testing.

Complex multi-touch gestures do not apply to the resistive single-contact panel.

## 18. Resistive-touch UI constraints

Design for:

- lower positional precision than a modern capacitive panel;
- stylus or finger pressure variation;
- edge inaccuracy;
- enclosure preload;
- calibration drift;
- false contacts;
- one active contact only.

Use generous targets and spacing.

## 19. Touch target policy

Target dimensions must be validated on the physical 240 x 320 display.

Guidelines:

- avoid densely packed small controls;
- place destructive actions away from ordinary navigation;
- require confirmation for dangerous actions;
- provide visual pressed state;
- debounce repeated taps;
- ignore movement outside a bounded tolerance where appropriate;
- prevent one long contact from triggering repeated destructive commands.

No universal pixel minimum is claimed until tested with the actual panel, enclosure, and intended users.

## 20. Hit testing

Hit testing should use the final calibrated, rotated coordinate space.

A widget should receive:

```text
contact ID or sequence
screen coordinate
pressure/quality
contact phase
timestamp
```

Reject coordinates outside the active screen bounds.

## 21. Touch failure behavior

When touch is unavailable:

- display a clear indicator;
- keep status rendering active;
- allow essential recovery through serial or BOOT-button controls;
- avoid repeated calibration prompts;
- do not interpret random noise as authorization;
- preserve diagnostic raw data.

## 22. BOOT-button application input

GPIO0 may support limited runtime interaction after reset.

Possible semantic actions:

- short press: cycle status page;
- long press: open local menu;
- very long press: request service mode, with confirmation;
- press during boot: follow separately defined boot/service policy.

The runtime gesture must not be confused with bootloader entry.

## 23. UI rendering architecture

Recommended separation:

```text
ViewModel -> Screen Renderer -> Widget Renderer -> Display Adapter
```

Rendering code should consume values, not fetch hardware directly.

### 23.1 Retained versus immediate mode

A small embedded UI can use either:

- immediate-mode rendering with explicit invalidation;
- retained widgets with bounded object count.

The choice should be documented through an ADR and tested for memory fragmentation, redraw cost, and responsiveness.

## 24. Dirty-region rendering

To reduce SPI traffic:

- redraw only changed regions;
- group related updates;
- avoid full-screen clear on every sample;
- cap animation rates;
- defer low-priority visual changes during high-priority bus activity.

Dirty-region correctness must be tested to avoid stale pixels.

## 25. Frame scheduling

The UI does not need unrestricted frame rate.

Define classes such as:

```text
STATIC: redraw on state change
SLOW: 1-2 updates per second
NORMAL: bounded interactive updates
FAST: temporary animation or graph mode
```

Exact rates should be selected from measured SPI load and usability tests.

## 26. Display orientation

Orientation should be a declared configuration/profile value.

All of these must agree:

- TFT rotation;
- touch transform;
- layout dimensions;
- bitmap orientation;
- screenshot tests;
- enclosure orientation.

Changing rotation requires touch and layout regression.

## 27. Text and fonts

Font policy should define:

- included character sets;
- memory cost;
- fallback behavior;
- minimum readable sizes;
- truncation policy;
- numeric alignment;
- units rendering;
- localization coverage.

Do not assume a selected embedded font contains every translated character.

## 28. Numeric formatting

A measurement presentation must include:

- value;
- unit;
- validity or quality;
- age;
- optional trend;
- explicit unavailable state.

Avoid replacing missing data with zero.

Examples:

```text
23.4 C
--.- C   unavailable
23.4 C   stale 18 min
23.4 C   estimated
```

## 29. Data validity model

A data item may contain:

```text
value
unit
quality
source timestamp
receive timestamp
sequence
source
validity interval
error/status
```

Quality examples:

```text
VALID
STALE
ESTIMATED
OUT_OF_RANGE
SENSOR_FAULT
NOT_CONFIGURED
UNAVAILABLE
UNKNOWN
```

## 30. Data acquisition pipeline

Generic pipeline:

```text
hardware or remote source
 -> raw sample
 -> validation
 -> calibration
 -> unit conversion
 -> filtering/aggregation
 -> domain measurement
 -> alarm evaluation
 -> persistence policy
 -> state store
 -> UI/network publication
```

Each transformation should be testable independently.

## 31. Sampling policy

Define per source:

- nominal sampling interval;
- jitter tolerance;
- timeout;
- retry policy;
- minimum/maximum plausible value;
- calibration version;
- filter;
- persistence rate;
- UI publication rate;
- alarm evaluation rate.

Sampling, rendering, storage, and network publication rates need not be identical.

## 32. Filtering policy

Filtering may reduce noise but must not hide faults.

Document:

- filter type;
- window size or time constant;
- startup behavior;
- missing-sample behavior;
- saturation behavior;
- delay introduced;
- alarm input source: raw, filtered, or both.

## 33. Time-series history

History records should include:

```text
measurement ID
timestamp quality
value
unit or schema version
quality flags
source
```

### 33.1 Aggregation levels

Possible levels:

- raw recent samples;
- minute aggregates;
- hourly aggregates;
- daily aggregates.

Retention depends on application requirements and storage endurance.

## 34. Timestamp quality

A timestamp should carry quality:

```text
MONOTONIC_ONLY
UNSYNCHRONIZED
RTC_ESTIMATED
NETWORK_SYNCHRONIZED
AUTHENTICATED_SOURCE
```

Do not present unsynchronized time as trusted historical time.

## 35. Local persistence domains

Separate:

- identity;
- credentials;
- configuration;
- calibration;
- application state;
- event history;
- alarm history;
- measurement history;
- update state;
- diagnostic evidence.

Each domain needs its own schema and retention policy.

## 36. Persistence strategy

For each persisted object define:

```text
schema version
storage target
atomicity method
CRC/hash
write frequency
wear policy
backup/restore policy
migration policy
privacy class
factory-reset behavior
```

## 37. Configuration editing workflow

Recommended flow:

1. open a draft copy;
2. edit locally;
3. validate fields;
4. validate cross-field constraints;
5. show summarized changes;
6. authorize if required;
7. commit transactionally;
8. apply through owning services;
9. verify resulting state;
10. retain audit record;
11. roll back on failure.

Never write each keystroke or touch adjustment directly to persistent storage.

## 38. Configuration source precedence

Define precedence explicitly, for example:

```text
safe compiled defaults
validated persistent configuration
provisioning override
authorized temporary session override
```

Remote configuration must not silently override protected local safety limits.

## 39. Offline-first behavior

A network-capable application should define what remains available offline.

Possible offline capabilities:

- local display;
- local sensing;
- local control within declared safety policy;
- local alarm generation;
- bounded buffering;
- local configuration subset;
- maintenance access.

Possible unavailable functions:

- cloud-only commands;
- remote history;
- certificate renewal;
- external time synchronization;
- OTA download.

## 40. Network synchronization

Separate flows:

- telemetry upload;
- command receipt;
- configuration synchronization;
- time synchronization;
- update metadata;
- log export.

Each flow needs independent retry, authentication, and queue policy.

## 41. Outbound telemetry

Telemetry records should include:

```text
device identity
schema version
measurement/event ID
sequence
source time and quality
send time
value/status
firmware build
configuration version
```

### 41.1 Buffering

Define:

- maximum queued records;
- memory/storage location;
- overflow policy;
- reconnect upload ordering;
- deduplication behavior;
- privacy policy.

## 42. Inbound remote commands

Remote commands require:

- authenticated origin;
- authorization;
- freshness or expiry;
- unique command ID;
- replay protection;
- parameter validation;
- local mode compatibility;
- result acknowledgment;
- audit record.

A remote command must not directly invoke a widget callback or hardware driver.

## 43. Command conflict resolution

Potential conflicts:

- local user edits while remote configuration arrives;
- update requested during diagnostics;
- alarm active during power-saving request;
- storage export during card removal;
- network reconnect while provisioning UI is active.

Define policy using ownership, priority, mode, and explicit cancellation.

## 44. Idempotency

Commands likely to be retried should be idempotent where practical.

Examples:

```text
set backlight to 40%
acknowledge alarm ID 123
apply configuration revision 27
request update release X
```

Avoid ambiguous commands such as `toggle` across unreliable networks.

## 45. Alarm architecture

An alarm is not merely a colored banner.

Alarm record:

```text
alarm ID
rule ID
severity
state
source
first-active time
last-change time
current value
threshold/context
acknowledgment
silence state
clear condition
latching policy
```

## 46. Alarm states

Recommended states:

```text
INACTIVE
PENDING
ACTIVE_UNACKNOWLEDGED
ACTIVE_ACKNOWLEDGED
SILENCED
RETURNED_TO_NORMAL_UNACKNOWLEDGED
CLEARED
SUPPRESSED
UNKNOWN
```

## 47. Alarm severity

Example levels:

- INFO;
- NOTICE;
- WARNING;
- HIGH;
- CRITICAL.

Severity policy is application-specific and must not be inferred from color alone.

## 48. Alarm evaluation

Rules should define:

- input signal and quality requirements;
- threshold or condition;
- activation delay;
- clear delay;
- hysteresis;
- missing-data behavior;
- latching behavior;
- suppression dependencies;
- notification routes.

## 49. Missing-data alarms

A sensor that stops updating must not automatically appear normal.

Possible outcomes:

- dedicated `DATA_STALE` alarm;
- source fault alarm;
- dependent rule becomes `UNKNOWN`;
- controlled fail-safe action where required.

## 50. Alarm acknowledgment

Acknowledgment means the alarm was seen; it does not mean the condition cleared.

Record:

- actor;
- time quality;
- interface;
- alarm revision;
- optional comment.

## 51. Alarm silence

Silence should be bounded and visible.

Define:

- what output is silenced;
- duration;
- authorization;
- whether critical alarms can be silenced;
- reactivation behavior;
- audit record.

## 52. Local alarm presentation

Use:

- text;
- icon or shape;
- severity;
- active time;
- current value/context;
- acknowledgment state;
- navigation to details.

RGB LED or audio may supplement the TFT but must use owning services and respect active-LOW and BTL constraints.

## 53. Notification routing

Potential routes:

- TFT banner;
- RGB indicator;
- audio pattern;
- serial log;
- local network message;
- remote backend;
- retained event history.

Each route can independently fail; the alarm state must remain authoritative.

## 54. User roles and sessions

Possible roles:

```text
VIEWER
OPERATOR
MAINTAINER
PROVISIONER
ADMINISTRATOR
FACTORY
```

Role names and permissions are application policy, not a substitute for authentication.

### 54.1 Session state

Track:

- authenticated role;
- authentication method;
- start time;
- expiry;
- inactivity timeout;
- allowed operations;
- local physical presence if required.

## 55. Privileged local actions

Examples:

- changing network credentials;
- starting update;
- clearing history;
- entering diagnostics;
- exporting sensitive logs;
- factory reset;
- decommissioning.

Require explicit confirmation and, when appropriate, authentication or physical presence.

## 56. Confirmation dialogs

A safe confirmation should state:

- action;
- scope;
- consequences;
- affected data;
- reversibility;
- current power/network prerequisites;
- cancel path.

Avoid generic `Are you sure?` for destructive actions.

## 57. First-run workflow

A possible first-run sequence:

1. verify hardware profile;
2. show identity and firmware build;
3. select language/orientation if policy permits;
4. verify touch calibration;
5. establish provisioning method;
6. configure network;
7. set time policy;
8. configure application essentials;
9. run validation tests;
10. show final summary;
11. activate only after successful commit.

First-run should remain resumable after power loss.

## 58. Provisioning UI

Provisioning UI must not display secrets after entry unless explicitly necessary.

Features:

- masked input;
- clear error reasons without revealing secret values;
- timeout;
- cancel and restart;
- backend enrollment progress;
- verification result;
- receipt or non-secret identifier.

## 59. Network setup UI

Show separate states:

```text
radio unavailable
scanning
network selected
authenticating
associated
DHCP pending
connected locally
Internet unavailable
service unavailable
```

Do not collapse every failure into `Wi-Fi failed`.

## 60. Update UI

Update screens should show:

- current release;
- candidate release;
- trust/verification status;
- compatibility status;
- download progress;
- restart requirement;
- trial-boot status;
- rollback status;
- blocking reason.

The UI must not label an image `verified` before signature and policy checks complete.

## 61. Maintenance UI

Maintenance may expose:

- health summary;
- lifecycle state;
- test history;
- calibration;
- log export;
- storage status;
- network diagnostics;
- firmware identity;
- service-mode request.

Privileged operations should be separated from read-only observation.

## 62. Diagnostic UI integration

The application should hand control to the diagnostic framework rather than duplicate test logic.

A diagnostic screen displays:

- test ID;
- progress;
- preconditions;
- result;
- raw evidence reference;
- blocked dependencies;
- exit/recovery action.

## 63. Recovery UI

Recovery UI must remain simple and robust.

Possible options:

- export evidence;
- restore configuration;
- roll back firmware;
- enter serial recovery;
- reboot;
- authorized reset.

Do not depend on network or touch exclusively.

## 64. Backlight policy

Backlight policy may use:

- user setting;
- application mode;
- inactivity;
- alarm state;
- maintenance mode;
- external light measurement only after validated calibration.

GPIO21 ownership remains with the backlight service.

## 65. Inactivity and screen saver

Define:

- inactivity source;
- dim timeout;
- blank timeout;
- wake input;
- alarm override;
- update/maintenance override;
- burn-in mitigation if relevant.

Blank display must not imply the device is off.

## 66. Accessibility

Even on a small embedded screen, include:

- readable contrast;
- text plus icons;
- non-color-only status;
- consistent navigation;
- sufficiently large touch targets;
- visible focus/pressed state;
- adjustable brightness;
- clear units;
- timeout behavior that does not erase work unexpectedly.

## 67. Color policy

Define semantic colors centrally.

Examples:

- normal;
- information;
- warning;
- critical;
- disabled;
- selected;
- background;
- unknown.

Test with grayscale or color-vision simulations where practical.

## 68. Localization architecture

Use stable message IDs rather than source text as keys.

```text
ui.dashboard.title
alarm.storage.write_failed
network.state.dhcp_pending
update.action.install
```

### 68.1 Translation bundle

Each bundle should declare:

- locale;
- bundle version;
- firmware compatibility;
- character coverage;
- fallback locale;
- plural/format support if used.

## 69. Localization constraints

Test:

- longer strings;
- missing translations;
- unsupported glyphs;
- unit placement;
- decimal separators;
- date/time format;
- text clipping;
- right-to-left support only if deliberately implemented.

Do not claim localization support from translating a few labels.

## 70. Units and locale

Keep internal units stable. Convert only at presentation or explicit interface boundaries.

Store configuration values with declared units and schema, not localized strings.

## 71. Help and explainability

For important states, provide concise explanations:

- why a function is unavailable;
- why a test is blocked;
- why an update cannot start;
- why time is untrusted;
- why data is stale;
- what a factory reset will clear.

## 72. Error presentation

User-facing errors should include:

- concise summary;
- actionable next step;
- stable reference/error code;
- optional detail page;
- no secrets.

Avoid raw stack traces on ordinary screens.

## 73. Retry UX

A retry action should state what is retried and whether data may be duplicated or overwritten.

Automatic retries should show bounded progress rather than an endless spinner.

## 74. Progress reporting

Long operations should expose:

- phase;
- progress if measurable;
- elapsed time;
- cancellation support;
- whether cancellation is safe;
- last error;
- recovery action.

## 75. Cancellation

Cancellation is a command, not an assumption.

A subsystem may respond:

```text
CANCELLED
CANCEL_PENDING
NOT_CANCELLABLE
ALREADY_COMPLETED
```

## 76. Application logging

Log semantic application events:

- mode transition;
- user command;
- command rejection;
- configuration commit;
- alarm transition;
- navigation to privileged screen;
- update decision;
- recovery action.

Do not log every touch coordinate in production unless needed and bounded.

## 77. Application metrics

Possible metrics:

- UI event latency;
- render duration;
- queue depth;
- dropped low-priority events;
- command completion time;
- configuration failures;
- network buffer depth;
- alarm counts;
- stale-data durations;
- screen transition errors.

## 78. Performance budgets

Define budgets for:

- touch-to-feedback latency;
- screen-transition latency;
- maximum redraw duration;
- event queue occupancy;
- state snapshot size;
- history query time;
- configuration commit time.

Values require measurement on the target firmware and board.

## 79. Memory policy

UI and application code should avoid uncontrolled allocation during steady operation.

Review:

- font memory;
- bitmap assets;
- screen object count;
- history buffers;
- network queues;
- JSON document sizes;
- localization bundles;
- fragmentation after repeated navigation.

## 80. Asset management

Images, fonts, and translations should be versioned and included in release manifests.

For each asset define:

- source/license;
- dimensions;
- encoding;
- memory location;
- checksum;
- fallback behavior.

## 81. Screen testability

Screen renderers should run against fake view models without real hardware.

Possible test outputs:

- command list sent to a fake display;
- framebuffer snapshot;
- pixel hash with controlled fonts;
- semantic widget tree;
- screenshot artifact.

Pixel hashes may be sensitive to library versions; semantic assertions should accompany them.

## 82. Touch testability

Test sequences:

```text
tap inside target
tap at boundary
tap outside target
press and hold
move out then release
noisy coordinate sequence
low-pressure contact
stuck contact
touch during screen transition
```

## 83. Navigation tests

Verify:

- every reachable screen can exit;
- privileged screens enforce session policy;
- modal depth is bounded;
- back action is deterministic;
- mode change redirects correctly;
- missing capability prevents entry;
- touch failure leaves recovery route.

## 84. State-reducer tests

Given an initial state and event/command result, test the exact next state.

These tests should not require TFT, Wi-Fi, or storage hardware.

## 85. Command-policy tests

Test:

- wrong mode;
- unauthorized actor;
- unsupported capability;
- duplicate command ID;
- stale remote command;
- busy conflict;
- invalid parameters;
- service failure;
- timeout;
- successful completion.

## 86. Alarm tests

For each rule test:

- below threshold;
- boundary;
- above threshold;
- activation delay;
- clear delay;
- hysteresis;
- missing data;
- stale data;
- acknowledgment;
- silence expiry;
- reboot persistence;
- time-quality change.

## 87. Persistence tests

Test:

- valid load;
- missing object;
- bad CRC;
- unknown schema;
- migration;
- power loss before commit;
- power loss during commit;
- full storage;
- read-only storage;
- backup restore;
- secret redaction.

## 88. Network-flow tests

Test:

- offline startup;
- delayed DHCP;
- DNS failure;
- service authentication failure;
- reconnect;
- duplicate telemetry acknowledgment;
- replayed command;
- queue overflow;
- clock not synchronized;
- backend unavailable during update check.

## 89. Update workflow tests

Test:

- no update;
- incompatible hardware profile;
- invalid signature;
- insufficient space;
- low-power policy block;
- network interruption;
- reboot during download;
- trial boot success;
- trial boot failure;
- rollback;
- configuration migration failure.

## 90. Application-level fault injection

Safe fault injection may simulate:

- display unavailable;
- touch unavailable;
- stale measurement;
- storage full;
- network loss;
- untrusted time;
- configuration corruption;
- command timeout;
- queue overflow;
- alarm route failure.

Simulation should be clearly marked and impossible to confuse with a real fault in retained evidence.

## 91. Hardware-in-the-loop UI tests

Possible HIL tests:

- display color/orientation reference screen;
- physical touch target grid;
- repeated navigation;
- backlight transitions;
- BOOT-button navigation;
- enclosure-installed touch behavior;
- long-duration dashboard refresh;
- combined TFT/touch/SD/network load.

## 92. Screenshot and evidence policy

For UI acceptance retain:

- firmware build;
- board profile;
- locale;
- orientation;
- state fixture;
- screenshot or photograph;
- expected result;
- test operator or automation ID.

## 93. Application acceptance levels

Suggested levels:

| Level | Evidence |
|---|---|
| A0 | architecture documented |
| A1 | host state and command tests |
| A2 | fake-display and touch tests |
| A3 | basic target UI demonstration |
| A4 | persistence/network/alarm integration |
| A5 | degraded and recovery workflows |
| A6 | HIL and enclosure validation |
| A7 | field workflow evidence |

A successful demo screen is not A7.

## 94. Release gate

Before an application release:

1. board profile is declared;
2. application mode transitions are reviewed;
3. configuration schema and migrations are tested;
4. alarms are versioned and tested;
5. privileged commands enforce policy;
6. UI works without network where required;
7. recovery remains reachable;
8. secrets are absent from UI/log artifacts;
9. touch calibration/orientation are compatible;
10. display and memory budgets are measured;
11. persistence fault tests pass;
12. remote command replay tests pass where applicable;
13. update workflow is tested if included;
14. localization bundles pass glyph/layout checks;
15. release manifest includes application assets;
16. known limitations are documented.

## 95. Suggested code organization

```text
src/app/
  Application.cpp
  ApplicationState.cpp
  ApplicationMode.cpp
  CommandRouter.cpp
  CommandPolicy.cpp
  EventReducer.cpp
  AlarmEngine.cpp
  ConfigurationWorkflow.cpp
  TelemetryFlow.cpp

src/ui/
  Navigation.cpp
  UiController.cpp
  ViewModels.cpp
  screens/
  widgets/
  themes/
  localization/

src/domain/
  Measurements.cpp
  Commands.cpp
  Events.cpp
  Alarms.cpp

src/persistence/
  ApplicationStore.cpp
  HistoryStore.cpp

src/network/
  TelemetryClient.cpp
  RemoteCommandClient.cpp
```

The exact structure may change; ownership boundaries are more important than folder names.

## 96. Migration from the current self-test

### Stage 1 — extract the view model

Move displayed LDR and BOOT values into a small state structure.

### Stage 2 — separate rendering

Create a renderer that consumes the state and does not read GPIO.

### Stage 3 — normalize input

Convert BOOT and future touch activity into events/intents.

### Stage 4 — add navigation

Introduce dashboard and system-information screens.

### Stage 5 — add command routing

Route backlight and RGB requests through owning services.

### Stage 6 — add persistence

Introduce versioned configuration with draft/validate/commit flow.

### Stage 7 — add alarms and quality

Represent stale and unavailable data explicitly.

### Stage 8 — add network flows

Keep telemetry and commands independent.

### Stage 9 — integrate diagnostics and update

Use existing service frameworks, not duplicate logic.

### Stage 10 — automate application tests

Add host state, navigation, alarm, persistence, and screenshot tests to CI.

## 97. Unsupported claims

Until implemented and tested, do not claim:

- complete application framework implementation;
- deterministic touch latency;
- accessible UI for all users;
- full localization;
- lossless offline telemetry buffering;
- transactional persistence for every power-loss point;
- authenticated remote commands;
- complete alarm coverage;
- guaranteed history retention;
- automatic conflict-free local/remote editing;
- production update UX;
- pixel-perfect rendering across library versions;
- portability across all CYD revisions;
- field-proven user workflows.

## 98. Completion criteria

This application architecture can be considered sample-qualified when:

1. application state is centralized and versioned;
2. modes and legal transitions are implemented;
3. views do not read hardware directly;
4. commands use common validation regardless of origin;
5. navigation is bounded and recoverable;
6. touch input follows raw-to-intent processing;
7. display rotation and touch mapping are tested together;
8. missing and stale data are explicit;
9. persistence domains and schemas are separated;
10. configuration editing is transactional;
11. offline behavior is declared and tested;
12. telemetry and command flows are independent;
13. remote commands have replay and authorization controls where used;
14. alarm lifecycle and acknowledgment are implemented;
15. critical actions require meaningful confirmation;
16. diagnostic and recovery screens use lower-level services;
17. localization has fallback and glyph validation;
18. accessibility does not depend on color alone;
19. host-side state, command, navigation, and alarm tests exist;
20. target HIL tests exercise TFT, touch, BOOT, and degraded modes;
21. memory and latency budgets are measured;
22. release evidence and known limitations are retained.

## 99. Open implementation tasks

- Define the first real application domain and its primary dashboard.
- Introduce `ApplicationState` and immutable snapshots.
- Define application modes and reducers.
- Add typed intents and command routing.
- Add a navigation stack with bounded modals.
- Implement touch contact lifecycle and hit testing.
- Add data-quality and stale-value representation.
- Add versioned configuration draft/commit workflow.
- Define local history requirements and storage budget.
- Implement an alarm engine with hysteresis and acknowledgment.
- Define offline operation and telemetry overflow policy.
- Add secure remote command envelopes only when backend requirements exist.
- Define theme tokens and localization message IDs.
- Add fake display, fake touch, and fake persistence tests.
- Add screenshot or semantic rendering evidence to CI.
- Perform enclosure-installed usability testing.

## 100. Summary

The application layer should be a policy-driven consumer of the services defined in Part 21. It should transform normalized events into authoritative state, transform validated commands into controlled state changes, and render that state through a recoverable, touch-aware, degraded-mode-capable UI.

Its central rule is:

> the display presents state; it does not define state, authorize commands, or prove that an operation succeeded.

A production application is not complete because its dashboard looks polished. It is complete only when state, data quality, commands, persistence, alarms, security, recovery, navigation, accessibility, and tests remain coherent under normal, offline, degraded, and failure conditions.