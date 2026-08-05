# Deployment, Site Integration and Operational Readiness — ESP32-2432S028

> **Document status:** sample-specific deployment and operational-readiness framework for systems built around the documented `ESP32-2432S028` board.  
> **Scope:** deployment profiles, site prerequisites, installation, commissioning, site acceptance, rollback readiness, operator handover, maintenance windows, disaster recovery, evidence retention and deployment closeout.  
> **Evidence rule:** installation is not considered complete merely because the board powers on, joins Wi-Fi, draws a screen or reports a heartbeat. Operational readiness requires verified prerequisites, controlled configuration, site-specific acceptance evidence, recovery capability, assigned ownership and an explicit release decision.

## 1. Purpose

A board that works on a development bench can fail after deployment because of power quality, cable routing, enclosure geometry, antenna obstruction, network policy, environmental exposure, operator error, inaccessible service interfaces or missing recovery assets.

This chapter defines a controlled transition from an accepted board and firmware build to an operational installation.

The deployment process must answer:

- what is being installed;
- where and under which environmental conditions;
- what power and network services are available;
- which hardware and firmware profiles are approved;
- who owns installation, acceptance and operation;
- how failure is detected and contained;
- how the previous known-good state is restored;
- how the site is handed over and later maintained;
- what evidence closes the deployment.

## 2. Relationship to earlier chapters

Deployment depends on the preceding reference:

- Part 5 defines power-path limits and measurement needs;
- Parts 11–13 define USB/UART, expansion and wireless constraints;
- Part 14 defines enclosure, mounting, antenna and cable integration;
- Part 15 defines GPIO ownership and resource conflicts;
- Part 16 defines board bring-up and acceptance;
- Part 17 defines maintenance and lifecycle controls;
- Part 18 defines diagnostics and recovery;
- Part 19 defines procurement, lot and revision control;
- Part 20 defines identity, provisioning and update security;
- Part 21 defines software ownership and subsystem architecture;
- Part 22 defines application workflows;
- Part 23 defines observability and fleet operations.

Site acceptance must not silently override a failed laboratory qualification, unresolved hardware-profile mismatch or missing security control.

## 3. Current baseline and limitations

The repository currently demonstrates a local self-test and documents an intended production architecture. It does not by itself establish:

- suitability for a particular site;
- environmental protection;
- mains or industrial power compliance;
- network admission;
- remote observability;
- production provisioning;
- secure OTA;
- disaster-recovery capability;
- trained operators;
- validated service-level objectives;
- field reliability.

Each deployment must therefore create its own site record and acceptance evidence.

## 4. Deployment object model

Treat deployment records as linked but distinct objects:

```text
product definition
  -> approved hardware profile
  -> approved firmware release
  -> provisioned device identity
  -> deployment package
  -> site
  -> installation position
  -> commissioning session
  -> site acceptance record
  -> operational ownership
  -> maintenance and incident history
```

Do not use one identifier for all objects.

## 5. Deployment states

Recommended states:

```text
PLANNED
SITE_SURVEY_PENDING
SITE_NOT_READY
PACKAGE_PREPARED
SHIPPED
RECEIVED_AT_SITE
INSTALLATION_IN_PROGRESS
INSTALLED_NOT_COMMISSIONED
COMMISSIONING
ACCEPTANCE_PENDING
ACCEPTED_CONDITIONAL
ACCEPTED_OPERATIONAL
SUSPENDED
ROLLBACK_IN_PROGRESS
QUARANTINED
DECOMMISSIONING
CLOSED
```

Every transition should record actor, timestamp, reason and evidence reference.

## 6. Deployment classes

### 6.1 LAB deployment

Characteristics:

- attended use;
- open access to USB and buttons;
- temporary wiring may be permitted;
- test credentials only;
- rapid manual recovery expected;
- no unattended service claim.

### 6.2 PILOT deployment

Characteristics:

- limited device count;
- known users;
- enhanced logging;
- frequent observation;
- explicit rollback window;
- reduced claim ceiling.

### 6.3 CONTROLLED INDOOR deployment

Characteristics:

- stable indoor environment;
- controlled power and network;
- approved enclosure;
- documented service access;
- trained local operator or remote support.

### 6.4 UNATTENDED deployment

Requires additional evidence for:

- automatic recovery;
- power-loss behavior;
- watchdog behavior;
- remote health reporting;
- safe degraded states;
- maintenance access;
- physical security.

