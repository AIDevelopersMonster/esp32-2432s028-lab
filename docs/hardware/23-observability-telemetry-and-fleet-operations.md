# Observability, Telemetry and Fleet Operations — ESP32-2432S028

> **Document status:** sample-specific operational architecture and implementation plan for the author's `ESP32-2432S028` platform.  
> **Scope:** device observability, structured telemetry, health summaries, inventory, remote diagnostics, log transport, dashboards, alert routing, privacy, retention, rollout observation, capacity planning and operational runbooks.  
> **Evidence rule:** a device is not considered observable merely because it prints messages to UART or periodically sends values. Operational claims require defined signals, schemas, freshness rules, loss accounting, privacy classification, storage policy, dashboards, alerts, response procedures and validation under failure.

## 1. Purpose

A deployed device may continue to appear powered while its application, storage, networking, sensors, display, touch path or update state has degraded. Fleet operation therefore needs more than a binary online/offline flag.

This chapter defines how a future ESP32-2432S028 system should answer:

- which physical board is reporting;
- which hardware and firmware profile it runs;
- whether the report is fresh and trustworthy;
- which subsystems are healthy, degraded, unavailable or unknown;
- whether data was lost, delayed, duplicated or reordered;
- whether an update improved or harmed the fleet;
- when an operator should investigate;
- which evidence is safe to collect remotely;
- what must remain local or redacted;
- how incidents, maintenance and replacement decisions are supported.

The design complements Parts 17–22. It does not claim that the current self-test already implements fleet telemetry.

## 2. Relationship to earlier chapters

Observability depends on previously defined controls:

- Part 13 defines wireless behavior and antenna constraints;
- Part 16 defines bring-up and acceptance evidence;
- Part 17 defines lifecycle records and maintenance;
- Part 18 defines diagnostics and recovery artifacts;
- Part 19 defines lot, revision and asset traceability;
- Part 20 defines identity, credentials, update trust and incident security;
- Part 21 defines services, events, ownership, logging and configuration;
- Part 22 defines application state, data quality, commands, alarms and workflows.

Fleet telemetry must not bypass security, duplicate authoritative state, or create hidden hardware ownership.

## 3. Current repository baseline

The current firmware provides local serial output for LDR and BOOT state and a simple TFT self-test. This is useful development evidence, but it does not establish:

- stable machine-readable schemas;
- persistent device identity;
- remote health reporting;
- authenticated telemetry transport;
- queue and loss accounting;
- fleet inventory;
- dashboard coverage;
- remote diagnostics authorization;
- privacy classification;
- retention control;
- alert routing;
- rollout observation;
- service-level objectives.

These remain implementation tasks.

## 4. Operational objectives

Recommended objectives:

1. identify every managed physical device and its current software/hardware profile;
2. distinguish healthy, degraded, failed, offline and unknown states;
3. detect regressions after releases or configuration changes;
4. preserve enough evidence to diagnose intermittent failures;
5. avoid making the telemetry path a cause of application failure;
6. account explicitly for dropped, delayed and duplicated records;
7. minimize sensitive data collection;
8. support safe remote triage without silently granting remote control;
9. connect fleet evidence to maintenance, incident and revision records;
10. keep schemas and operational semantics versioned;
11. maintain bounded CPU, memory, Flash, network and backend cost;
12. allow local operation when telemetry infrastructure is unavailable.

## 5. Non-objectives and limitations

Unless separately implemented and verified, this architecture does not imply:

- continuous connectivity;
- guaranteed delivery of every telemetry record;
- exact wall-clock time at every boot;
- calibrated voltage, current or temperature measurements from the unmodified board;
- complete remote diagnosis of physical faults;
- remote repair of arbitrary failures;
- safe remote execution of unrestricted commands;
- tamper-proof logs;
- regulatory monitoring compliance;
- unlimited history retention;
- automatic root-cause determination;
- fault-free fleet dashboards;
- equivalent behavior across all CYD revisions.

A missing signal must be reported as unknown, not inferred as normal.

## 6. Observability model

The system should use four complementary signal classes:

| Signal class | Purpose | Examples |
|---|---|---|
| metrics | bounded numerical state and rates | uptime, heap minimum, reconnect count |
| events | discrete state transitions | boot, SD mount failure, update rollback |
| logs | diagnostic narrative with context | driver error, command rejection |
| traces or operation records | lifecycle of important multi-step operations | update, provisioning, configuration commit |

Not every embedded deployment needs a full distributed tracing system. A compact operation ID and step history may be sufficient.

## 7. Signal-quality attributes

Every signal should declare:

- source subsystem;
- schema version;
- device identity;
- boot/session identity;
- sequence number where applicable;
- monotonic timestamp;
- wall-clock timestamp and quality if available;
- severity or quality;
- units;
- aggregation period;
- whether it is sampled;
- privacy class;
- retention class;
- loss behavior;
- firmware and hardware profile.

A value without unit, timestamp quality or source context is operationally ambiguous.

## 8. Device and session identity

Recommended identity fields:

```text
asset_id
logical_device_id
hardware_profile_id
board_revision_fingerprint
procurement_lot_id
firmware_release_id
configuration_revision
calibration_revision
boot_id
session_id
```

### 8.1 Boot ID

Generate a new `boot_id` for each successful start of the application image. It permits separation of records from different resets even when wall-clock time is uncertain.

### 8.2 Session ID

A telemetry transport session may reconnect several times within one boot. Use a separate session identifier if the backend needs connection-level accounting.

### 8.3 Identity conflict

If two active connections claim the same logical identity but disagree on immutable or assigned identifiers, flag an identity conflict. Do not silently merge their streams.

## 9. Health-state model

Recommended top-level states:

```text
HEALTHY
DEGRADED
FAILED
MAINTENANCE
QUARANTINED
OFFLINE_EXPECTED
OFFLINE_UNEXPECTED
UNKNOWN
DECOMMISSIONED
```

### 9.1 HEALTHY

All required subsystems meet declared operational criteria, telemetry is sufficiently fresh, and no active blocking fault exists.

### 9.2 DEGRADED

The device remains useful but one or more noncritical capabilities are unavailable, performance is below target, or evidence quality is reduced.

### 9.3 FAILED

A required application function is unavailable or safety/security policy requires shutdown.

### 9.4 UNKNOWN

Evidence is insufficient. Unknown must not be automatically converted to healthy.

## 10. Subsystem health contract

Each subsystem should publish a compact health record:

```text
subsystem_id
state
reason_code
since_monotonic_ms
last_success_monotonic_ms
error_count_window
last_error_code
confidence
```

Suggested subsystem IDs:

- power and reset evidence;
- program Flash and partitions;
- NVS/configuration;
- TFT;
- backlight;
- touch;
- microSD;
- auxiliary Flash;
- RGB indicator;
- LDR;
- BOOT button;
- audio;
- Wi-Fi;
- Bluetooth;
- time synchronization;
- telemetry transport;
- update service;
- application domain.

## 11. Health aggregation

Top-level health should be derived from declared policy rather than a simple worst-state rule.

Example:

```text
required subsystem failed -> FAILED
required subsystem unknown beyond grace period -> DEGRADED or FAILED by profile
optional subsystem failed -> DEGRADED
maintenance mode -> MAINTENANCE
security quarantine -> QUARANTINED
all required checks pass -> HEALTHY
```

The aggregation policy must be versioned and included in the device profile or backend configuration.

## 12. Heartbeat design

A heartbeat should be small, periodic and bounded.

Recommended content:

- identity fields;
- firmware/configuration versions;
- boot ID;
- uptime;
- top-level health;
- compact subsystem bitmap or summary;
- last successful telemetry sequence;
- queue depth and dropped-record counters;
- time quality;
- update channel and update state;
- free heap and minimum free heap;
- reset reason from current boot;
- connectivity quality summary;
- active alarm count.

Do not include large logs or secrets in a heartbeat.

## 13. Freshness and liveness

The backend should classify heartbeat freshness using deployment-specific thresholds:

```text
FRESH
LATE
STALE
MISSING
EXPECTED_OFFLINE
```

Thresholds must account for:

- normal heartbeat interval;
- sleep policy;
- network backoff;
- maintenance windows;
- expected site outages;
- transport batching;
- clock uncertainty.

A late heartbeat alone does not prove device failure.

## 14. Metrics taxonomy

Use stable names and explicit units.

Recommended groups:

### 14.1 Runtime

- `device_uptime_seconds`;
- `boot_count_total`;
- `unexpected_reset_count_total`;
- `watchdog_reset_count_total`;
- `brownout_reset_count_total`;
- `safe_mode_entry_count_total`;
- `main_loop_lag_max_ms` or scheduler progress indicators.

### 14.2 Memory

- current free heap bytes;
- minimum free heap bytes;
- largest free block bytes if available;
- allocation failure count;
- queue high-water marks;
- stack high-water marks for critical tasks.

### 14.3 Storage

- SD mounted state;
- SD mount failures;
- filesystem errors;
- bytes pending for upload;
- local log utilization;
- telemetry spool utilization;
- configuration commit failures;
- NVS write failures;
- Flash operation failures.

### 14.4 Network