### 6.5 HARSH OR OUTDOOR deployment

The bare board is not assumed suitable. Separate engineering is required for:

- ingress protection;
- condensation;
- temperature extremes;
- UV exposure;
- surge and lightning environment;
- corrosion;
- vibration;
- insects and contamination.

## 7. Site survey

A site survey should be completed before equipment is committed.

Record:

- site identifier and address or controlled location code;
- installation-room and mounting-position identifiers;
- responsible site contact;
- access restrictions;
- working hours;
- available power sources;
- grounding and bonding arrangements where relevant;
- network availability and ownership;
- radio conditions;
- environmental observations;
- enclosure and mounting constraints;
- cable routes and lengths;
- nearby heat, moisture, dust, vibration and interference sources;
- maintenance access;
- emergency shutdown expectations;
- photography permissions;
- security requirements.

## 8. Site-readiness gate

A site is not ready until mandatory prerequisites are either satisfied or formally waived.

Suggested gate:

| Area | Required evidence |
|---|---|
| physical location | approved mounting position and access path |
| power | source type, connector, capacity and measured behavior |
| network | approved SSID/VLAN/service path or declared offline mode |
| environment | conditions within validated limits |
| enclosure | approved assembly and antenna clearance |
| identity | asset and installation labels assigned |
| security | credential and local-access plan approved |
| support | named owner and escalation route |
| recovery | rollback image, cable and procedure available |
| records | deployment package version fixed |

A failed mandatory item leaves the state `SITE_NOT_READY`.

## 9. Power prerequisites

Document the actual power architecture, not just nominal voltage.

Record:

- source type;
- nominal and measured voltage;
- connector and polarity;
- cable length and conductor size;
- source current capability;
- shared loads;
- expected interruption behavior;
- surge or transient environment;
- backup-power behavior if present;
- protective devices;
- shutdown responsibility.

## 10. USB power caution

A USB connector does not prove that every supply, cable or charger is suitable.

Before deployment verify:

- stable 5-V delivery at required load;
- cable voltage drop;
- connector retention;
- absence of data-line or charger-mode assumptions that affect operation;
- behavior during Wi-Fi, display, SD and audio peaks;
- restart behavior after brief interruption;
- no unintended back-power path through UART or external peripherals.

## 11. Power acceptance tests

Recommended tests:

1. cold start from fully unpowered state;
2. repeated power cycles;
3. low-voltage margin test within a controlled setup;
4. maximum expected load combination;
5. network reconnect during load;
6. removable-storage activity during load;
7. controlled interruption and restoration;
8. observation for reset, brownout and data corruption;
9. thermal observation at regulators and enclosure surfaces.

Do not perform unsafe fault injection on an installed live system without an approved method.

## 12. Network prerequisites

Before installation obtain:

- approved network type;
- SSID or wired-gateway arrangement if applicable;
- authentication method;
- VLAN or segmentation requirement;
- DHCP or static-address policy;
- DNS requirements;
- time-service requirements;
- allowed destinations and ports;
- proxy or captive-portal constraints;
- certificate-validation requirements;
- outbound and inbound policy;
- monitoring expectations;
- change-control contact.

## 13. Network admission test

Verify:

- successful association;
- expected IP configuration;
- DNS resolution;
- time synchronization where required;
- backend reachability;
- certificate validation;
- telemetry upload;
- command-channel policy;
- reconnect after access-point restart;
- behavior when backend is unavailable;
- bounded retry and no reconnect storm.

## 14. Offline deployment profile

An offline installation requires an explicit profile.

Define:

- what functions remain available;
- how time is handled;
- where data is stored;
- storage-capacity limits;
- how logs and updates are collected;
- how credentials are managed;
- how alarms are presented locally;
- how maintenance is scheduled;
- what data loss is acceptable.

Offline must not mean unmanaged.

## 15. Radio-site assessment

For Wi-Fi or Bluetooth deployments, record:

- signal level at the final mounting position;
- orientation of the ESP-WROOM-32 antenna;
- nearby metal or conductive surfaces;
- enclosure material;
- cable bundles near the antenna;
- competing access points;
- channel utilization if measured;
- connection stability over time;
- performance with enclosure closed;
- performance during display, SD and audio activity.

Bench measurements with an open enclosure do not qualify the installed radio path.