- Wi-Fi connection state;
- reconnect count;
- association failures;
- DHCP failures;
- DNS failures;
- transport connection failures;
- bytes transmitted and received;
- telemetry acknowledgments;
- estimated RSSI summary where meaningful;
- offline duration.

### 14.5 UI and input

- display initialization failures;
- render error count;
- frame or update latency summaries;
- touch calibration validity;
- touch sample rejection count;
- touch inactivity caused by failure;
- BOOT fallback use count.

### 14.6 Application

Application metrics depend on the final product and must not be invented at the board-reference level.

## 15. Counter semantics

Counters should be monotonic within a boot or persisted only where justified.

Declare whether each counter:

- resets on boot;
- persists across boot;
- saturates;
- wraps;
- is sampled;
- may be lost before upload.

Persisting every counter update may create unnecessary Flash wear.

## 16. Histograms and summaries

For latency, duration and size distributions, avoid transmitting every sample.

Possible compact approaches:

- fixed buckets;
- minimum/maximum/mean/count;
- rolling quantile approximation;
- exponentially decayed summaries.

Bucket definitions and aggregation windows must be versioned.

## 17. Event model

An operational event should contain:

```text
event_id
event_type
event_schema_version
severity
device_id
boot_id
sequence
monotonic_time
wall_time
wall_time_quality
subsystem
reason_code
context_fields
```

Important event categories:

- boot and shutdown;
- reset reason;
- mode transition;
- subsystem state transition;
- storage mount/unmount;
- network connect/disconnect;
- alarm transition;
- command receipt/result;
- configuration change;
- provisioning state change;
- update lifecycle;
- recovery action;
- diagnostic suite result;
- security event;
- maintenance and decommissioning.

## 18. Event severity

Recommended levels:

```text
DEBUG
INFO
NOTICE
WARNING
ERROR
CRITICAL
```

Severity must represent operational impact, not developer frustration.

Examples:

- normal Wi-Fi reconnect after short outage: `NOTICE`;
- repeated reconnect exhaustion: `WARNING` or `ERROR`;
- verified update rollback: `ERROR`;
- credential compromise indication: `CRITICAL`.

## 19. Structured logging

Logs should use structured fields internally, even if rendered as human-readable serial lines.

Recommended fields:

- timestamp and quality;
- severity;
- component;
- event or message code;
- boot ID;
- correlation or operation ID;
- bounded message;
- typed context;
- source firmware version.

Avoid parsing essential facts from free-form text.

## 20. Log redaction

Never emit plaintext:

- Wi-Fi passwords;
- private keys;
- API tokens;
- complete provisioning secrets;
- update signing keys;
- recovery secrets;
- sensitive user data;
- unredacted authorization headers.

Identifiers may also require pseudonymization depending on deployment.

## 21. Local log buffering

Local logs should use bounded storage with explicit policy.

Possible tiers:

```text
RAM recent ring
persistent critical-event journal
optional microSD extended log
upload spool
```

### 21.1 Ring behavior

Define:

- record count or byte capacity;
- overwrite policy;
- critical-event reservation;
- corruption detection;
- boot recovery;
- wear considerations;
- export procedure.

### 21.2 microSD caution

Removable storage is not trusted or guaranteed present. The device must remain operational without it unless the product explicitly requires it.

## 22. Crash and reset evidence

Before risky initialization, record a boot-stage marker as described in Part 18.

A crash bundle may include:

- reset reason;
- previous boot ID;
- last boot stage;
- recent critical events;
- task or subsystem progress markers;
- update state;
- free-heap minimum;
- configuration revision;
- firmware release;
- integrity status of retained evidence.

Do not claim a complete core dump unless actually implemented and safely stored.

## 23. Telemetry envelope

Recommended envelope:

```json
{
  "schema": "cyd.telemetry.v1",
  "device_id": "...",
  "boot_id": "...",
  "sequence": 123,
  "created_monotonic_ms": 456789,
  "wall_time": "...",
  "wall_time_quality": "SYNCED",
  "records": []
}
```

The exact encoding may be JSON, CBOR, protobuf or another controlled format. Choice should consider code size, memory, backend tooling and schema evolution.

## 24. Sequence and duplicate handling

Use sequence numbers per boot or stream.

The backend should tolerate:

- duplicates after retry;
- delayed batches;
- gaps caused by queue overflow;
- out-of-order delivery;
- reboot sequence reset when boot ID changes.

Device and backend must expose observed gaps rather than hiding them.

## 25. Delivery guarantees

Possible semantics:

- best effort;
- at-most-once;
- at-least-once;
- application-confirmed delivery.

For small embedded fleets, at-least-once with idempotent backend ingestion is often practical. It still does not guarantee retention if local buffers overflow.

The chosen guarantee must be documented per signal class.

## 26. Batching and compression

Batching can reduce network and power overhead but increases delay and memory use.

Control:

- maximum batch records;
- maximum encoded bytes;
- maximum age;
- high-severity immediate flush;
- retry backoff;
- compression threshold;
- CPU and heap budget.

Compression should not be enabled solely by assumption; measure its cost and benefit.

## 27. Offline spool

Recommended spool states:

```text
EMPTY
ACTIVE
NEAR_CAPACITY
FULL_DROPPING_LOW_PRIORITY
FULL_BLOCKED
CORRUPT
```

The policy should prioritize:

1. security and critical recovery evidence;
2. update and configuration results;
3. alarms and failures;
4. health transitions;
5. routine metrics;
6. debug logs.

A full spool must not deadlock the main application.

## 28. Loss accounting

Track at least:

- dropped records by priority;
- dropped bytes;
- queue overflow count;
- spool overwrite count;
- encoding failures;
- transmission failures;
- backend rejection count;
- sequence gaps detected locally if applicable.

A system that drops data silently is not operationally trustworthy.

## 29. Transport security

Telemetry transport should follow Part 20:

- authenticate the device or connection;
- authenticate the destination;
- protect integrity;
- protect confidentiality where data requires it;
- validate certificates and time according to policy;
- support credential rotation and revocation;
- reject unauthenticated remote commands;
- avoid embedding production secrets in firmware source.

Transport encryption alone does not make telemetry content safe to collect.

## 30. Transport independence

The telemetry model should not be coupled directly to one protocol.

Possible adapters:

- HTTPS batch upload;
- MQTT publish;
- WebSocket session;
- local serial export;
- microSD diagnostic bundle;
- service-tool transfer.

All should use the same logical schemas where practical.

## 31. Fleet inventory

The fleet registry should record:

- asset ID;
- logical device ID;
- hardware profile;
- board fingerprint;
- supplier lot;
- location or assignment subject to privacy policy;
- owner/operator group;
- provisioning state;
- credential status;
- current firmware;
- desired firmware/channel;
- configuration revision;
- last contact;
- health state;
- maintenance status;
- quarantine or decommission status.

The registry is authoritative for fleet membership; telemetry alone must not automatically enroll unknown devices.

## 32. Desired versus reported state

Separate:

```text
desired firmware/configuration/channel
reported firmware/configuration/channel
```

A mismatch may indicate:

- rollout in progress;
- offline device;
- failed update;
- manual local change;
- stale backend state;
- unauthorized modification.

Do not resolve mismatches by silently overwriting evidence.

## 33. Fleet groups

Useful grouping dimensions:

- hardware profile;
- procurement lot;
- firmware release;
- update channel;
- deployment site;
- application role;
- maintenance cohort;
- security profile;
- experimental canary group.

Groups should be explicit and auditable because they influence rollout and alert policy.

## 34. Dashboard hierarchy

Recommended dashboard layers:

### 34.1 Fleet overview

- managed device count;
- fresh/late/stale/offline counts;
- health-state distribution;
- firmware distribution;
- active critical incidents;
- rollout status;
- telemetry ingestion health.

### 34.2 Cohort view

- hardware-profile comparison;
- lot-specific failure rate;
- release-specific reset rate;
- reconnect and storage error trends;
- alarm distribution.

### 34.3 Device view

- identity and lifecycle status;
- latest health summary;
- timeline of events;
- firmware/configuration history;
- recent resets;
- subsystem details;
- active alarms;
- maintenance and diagnostic records.

### 34.4 Operation view

- provisioning transaction;
- configuration rollout;
- update attempt;
- recovery operation;
- remote diagnostic session.

## 35. Dashboard cautions

A dashboard must show:

- data freshness;
- sample size;
- unknown values;
- aggregation window;
- excluded devices;
- schema changes;
- time quality;
- data loss or ingestion gaps.

A green chart based on stale or partial data is misleading.

## 36. Alert design

Alerts should represent actionable conditions.

Each alert rule should define:

- signal source;
- scope;
- threshold or condition;
- duration;
- severity;
- suppression and deduplication;
- expected operator action;
- runbook;
- owner;
- escalation;
- recovery condition;
- test evidence.

## 37. Suggested alert classes

### 37.1 Device-level

- repeated unexpected resets;
- update rollback;
- persistent storage failure;
- telemetry spool near full;
- required subsystem failed;
- credential rejected;
- prolonged unknown state.

### 37.2 Cohort-level

- reset rate increased after release;
- one procurement lot has abnormal failures;
- one hardware profile loses touch or SD functionality;
- Wi-Fi reconnection failures rise at a site;
- update success rate falls below gate.