## 16. Environmental prerequisites

The documented board does not include a declared industrial environmental rating.

Site records should address:

- ambient temperature;
- internal enclosure temperature;
- humidity;
- condensation risk;
- dust;
- conductive contamination;
- liquids;
- direct sunlight;
- vibration and shock;
- electromagnetic interference;
- corrosive atmosphere;
- insects and biological contamination;
- cleaning chemicals.

Unknown conditions must be marked `UNKNOWN`, not assumed benign.

## 17. Thermal integration

Verify the assembled installation under realistic load.

Record:

- ambient temperature;
- enclosure state;
- display brightness;
- radio activity;
- processor load;
- SD and audio activity;
- measured temperatures or qualitative thermal evidence;
- duration;
- hottest observed location;
- reset or throttling evidence;
- margin rationale.

Onboard temperature estimates are not substitutes for calibrated external measurements unless their relationship is established.

## 18. Condensation control

Condensation risk exists when equipment transitions between temperature zones or operates in humid environments.

Controls may include:

- acclimatization before power-up;
- sealed or ventilated enclosure strategy;
- drainage and orientation;
- desiccant with replacement schedule;
- conformal coating only after compatibility review;
- humidity monitoring;
- prohibition on powering visibly wet equipment.

## 19. Mechanical installation package

The package should define:

- enclosure part and revision;
- board mounting method;
- screw and spacer specification;
- torque or retention method where needed;
- display and touch opening;
- button access;
- USB and service access;
- microSD access policy;
- speaker mounting;
- cable strain relief;
- antenna keep-out;
- labels;
- tamper evidence if required;
- assembly photographs.

## 20. Mounting verification

Check that:

- no fastener contacts circuitry;
- the PCB is not bent;
- spacers have correct height;
- the display is not mechanically stressed;
- touch operation does not flex the assembly excessively;
- the antenna region remains clear;
- cables cannot pull on small connectors;
- ventilation and service access match the design;
- conductive debris is absent.

## 21. Cable-management rules

Each cable should have:

- identified purpose;
- controlled pinout;
- connector orientation;
- length limit;
- strain relief;
- route;
- separation requirement where applicable;
- label at one or both ends;
- inspection method;
- replacement part record.

Cable color alone is not an identity control.

## 22. Expansion-interface deployment control

External devices connected to P1, P3, CN1 or P4 require a site-specific interface record.

The record should state:

- connector reference;
- pin order and orientation;
- voltage domain;
- current demand;
- signal direction;
- startup behavior;
- pull-ups or pull-downs;
- shared onboard function;
- cable length;
- protection;
- failure effect;
- disconnection and replacement procedure.

## 23. Known board-specific restrictions

For the documented profile:

- GPIO21 controls TFT backlight and is also exposed on P3;
- GPIO35 is input-only;
- GPIO27 is a candidate expansion signal but its exact use must remain controlled;
- UART0/P1 is shared with programming and logs;
- GPIO0 is a boot-strapping input;
- GPIO34, 36 and 39 are input-only and already assigned;
- GPIO16 and 17 have RGB LED loads;
- GPIO26 is assigned to audio;
- speaker output is BTL and neither side is ground;
- U4 W25Q32 routing remains unverified.

Site wiring must not treat a physically exposed pin as automatically free.

## 24. Deployment package

A complete package should contain:

- deployment manifest;
- site identifier;
- device asset IDs;
- hardware-profile ID;
- board lot and revision evidence;
- firmware release ID and hash;
- configuration schema and instance ID;
- provisioning receipt;
- credentials-status record without secret disclosure;
- enclosure and cable BOM;
- installation drawings or photographs;
- commissioning checklist;
- acceptance test plan;
- rollback package;
- operator guide;
- maintenance guide;
- escalation contacts;
- closeout template.

## 25. Package integrity

Before shipment or installation verify:

- all artifacts are versioned;
- firmware hashes match approved release records;
- configuration belongs to the intended site and device;
- secrets are delivered through an approved channel;
- labels match electronic identity records;
- the rollback image is readable;
- required cables and adapters are present;
- test equipment is available;
- obsolete files are not mixed into the package.

## 26. Pre-deployment staging

Staging should reproduce the intended deployment as closely as practical.

Test:

- actual enclosure;
- final power supply and representative cable;
- intended peripherals;
- site-like network policy;
- production-like configuration;
- reboot and recovery;
- telemetry and alarms;
- update and rollback;
- operator workflows;
- data retention and spool behavior.