### 37.3 Platform-level

- telemetry ingestion unavailable;
- backend rejection spike;
- certificate expiration risk;
- dashboard data stale;
- command service unavailable;
- signing or release pipeline incident.

## 38. Alert fatigue control

Use:

- minimum duration;
- hysteresis;
- deduplication;
- grouping by root symptom;
- maintenance suppression;
- expected-offline state;
- escalation only when action is needed;
- periodic review of noisy rules.

Do not suppress evidence permanently to make dashboards appear healthy.

## 39. Operational service indicators

Potential indicators:

- heartbeat freshness rate;
- healthy-device percentage;
- unexpected reset rate;
- update success and rollback rate;
- command completion rate;
- telemetry delivery lag;
- backend rejection rate;
- unresolved critical-alarm age;
- mean time to acknowledge;
- mean time to restore;
- maintenance recurrence rate.

Targets must be based on actual product requirements and measured baselines, not invented here.

## 40. Release and rollout observation

Every rollout should link:

- release ID;
- source commit;
- build manifest;
- hardware profiles allowed;
- rollout channel;
- cohort membership;
- start and stop times;
- success criteria;
- rollback criteria;
- observed metrics;
- operator decisions.

### 40.1 Canary phase

Start with a small representative cohort, including relevant hardware revisions.

### 40.2 Expansion gates

Expand only if declared metrics remain within thresholds.

### 40.3 Automatic pause

Possible pause triggers:

- elevated reset rate;
- update failure or rollback rate;
- loss of required subsystem;
- severe application alarm increase;
- telemetry silence after update;
- security verification failure.

Automatic rollback is a separate policy and must not be assumed safe for every failure.

## 41. Configuration rollout observation

Configuration changes can be as risky as firmware changes.

Track:

- desired revision;
- applied revision;
- validation result;
- migration result;
- restart requirement;
- rollback result;
- changed parameters excluding secrets;
- operator or automation identity.

## 42. Remote diagnostics boundary

Remote diagnostics should expose predefined, bounded operations rather than arbitrary code or unrestricted shell access.

Examples:

- request health snapshot;
- request recent event bundle;
- run QUICK diagnostic suite;
- test one subsystem using safe profile;
- export configuration metadata without secrets;
- request reboot with reason;
- enter approved maintenance mode.

Every remote action requires authorization, audit logging and timeout.

## 43. Remote diagnostic session

Session record:

```text
session_id
device_id
requester_identity
authorization_scope
start_time
commands
results
artifacts
end_reason
```

The device should visibly or operationally indicate maintenance/diagnostic mode where appropriate.

## 44. Command observability

For each remote or local administrative command, record:

- command ID;
- origin;
- authenticated principal;
- authorization decision;
- receipt time;
- validation result;
- execution start/end;
- result code;
- side-effect summary;
- resulting configuration or mode revision.

Do not log secret command payloads in plaintext.

## 45. Privacy classification

Recommended classes:

```text
PUBLIC
INTERNAL_OPERATIONAL
SENSITIVE_OPERATIONAL
PERSONAL
SECRET
```

Most device metrics are operational, but location, network identifiers, user behavior and content can become personal or sensitive.

Collection requires declared purpose and minimum necessary scope.

## 46. Data minimization

Prefer:

- counts instead of raw content;
- coarse location instead of exact location where sufficient;
- hashed or pseudonymous identifiers where operationally valid;
- bounded diagnostic windows;
- explicit opt-in for verbose capture;
- local processing before upload;
- short retention for high-detail logs.

## 47. Retention policy

Define retention by data class and purpose.

Example categories:

- latest device state;
- short-term raw metrics;
- long-term aggregates;
- incident evidence;
- security audit records;
- provisioning and update records;
- maintenance records;
- temporary diagnostic bundles.

Deletion policy should include backups and derived indexes where applicable.

## 48. Time handling

Records should distinguish:

- monotonic device time;
- unsynchronized wall time;
- approximate wall time;
- synchronized wall time;
- backend receipt time.

Never sort solely by device wall time when its quality is unknown.

## 49. Clock-change events

Record significant time corrections with:

- previous quality;
- new quality;
- estimated offset;
- source;
- reason;
- monotonic timestamp.

Applications and alarms should avoid treating backward wall-clock jumps as negative elapsed time.

## 50. Capacity planning on device

Budget:

- telemetry RAM queue;
- persistent spool bytes;
- event journal writes;
- encoding buffers;
- TLS/network buffers;
- CPU time;
- wake and radio energy if battery-powered;
- SD/Flash wear;
- diagnostic bundle size.

Observability must remain below declared resource limits.

## 51. Backend capacity planning

Estimate:

```text
devices × records per interval × encoded bytes × retention
```

Also account for:

- reconnect bursts;
- rollout bursts;
- incident verbose mode;
- duplicates;
- indexes;
- dashboard queries;
- backups;
- export and deletion operations.

Do not assume current lab traffic represents fleet scale.

## 52. Adaptive telemetry

Possible modes:

```text
NORMAL
REDUCED
VERBOSE_DIAGNOSTIC
OFFLINE_SPOOLING
EMERGENCY_MINIMAL
```

Transitions should be bounded and audited. Verbose mode must expire automatically unless explicitly renewed.

## 53. Telemetry backpressure

When backend or network is slow:

- do not block time-critical tasks indefinitely;
- bound retries;
- apply priority-aware dropping;
- expose queue pressure;
- preserve critical evidence where possible;
- recover gradually to avoid reconnect storms.

## 54. Reconnect storm control

Use randomized exponential backoff and fleet-aware rollout controls.

After common power restoration or backend recovery, devices should not all reconnect at the same instant.

## 55. Site and network outage correlation

Backend analysis should distinguish:

- single-device failure;
- site-wide network outage;
- backend outage;
- credential service failure;
- release-specific regression;
- hardware-lot issue.

Correlation is an analytical aid, not automatic proof of root cause.

## 56. Hardware-profile comparison

Use the hardware profile and procurement lot fields to compare:

- boot stability;
- TFT and touch initialization;
- SD behavior;
- Wi-Fi quality;
- regulator-related brownouts where observable;
- audio-related resets;
- update compatibility.

Do not merge unknown revisions into the reference sample cohort.

## 57. Environmental inference caution

The base board does not provide calibrated ambient temperature, supply-current or rail-voltage telemetry. Do not label proxy values as direct measurements.

If external sensors are added, record:

- sensor type;
- calibration;
- location;
- units;
- accuracy assumptions;
- hardware profile.

## 58. Operational runbook structure

Every actionable alert should link to a runbook containing:

1. purpose and scope;
2. prerequisites and permissions;
3. immediate safety/security checks;
4. evidence to preserve;
5. verification steps;
6. bounded remote actions;
7. local-service escalation;
8. recovery criteria;
9. rollback or replacement criteria;
10. closure records.

## 59. Runbook — device unexpectedly offline

Suggested flow:

1. verify backend ingestion and site status;
2. confirm expected-offline or maintenance windows;
3. inspect last heartbeat and event timeline;
4. compare neighboring devices;
5. inspect last command/update/configuration change;
6. avoid repeated uncontrolled reboot commands;
7. request local power/network inspection if needed;
8. preserve device and backend evidence;
9. classify as restored, maintenance, quarantine or replacement.

## 60. Runbook — repeated resets

1. identify reset reasons and boot stages;
2. compare firmware and hardware cohorts;
3. inspect power, update and watchdog evidence;
4. reduce optional load if approved;
5. prevent endless update/reboot loops;
6. collect diagnostic bundle;
7. move to safe mode or quarantine;
8. perform local power and thermal tests;
9. record root cause or unresolved status.

## 61. Runbook — telemetry spool full

1. verify network/backend availability;
2. inspect queue and rejection counters;
3. preserve critical records;
4. lower verbose telemetry;
5. confirm retention/drop policy activated correctly;
6. avoid filling application storage;
7. restore transport;
8. verify backlog drains without reconnect storm;
9. review capacity assumptions.

## 62. Runbook — failed rollout

1. pause further expansion;
2. identify affected cohorts;
3. compare reset, rollback and health metrics;
4. confirm image/signature/profile compatibility;
5. decide rollback, hold or local recovery;
6. retain failed-device evidence;
7. revoke release if required;
8. document decision and corrective action;
9. requalify before restart.

## 63. Runbook — suspected credential compromise

Follow Part 20 incident policy:

1. suspend or revoke affected identity;
2. prevent unauthorized commands and updates;
3. preserve evidence;
4. determine blast radius;
5. rotate credentials where required;
6. re-provision or quarantine devices;
7. validate restored trust before activation.

## 64. Maintenance integration

Fleet evidence should create maintenance candidates based on declared rules, for example:

- increasing reset frequency;
- recurring SD errors;
- repeated touch recalibration;
- persistent Wi-Fi degradation relative to cohort;
- repeated recovery use;
- configuration corruption;
- excessive local log/spool wear indicators.

Predictive-maintenance claims require historical validation and are not established by this chapter.

## 65. Ticket and incident linkage

Operational records should link:

```text
alert_id
incident_id
maintenance_ticket_id
device_id
release_id
configuration_revision
diagnostic_bundle_id
```