## 27. Staging exit criteria

The package may leave staging only when:

- device identity is correct;
- hardware and firmware profiles agree;
- acceptance tests pass;
- unresolved deviations are approved;
- recovery assets are verified;
- no development credentials remain;
- logs are cleared or archived according to policy;
- shipment or installation record is created.

## 28. Shipping and receipt

Record:

- shipment identifier;
- package contents;
- packaging method;
- ESD protection;
- shock or moisture indicators if used;
- custody handoffs;
- receipt date;
- package damage;
- missing items;
- quarantine decision.

A device damaged in transit must not proceed directly to commissioning.

## 29. Installation work control

Before work starts confirm:

- approved method statement;
- responsible installer;
- access authorization;
- outage or maintenance window;
- safety requirements;
- tools and test equipment;
- rollback stop point;
- communication route;
- site contact;
- change ticket if required.

## 30. Installation sequence

Recommended sequence:

1. verify site and package identity;
2. inspect equipment for damage;
3. confirm power is isolated where required;
4. mount enclosure and hardware;
5. route and label cables;
6. verify connector orientation and polarity;
7. perform continuity or resistance checks where appropriate;
8. connect controlled power;
9. observe first boot locally;
10. verify identity and hardware profile;
11. connect network or peripherals in stages;
12. proceed to commissioning tests;
13. photograph final installation if permitted.

## 31. First power-up at site

Observe and record:

- supply voltage before connection;
- inrush or startup behavior if measured;
- reset reason;
- boot-stage progress;
- display and RGB indications;
- UART output where allowed;
- unexpected heating, odor or noise;
- network association;
- local input behavior;
- initial health state.

Stop immediately for smoke, excessive heating, unstable supply or repeated brownout/reset behavior.

## 32. Commissioning phases

Suggested phases:

```text
C0 physical inspection
C1 power and boot
C2 identity and configuration
C3 local hardware functions
C4 network and time
C5 backend enrollment
C6 data flow and storage
C7 commands and alarms
C8 update and rollback readiness
C9 endurance observation
C10 operator handover
```

Do not skip directly to application testing if foundational phases fail.

## 33. Identity verification at site

Compare:

- physical label;
- owner asset ID;
- silicon-derived identifier if used;
- logical device ID;
- certificate or key identifier;
- hardware-profile ID;
- firmware release ID;
- backend inventory record;
- intended site and installation position.

Any mismatch is a commissioning stop condition until resolved.

## 34. Configuration verification

Record:

- configuration schema version;
- configuration instance or revision;
- site-specific parameters;
- units and locale;
- network settings;
- data endpoints;
- alarm thresholds;
- reporting intervals;
- maintenance mode settings;
- privacy settings;
- update channel;
- expected desired state.

Secrets should be verified by status or controlled test, not printed into reports.

## 35. Local hardware commissioning

At minimum test applicable functions:

- TFT initialization and visible patterns;
- backlight control;
- touch raw response and calibrated mapping;
- RGB indicators;
- LDR behavior;
- BOOT and RESET handling;
- microSD mount/read/write/removal policy;
- audio output with approved load;
- expansion inputs and outputs;
- USB/UART service path;
- power stability under combined load.

## 36. Touch commissioning

Verify with enclosure closed:

- all corners;
- center;
- representative targets;
- edge targets;
- intended screen rotation;
- pressure variation;
- repeated taps;
- no activation from enclosure pressure;
- recovery path if touch fails.

Calibration belongs to the physical display/touch assembly and orientation, not merely to the firmware version.

## 37. Display commissioning

Verify:

- correct rotation;
- color order;
- no visible corruption;
- stable backlight;
- readable text at operating brightness;
- no enclosure obstruction;
- no unacceptable glare for the intended location;
- safe fallback indication when application data is unavailable.

## 38. Storage commissioning

For microSD or other writable storage:

- identify media;
- verify capacity and filesystem;
- perform controlled write/read;
- test restart after write;
- test missing-media behavior;
- verify full-storage handling;
- verify no secrets are exposed contrary to policy;
- record replacement and extraction procedure.

## 39. Network and backend commissioning

Verify:

- network association;
- backend authentication;
- inventory registration;
- heartbeat;
- telemetry sequence;
- desired/reported-state exchange;
- command authorization;
- loss of connection and reconnect;
- backend outage behavior;
- certificate-expiry visibility;
- device revocation path where applicable.

## 40. Time commissioning

Record:

- time source;
- initial synchronization delay;
- reported quality state;
- behavior before synchronization;
- monotonic-timer operation;
- behavior after network loss;
- timezone and daylight-saving presentation rules;
- log ordering across restart.

## 41. Alarm commissioning

Use approved safe stimuli to verify:

- alarm entry;
- severity;
- local indication;
- remote delivery;
- acknowledgment;
- silence policy;
- clear condition;
- missing-data behavior;
- deduplication;
- maintenance suppression;
- audit trail.

Do not claim alarm coverage for hazards not represented in the design.

## 42. Command commissioning

For each command class verify:

- authorized origin;
- schema validation;
- current-state preconditions;
- idempotency or duplicate handling;
- timeout behavior;
- result record;
- rejection of unauthorized command;
- rejection of stale or replayed command;
- safe behavior on connection loss;
- operator-visible outcome.

## 43. Update-readiness test

Before operational acceptance establish:

- current release identity;
- approved target release source;
- signature or authenticity check as implemented;
- sufficient storage;
- power prerequisites;
- download interruption behavior;
- trial-boot policy;
- healthy checkpoint;
- rollback method;
- anti-rollback constraints;
- configuration migration and recovery;
- operator communication plan.

## 44. Rollback readiness

Rollback readiness requires more than retaining an old binary.

Verify:

- known-good image and hash;
- compatible partition table and bootloader;
- compatible configuration or migration path;
- required credentials and certificates;
- host tools and drivers;
- correct cable and connector access;
- local authorization;
- expected recovery time;
- post-rollback acceptance checklist;
- backend compatibility.

## 45. Rollback decision triggers

Predefine triggers such as:

- repeated boot failure;
- health regression;
- data corruption;
- command failure above threshold;
- loss of connectivity across a cohort;
- unexpected power or thermal behavior;
- security finding;
- operator-critical workflow failure;
- incompatible configuration migration.

Triggers should pause broader rollout while preserving evidence.

## 46. Site acceptance test

A site acceptance test should include:

- installation inspection;
- identity verification;
- power test;
- local subsystem test;
- network and backend test;
- application workflow test;
- alarm and command test;
- offline or outage test;
- restart test;
- update/rollback readiness;
- documentation check;
- ownership and support check.

## 47. Acceptance outcomes

Allowed outcomes:

```text
PASS
PASS_WITH_CONDITIONS
FAIL_CORRECTABLE
FAIL_QUARANTINE
DEFERRED
```

`PASS_WITH_CONDITIONS` must include:

- explicit deviation;
- risk owner;
- compensating control;
- expiry date;
- closure action.

## 48. Operational acceptance levels

Suggested levels:

| Level | Meaning |
|---|---|
| D0 | documentation concept only |
| D1 | bench deployment procedure drafted |
| D2 | staged package demonstrated |
| D3 | one attended pilot commissioned |
| D4 | site acceptance repeatable |
| D5 | recovery and maintenance exercised |
| D6 | multi-device or multi-site operations demonstrated |
| D7 | field evidence supports declared operating profile |

A project must state its achieved level.

## 49. Soak and observation period

After commissioning, observe for a declared interval.

Monitor:

- resets;
- brownouts;
- heap and storage trends;
- queue depth;
- telemetry gaps;
- network reconnects;
- time quality;
- touch and UI complaints;
- temperature observations;
- alarm behavior;
- operator interventions;
- configuration drift.

The interval should match the risk and operating cycle.

## 50. Readiness review

Before release to operations, review:

- all acceptance results;
- open deviations;
- current health;
- observability coverage;
- recovery evidence;
- spare and tool availability;
- operator training;
- maintenance plan;
- support contacts;
- security and credential state;
- document completeness.

## 51. Operational ownership

Assign named roles:

- service owner;
- site owner;
- device or fleet operator;
- firmware owner;
- backend owner;
- network owner;
- security contact;
- maintenance technician;
- incident commander or escalation role;
- approval authority for changes and rollback.

Unowned systems are not operationally ready.

## 52. Operator handover package

Provide:

- purpose and limitations;
- normal-state description;
- local indicator meanings;
- startup and shutdown procedure;
- routine checks;
- alarm response;
- safe reset procedure;
- actions that are prohibited;
- support contact;
- escalation thresholds;
- maintenance-window process;
- privacy and credential handling;
- replacement-unit process.

## 53. Training and competency

Record:

- training material version;
- attendee;
- date;
- instructor;
- practical exercises;
- recovery exercise if required;
- competency result;
- refresher requirement.

Training attendance alone does not prove competency for destructive recovery or security operations.

## 54. Maintenance windows

Define:

- allowed days and times;
- notification lead time;
- approval route;
- pre-work backup;
- current-state capture;
- service-mode entry;
- rollback stop point;
- post-work validation;
- closeout communication;
- emergency exception process.

## 55. Planned maintenance

Possible activities:

- inspection and cleaning;
- connector check;
- storage-health check;
- log extraction;
- certificate rotation;
- firmware update;
- configuration review;
- touch recalibration;
- power-supply replacement;
- enclosure seal inspection;
- backup and recovery verification.

Intervals should be based on evidence and environment, not invented universal periods.

## 56. Emergency maintenance

Emergency work still requires minimum controls:

- incident identifier;
- responsible authority;
- current evidence capture if possible;
- explicit action scope;
- credentials protection;
- replacement or rollback path;
- post-action verification;
- retrospective review.

## 57. Spare-device readiness

A spare board is not ready merely because it was purchased.

Verify:

- hardware profile;
- storage history;
- physical condition;
- firmware compatibility;
- provisioning state;
- credentials policy;
- enclosure and cable compatibility;
- acceptance-test result;
- ability to bind to the intended asset or replacement record.

## 58. Replacement procedure

A controlled replacement should:

1. identify failed asset;
2. preserve diagnostic evidence;
3. revoke or suspend credentials if needed;
4. verify replacement hardware profile;
5. provision a distinct identity or approved identity-transfer process;
6. load approved firmware and configuration;
7. commission at site;
8. update inventory and backend records;
9. quarantine the removed unit;
10. close records only after acceptance.

## 59. Configuration drift control

Compare desired and reported state for:

- firmware release;
- hardware profile;
- configuration version;
- update channel;
- alarm rules;
- telemetry policy;
- network settings;
- security state;
- maintenance mode;
- calibration version.

Unexplained drift requires investigation.

## 60. Site change management

Reassessment is required after changes such as:

- new enclosure;
- new power supply;
- longer cable;
- changed mounting position;
- access-point replacement;
- VLAN or firewall change;
- added nearby metalwork;
- new peripheral;
- building work;
- environmental change;
- firmware or configuration change;
- board replacement.

## 61. Disaster scenarios

Consider at least:

- site-wide power loss;
- repeated unstable power;
- network outage;
- backend outage;
- credential compromise;
- failed fleet rollout;
- corrupted local storage;
- damaged enclosure;
- water exposure;
- loss or theft of device;
- loss of provisioning or signing infrastructure;
- loss of inventory records.

## 62. Disaster-recovery objectives

Define where applicable:

- maximum tolerable outage;
- recovery-time objective;
- recovery-point objective;
- minimum local function during outage;
- required data retention;
- manual fallback;
- replacement-device availability;
- responsible decision maker.

Do not claim formal RTO/RPO achievement until tested.

## 63. Recovery hierarchy

Recommended order:

```text
restore external dependency
restart affected service
restart application subsystem
controlled device reboot
restore configuration
rollback firmware
reprovision credentials
replace device
rebuild site integration
```

Use the least destructive effective action.

## 64. Recovery exercises

Exercise representative scenarios:

- power restoration;
- network restoration;
- backend outage;
- failed update rollback;
- corrupted configuration recovery;
- full telemetry spool;
- device replacement;
- credential revocation;
- loss of touch with serial fallback.

Record actual duration, obstacles and evidence gaps.

## 65. Incident-site evidence

Preserve where possible:

- device identity;
- installation position;
- firmware and configuration versions;
- reset reason;
- logs and telemetry;
- photographs;
- power measurements;
- network status;
- recent changes;
- operator statements;
- environmental observations;
- removed hardware and media chain of custody.

## 66. Safety boundaries

This documentation does not establish that the board is suitable for:

- life-safety control;
- machinery safety functions;
- mains switching without approved isolation;
- medical use;
- explosive atmospheres;
- fire-alarm functions;
- automotive safety systems;
- unattended hazardous-energy control.