This prevents loss of context across systems.

## 66. Manual annotations

Operators should be able to add bounded annotations such as:

- device physically disconnected;
- site power maintenance;
- enclosure opened;
- antenna obstructed;
- SD replaced;
- board swapped;
- known backend outage.

Annotations require author and timestamp and must not overwrite raw telemetry.

## 67. Data export

Support controlled export for:

- incident review;
- reliability analysis;
- supplier issue;
- regression testing;
- user or regulatory request where applicable.

Exports should preserve schema, units, quality and provenance while applying redaction.

## 68. Schema evolution

Every telemetry, event and command-result schema needs:

- stable name;
- version;
- compatibility policy;
- migration or coexistence plan;
- test vectors;
- backend decoder support window;
- deprecation record.

A backend must not assume all devices update simultaneously.

## 69. Unknown-field behavior

Prefer forward-compatible decoders that:

- ignore unknown optional fields safely;
- reject invalid required fields;
- retain raw envelope for diagnostics where policy allows;
- report schema mismatch counts.

## 70. Feature and capability reporting

A device should report declared capabilities rather than forcing the backend to infer them from product name.

Examples:

- touch present and calibrated;
- SD supported and mounted;
- auxiliary Flash mapped or unmapped;
- Bluetooth enabled;
- remote diagnostics supported;
- OTA supported;
- secure boot enabled and verified;
- Flash encryption enabled and verified.

Capability claims require local implementation evidence.

## 71. Security-state reporting

Report only nonsecret status needed for operations:

- provisioning state;
- credential validity class;
- certificate expiration window;
- secure-boot state if verified;
- Flash-encryption state if verified;
- update-verification status;
- quarantine status;
- last security-policy failure code.

Do not export private material.

## 72. Backend authorization

Fleet dashboards and tools should separate permissions for:

- read inventory;
- read operational metrics;
- read sensitive diagnostics;
- issue safe commands;
- issue destructive commands;
- change desired configuration;
- approve rollout;
- manage credentials;
- export or delete data.

All privileged actions require audit records.

## 73. Operator audit trail

Record:

- principal;
- action;
- target scope;
- request and approval IDs;
- timestamp;
- result;
- reason;
- affected devices;
- rollback or follow-up.

Shared anonymous operator accounts weaken attribution.

## 74. Observability testing

Tests should include:

### 74.1 Schema tests

- encode/decode golden vectors;
- required-field validation;
- unknown-field handling;
- version compatibility;
- unit validation.

### 74.2 Queue tests

- normal flow;
- full queue;
- priority drop;
- reboot recovery;
- corrupt spool;
- duplicate retry;
- sequence gaps.

### 74.3 Transport tests

- DNS failure;
- TLS/authentication failure;
- backend rejection;
- timeout;
- reconnect backoff;
- partial upload;
- certificate expiry.

### 74.4 Dashboard and alert tests

- stale data visibly marked;
- alert fires at boundary;
- hysteresis works;
- maintenance suppression expires;
- runbook link valid;
- recovery closes alert.

### 74.5 Rollout tests

- canary selection;
- automatic pause;
- rollback event;
- offline device rejoins later;
- mixed hardware profiles;
- backend outage during rollout.

## 75. Hardware-in-the-loop observability tests

Recommended HIL cases:

- repeated power cycles;
- UART capture of boot IDs and reset reasons;
- Wi-Fi access-point loss;
- backend disconnect;
- SD insertion/removal;
- touch failure simulation;
- telemetry queue saturation;
- update and rollback cycle;
- safe-mode entry;
- maintenance-mode command audit.

## 76. Fault injection

Safe injections may include:

- forced encoder failure;
- artificial queue pressure;
- delayed transport acknowledgment;
- simulated stale clock;
- rejected credential;
- corrupt local spool copy;
- subsystem health transition;
- backend 4xx/5xx response.

Do not inject electrical faults without separate hardware safety planning.

## 77. Performance acceptance

Measure:

- telemetry CPU time;
- maximum heap use;
- queue memory;
- Flash writes;
- network bytes;
- heartbeat jitter;
- impact on UI responsiveness;
- impact on acquisition timing;
- reconnect behavior;
- backlog drain rate.

No fixed acceptable values are claimed until product requirements and measurements exist.

## 78. Operational acceptance levels

Suggested levels:

```text
O0 local serial evidence only
O1 structured local logs
O2 local health summary and bounded journal
O3 authenticated remote heartbeat and metrics
O4 fleet inventory, dashboards and alerting
O5 remote bounded diagnostics and rollout observation
O6 tested privacy, retention, runbooks and incident linkage
O7 field-validated fleet operations
```

The current repository is near O0. Documentation alone does not advance the implementation level.

## 79. Release gate for observability

Before declaring an observability-capable release:

- schemas are versioned;
- identity fields are correct;
- no secrets appear in logs;
- queue and loss counters are tested;
- telemetry failure does not block core application;
- freshness rules exist;
- dashboards show unknown/stale state;
- alerts have owners and runbooks;
- retention and privacy classes are defined;
- rollout metrics are available;
- backward compatibility is tested;
- HIL evidence is retained.

## 80. Recommended firmware modules

```text
src/observability/
  metrics_registry.*
  event_recorder.*
  structured_logger.*
  health_aggregator.*
  heartbeat_builder.*
  telemetry_encoder.*
  telemetry_queue.*
  persistent_spool.*
  telemetry_transport.*
  loss_accounting.*
  diagnostic_bundle.*
  operation_tracker.*
```

Interfaces should remain independent of one backend protocol.

## 81. Recommended backend domains

```text
fleet registry
ingestion gateway
schema validation
state projection
metrics/time-series storage
event and audit storage
artifact storage
dashboards
alert engine
command authorization
rollout controller
retention/deletion service
```

A simple deployment may combine these components, but their responsibilities should remain clear.

## 82. Example compact health payload

```json
{
  "schema": "cyd.health.v1",
  "device_id": "lab-001",
  "boot_id": "b-123",
  "uptime_s": 3600,
  "state": "DEGRADED",
  "reason": "SD_UNAVAILABLE",
  "firmware": "1.2.0",
  "hardware_profile": "ESP32-2432S028-SAMPLE-A",
  "queue_depth": 4,
  "dropped_low_priority": 12,
  "time_quality": "SYNCED"
}
```

This is an illustrative schema, not an implemented API.

## 83. Example update operation record

```json
{
  "operation_id": "upd-456",
  "type": "FIRMWARE_UPDATE",
  "release": "1.3.0",
  "state": "ROLLED_BACK",
  "steps": [
    "METADATA_VERIFIED",
    "IMAGE_DOWNLOADED",
    "IMAGE_VERIFIED",
    "TRIAL_BOOT",
    "HEALTH_CHECK_FAILED",
    "ROLLBACK_CONFIRMED"
  ]
}
```

Step names must match the implemented update architecture.

## 84. Unsupported claims

This chapter does not establish that:

- the current firmware sends remote telemetry;
- telemetry delivery is lossless;
- dashboards or alerts exist;
- remote diagnostics are authenticated;
- every reset can be explained remotely;
- RSSI alone predicts connectivity quality;
- local logs are tamper-proof;
- microSD provides reliable audit storage;
- all board revisions expose the same useful metrics;
- fleet analytics can predict failures;
- secure boot or Flash encryption status is already available;
- privacy or regulatory compliance has been achieved;
- operational SLOs have been field validated.

## 85. Completion criteria

This architecture may be considered implemented only when:

1. stable identity and boot/session IDs exist;
2. structured metrics, events and logs are generated;
3. subsystem and top-level health are derived from versioned policy;
4. bounded queues and persistent spool behavior are tested;
5. loss is counted and visible;
6. authenticated transport is deployed;
7. fleet inventory separates desired and reported state;
8. dashboards expose freshness and unknown data;
9. actionable alerts have tested runbooks;
10. rollout observation and pause criteria are operational;
11. privacy and retention policies are enforced;
12. remote diagnostics are bounded and audited;
13. schema compatibility is tested across releases;
14. HIL and failure tests demonstrate resource containment;
15. field evidence supports the declared operational level.

## 86. Initial implementation plan

Recommended sequence:

1. introduce stable event and metric codes;
2. add boot ID, sequence and structured serial output;
3. implement subsystem health records;
4. add RAM event ring and loss counters;
5. add machine-readable local export;
6. implement bounded telemetry queue;
7. add authenticated remote heartbeat;
8. create minimal fleet registry and device view;
9. add stale/offline classification;
10. add one tested alert and runbook;
11. add update-operation reporting;
12. add persistent spool only after wear and corruption tests;
13. add remote QUICK diagnostics with authorization;
14. validate cohort and rollout dashboards;
15. run field pilot and revise thresholds.

## 87. Final principle

Observability is not the volume of data collected. It is the controlled ability to determine what happened, what is happening, how certain that conclusion is, what evidence is missing, and which bounded action should follow.

For the ESP32-2432S028 platform, trustworthy fleet operation therefore requires identity, freshness, quality, loss accounting, revision awareness, privacy, security, runbooks and measured resource limits—not merely serial prints, online status or attractive dashboards.