Such applications require separate certified engineering and assurance.

## 67. Physical security at site

Depending on risk, control:

- enclosure access;
- USB access;
- BOOT and RESET access;
- microSD removal;
- UART/P1 access;
- exposed expansion connectors;
- device theft;
- label tampering;
- unauthorized power cycling;
- service-key custody.

## 68. Privacy at site

Document:

- what data is collected;
- whether location or user behavior can be inferred;
- local display exposure;
- log and microSD access;
- retention;
- support access;
- photographs taken during service;
- disposal of replaced media.

Operational convenience does not override declared privacy policy.

## 69. Documentation control

Site documentation should have:

- owner;
- version;
- approval status;
- effective date;
- superseded-version handling;
- storage location;
- access control;
- backup;
- review schedule;
- relationship to inventory and change records.

## 70. Required site records

Recommended minimum:

```text
SITE-RECORD
INSTALLATION-RECORD
COMMISSIONING-REPORT
SITE-ACCEPTANCE-REPORT
CONFIGURATION-RECORD
NETWORK-APPROVAL
POWER-VERIFICATION
OPERATOR-HANDOVER
MAINTENANCE-PLAN
ROLLBACK-READINESS
DISASTER-RECOVERY-PLAN
DEPLOYMENT-CLOSEOUT
```

## 71. Installation record template

```text
installation_id:
site_id:
position_id:
asset_id:
hardware_profile_id:
firmware_release_id:
configuration_id:
installer:
start_time:
end_time:
power_source:
network_profile:
enclosure_revision:
cable_set_revision:
photos:
observations:
deviations:
result:
```

## 72. Commissioning report template

```text
commissioning_id:
asset_id:
boot_id:
identity_check:
power_tests:
local_hardware_tests:
network_tests:
backend_tests:
data_flow_tests:
alarm_tests:
command_tests:
restart_tests:
rollback_readiness:
open_items:
overall_result:
approver:
```

## 73. Site acceptance template

```text
site_acceptance_id:
site_id:
deployment_scope:
devices:
mandatory_tests_passed:
conditions:
deviations:
risk_owner:
operator_handover_complete:
recovery_assets_verified:
acceptance_outcome:
accepted_by:
acceptance_time:
```

## 74. Closeout criteria

Deployment closes only when:

- all intended assets are accounted for;
- installation records are complete;
- acceptance outcome is recorded;
- conditional items have owners and dates;
- inventory matches physical state;
- backend records match site state;
- operators have accepted handover;
- recovery assets are stored and verified;
- temporary credentials and test configurations are removed;
- packaging and obsolete parts are dispositioned;
- photographs and diagrams are archived as permitted;
- lessons learned are captured.

## 75. Unsupported claims

This chapter does not claim:

- environmental qualification of the bare board;
- compliance with electrical-installation standards;
- suitability for outdoor or industrial use;
- guaranteed Wi-Fi coverage;
- validated enclosure ingress rating;
- universal cable-length limits;
- secure remote operation in the current firmware;
- automated disaster recovery;
- tested RTO or RPO;
- field-proven maintenance intervals;
- operational readiness of every CYD revision;
- safety certification.

## 76. Completion criteria for this chapter

The deployment framework is considered implemented only when the project has:

- at least one versioned deployment profile;
- a completed site-survey template;
- controlled staging and package generation;
- a repeatable commissioning procedure;
- explicit site acceptance criteria;
- verified rollback assets;
- assigned operational ownership;
- operator handover material;
- maintenance and disaster-recovery procedures;
- closeout records;
- evidence from at least one representative deployment.

## 77. Recommended implementation tasks

1. create machine-readable deployment manifests;
2. add firmware screens or serial commands for identity and version verification;
3. implement commissioning result export;
4. define site configuration schemas;
5. add power, network and storage readiness checks;
6. create operator-facing local health summary;
7. implement maintenance-mode indication and audit;
8. automate desired/reported-state comparison;
9. package rollback images and host tools together;
10. create a recovery drill checklist;
11. establish installation photograph and label conventions;
12. pilot the process on one controlled indoor site;
13. revise thresholds from measured field evidence.

## 78. Final operational principle

A deployment is ready only when hardware, firmware, configuration, identity, site conditions, recovery capability, observability and human ownership agree.

Successful first boot is one commissioning observation. It is not operational acceptance.
