# System Security, Provisioning and Trusted Update Architecture — ESP32-2432S028

> **Document status:** sample-specific security architecture and implementation plan for the author's `ESP32-2432S028` board.  
> **Scope:** threat model, device identity, credentials, provisioning, firmware authenticity, update and rollback policy, diagnostic-interface hardening, network exposure, incident response, lifecycle security, and decommissioning.  
> **Evidence rule:** a feature is not considered secure merely because a password, checksum, TLS connection, private repository, or hidden menu exists. Security claims require a declared threat model, implementation evidence, key-handling records, negative tests, recovery tests, and operational procedures.

## 1. Purpose

The board combines a programmable ESP32, USB/UART service access, Wi-Fi and Bluetooth radios, removable storage, auxiliary Flash, a display and touch interface, and expansion connectors. That combination creates several independent trust boundaries.

A secure deployment must answer:

- What is the device allowed to trust?
- How is each physical board identified?
- Where are secrets generated and stored?
- Who may provision, update, diagnose, reset, or retire a board?
- How is an update authenticated before execution?
- What happens when power is lost during an update?
- How is rollback controlled?
- What evidence is retained after a security event?
- How are credentials revoked when a board is lost or disposed of?

This chapter defines a practical security architecture without claiming that every mechanism is already implemented in the current firmware.

## 2. Relationship to earlier chapters

Security depends on the previously documented hardware and operational controls:

- Part 9 defines program Flash, auxiliary W25Q32 and microSD storage;
- Part 11 defines USB, CH340G, UART0 and bootloader access;
- Part 12 defines exposed GPIO and expansion risks;
- Part 13 defines wireless interfaces;
- Part 15 defines pin conflicts and boot-strapping behavior;
- Part 16 defines acceptance and release evidence;
- Part 17 defines lifecycle, backups and decommissioning;
- Part 18 defines diagnostic and recovery modes;
- Part 19 defines supplier, lot, revision and provisioning traceability.

Security controls must not silently conflict with service recovery, production testing, or hardware revision differences.

## 3. Current security baseline

The current repository demonstrates a local PlatformIO firmware that:

- initializes board hardware;
- writes a UI to the TFT;
- reads the LDR and BOOT button;
- cycles the RGB LED;
- writes status to UART0.

The current baseline does not by itself establish:

- secure boot;
- Flash encryption;
- signed update verification;
- per-device credentials;
- protected provisioning;
- authenticated diagnostic commands;
- anti-rollback enforcement;
- secret redaction from all logs;
- remote-service security;
- incident-response telemetry;
- verified credential erasure.

These remain architecture and implementation tasks until demonstrated.

## 4. Security objectives

Recommended objectives:

1. only authorized firmware executes in protected deployments;
2. firmware and configuration changes are attributable;
3. device credentials are unique where practical;
4. compromise of one board does not automatically compromise all boards;
5. secrets are not committed to the public repository;
6. production secrets are not printed to UART, TFT, logs or reports;
7. remote interfaces expose only required functions;
8. updates are authenticated, integrity-checked and recoverable;
9. power loss cannot leave the device permanently unrecoverable under the declared update design;
10. compromised devices can be revoked and quarantined;
11. local service access is controlled according to deployment risk;
12. decommissioning removes active trust relationships.

## 5. Security non-objectives and boundaries

Unless separately engineered and verified, this board should not be assumed to provide:

- tamper resistance against a skilled attacker with physical possession;
- secure-element-backed key isolation;
- resistance to invasive semiconductor attacks;
- guaranteed confidentiality when UART0, boot pins and Flash are physically accessible;
- certified safety or security compliance;
- protection against all supply-chain compromise;
- secure storage in the removable microSD card;
- trustworthy time without an authenticated time source and persistence policy;
- secure deletion equivalent to destruction of physical storage media.

The classic ESP32 may support useful silicon security features, but the actual protection depends on irreversible configuration, build settings, key custody and recovery planning.

## 6. Assets to protect

Security analysis should identify at least these assets:

### 6.1 Firmware assets

- bootloader;
- partition table;
- application images;
- recovery image;
- diagnostic firmware;
- OTA metadata;
- board-profile configuration;
- build and signing infrastructure.

### 6.2 Identity and credential assets

- device ID;
- Wi-Fi credentials;
- API tokens;
- client private keys;
- device certificates;
- update-verification public keys;
- provisioning authorization tokens;
- service credentials;
- recovery secrets;
- manufacturing records linking identity to a physical board.

### 6.3 User and operational data

- configuration;
- calibration;
- logs;
- network identifiers;
- application measurements;
- user-generated files;
- maintenance and incident history.

### 6.4 Physical and service assets

- USB programming access;
- P1 UART access;
- BOOT and RESET controls;
- microSD card;
- expansion connectors;
- auxiliary W25Q32 contents;
- golden images and backups;
- signing keys and provisioning workstation.

## 7. Threat actors

A useful threat model distinguishes:

| Actor | Typical capability |
|---|---|
| accidental operator | mistakes, wrong image, wrong cable, leaked logs |
| curious local user | touch/UI access, power cycling, removable card access |
| local network attacker | scanning, protocol abuse, credential guessing |
| Internet attacker | exposed services, cloud-account compromise, malicious update source |
| malicious technician | serial access, firmware replacement, credential extraction attempts |
| supply-chain attacker | substituted board, modified firmware, counterfeit components |
| device thief | prolonged physical possession |
| compromised developer machine | source, build, package or signing compromise |
| compromised backend | fraudulent commands, credential abuse, malicious update metadata |

Controls should be selected according to the deployment, not from an undefined generic attacker.

## 8. Attack surfaces

### 8.1 Physical

- USB connector;
- CH340G and UART0;
- P1 service UART;
- BOOT and RESET buttons;
- exposed GPIO;
- program Flash through the ESP32 module;
- auxiliary W25Q32;
- microSD;
- display and touch UI;
- enclosure access;
- power fault injection.

### 8.2 Wireless and network

- Wi-Fi association;
- DHCP, DNS and time services;
- application TCP/UDP services;
- HTTP or WebSocket interfaces;
- MQTT or other brokers;
- BLE advertising and GATT;
- Bluetooth Classic profiles;
- OTA download endpoints;
- remote diagnostics.

### 8.3 Software supply chain

- Git repository;
- PlatformIO platform and packages;
- Arduino framework;
- libraries;
- compiler and linker;
- CI runners;
- release artifacts;
- signing service;
- provisioning host utility.

### 8.4 Human and process

- shared passwords;
- copied configuration files;
- screenshots exposing secrets;
- unredacted logs;
- uncontrolled backup copies;
- undocumented board swaps;
- forgotten test credentials;
- emergency recovery procedures.

## 9. Trust zones

Recommended zones:

```text
Z0 silicon ROM and immutable hardware identity
Z1 bootloader and protected verification configuration
Z2 application firmware
Z3 protected configuration and device credentials
Z4 ordinary application data and logs
Z5 removable microSD
Z6 local service interfaces
Z7 local network
Z8 remote services and update infrastructure
Z9 development, CI, signing and provisioning systems
```

Data movement between zones should be explicit.

## 10. Security profiles

Not every project needs the same controls.

### 10.1 LAB profile

Use case: open bench development.

Characteristics:

- UART and bootloader accessible;
- unsigned local builds may be allowed;
- test credentials only;
- no production secrets;
- easy recovery prioritized;
- board clearly marked as non-production.

### 10.2 CONTROLLED-DEVELOPMENT profile

- restricted source and release process;
- per-device or per-lab credentials;
- signed release artifacts;
- authenticated remote services;
- debug access recorded;
- secrets excluded from repository.

### 10.3 DEPLOYED profile

- verified firmware authenticity;
- unique device identity;
- minimized network exposure;
- update and rollback policy;
- diagnostic commands restricted;
- incident telemetry and revocation;
- secure decommissioning.

### 10.4 HIGHER-ASSURANCE profile

May add:

- secure boot;
- Flash encryption;
- hardware-backed key handling where available;
- irreversible debug restrictions;
- controlled signing service;
- dual-control key operations;
- tamper-evident enclosure;
- formal penetration and recovery tests.

A higher-assurance profile must be separately designed and validated.

## 11. Device identity

Identity should not rely only on a user-editable hostname or MAC address.

Recommended identity layers:

```text
asset ID assigned by owner
hardware profile ID
lot and board record
silicon-derived identifier where available
provisioned logical device ID
certificate subject or key identifier
backend enrollment record
```

### 11.1 Identity properties

A production device identity should be:

- unique within the managed fleet;
- stable across ordinary firmware updates;
- linked to the physical asset record;
- revocable;
- non-secret;
- distinguishable from credentials;
- recoverable through controlled records.

### 11.2 MAC-address caution

MAC addresses may assist inventory but should not be treated as secret authentication credentials.

### 11.3 Asset-ID storage

Store the owner asset ID in a controlled configuration area and, where practical, also label the board or enclosure. Changes require an audit record.

## 12. Credential classes

Separate credentials by purpose:

| Credential | Purpose |
|---|---|
| Wi-Fi credential | join a network |
| device private key | authenticate device to service |
| device certificate | bind public identity |
| update verification key | verify firmware metadata/signature |
| service credential | authorize diagnostic actions |
| recovery authorization | permit destructive recovery |
| backend token | application API access |
| operator credential | authenticate human or workstation |

Do not reuse one secret for all purposes.

## 13. Per-device versus shared credentials

### 13.1 Shared credentials

Advantages:

- easy provisioning;
- simple replacement.

Risks:

- one leaked device compromises the fleet;
- difficult individual revocation;
- poor attribution.

### 13.2 Per-device credentials

Advantages:

- individual revocation;
- better attribution;
- reduced blast radius.

Costs:

- stronger provisioning system;
- inventory and backup requirements;
- certificate lifecycle management.

For deployed fleets, per-device credentials are generally preferred where operationally feasible.

## 14. Secret-generation policy

Secrets should be generated using an approved cryptographic random source.

Do not derive production credentials from:

- predictable serial numbers;
- MAC address alone;
- timestamp alone;
- default passwords;
- repeated human-chosen phrases;
- repository commit hashes;
- seller order numbers.

Record the generation process and responsible system without recording plaintext secrets in ordinary logs.

## 15. Secret-storage locations

Potential locations include:

- encrypted or protected ESP32 Flash partition;
- NVS with appropriate security settings;
- external secure element added by the application;
- backend-only secret not stored on device;
- removable media, generally unsuitable for high-value device secrets.

### 15.1 Auxiliary W25Q32 caution

The routing and intended use of U4 remain unverified. It must not be selected for secret storage until:

- the bus is mapped;
- access control is understood;
- readout resistance is assessed;
- original contents are preserved;
- threat model approves it.

### 15.2 microSD caution

Assume the microSD card can be removed and read. Do not store long-lived private keys there in plaintext.

## 16. Provisioning states

Recommended states:

```text
UNPROVISIONED
PROVISIONING_AUTHORIZED
IDENTITY_ASSIGNED
CREDENTIALS_INSTALLED
CONFIGURED
VERIFIED
ACTIVE
SUSPENDED
REVOKED
DECOMMISSIONED
```

A board should not enter `ACTIVE` until identity, credentials, configuration, firmware and backend enrollment agree.

## 17. Provisioning environment

A controlled provisioning station should include:

- identified workstation;
- versioned provisioning utility;
- authenticated operator;
- controlled network access;
- current approved firmware;
- verified hardware profile;
- audit logging;
- secret redaction;
- time source;
- backup and recovery policy;
- fixture identity;
- restricted access to credential systems.

Avoid provisioning production credentials on an uncontrolled personal computer.

## 18. Provisioning workflow

Recommended sequence:

1. verify incoming-lot and board identity;
2. assign asset ID;
3. run acceptance or minimum security precheck;
4. capture factory firmware if required;
5. install approved bootloader, partition table and application;
6. verify firmware hash;
7. create or retrieve device identity;
8. generate or install credentials;
9. write configuration;
10. enroll the device in backend inventory;
11. run authentication test;
12. run update-verification test;
13. export redacted provisioning receipt;
14. seal or mark provisioning state;
15. move board to active inventory.

### 18.1 Provisioning receipt

Record:

```text
asset ID
hardware profile
lot ID
firmware release ID
binary hashes
partition-table ID
provisioning-tool version
fixture ID
credential key ID or certificate fingerprint
backend enrollment ID
operator
timestamp
verification tests
result
```

Do not place private-key material in the receipt.

## 19. Provisioning authorization

Provisioning actions should require explicit authorization.

Possible controls:

- operator login;
- role-based permissions;
- short-lived enrollment token;
- physical fixture presence;
- one-time challenge;
- dual approval for high-value credentials;
- backend confirmation.

A board connected to USB should not automatically receive production secrets merely because it responds as an ESP32.

## 20. Unprovisioned-device behavior

An unprovisioned board should:

- clearly identify itself as unprovisioned;
- avoid joining production services;
- expose only the minimum provisioning interface;
- reject ordinary application commands;
- avoid default universal administrative passwords;
- not print secrets;
- time out provisioning sessions;
- support safe reset to the unprovisioned state only through policy.

## 21. Firmware release identity

Every executable release should have:

```text
semantic or monotonic release version
source commit
build configuration
hardware-profile compatibility
partition-table version
compiler/platform/framework versions
binary SHA-256
signing-key identifier
release timestamp
security classification
known limitations
rollback compatibility
```

The version displayed on TFT or UART is informational; the verified binary identity is authoritative.

## 22. Reproducible and traceable builds

Where practical:

- pin package versions;
- retain dependency lock information;
- archive toolchain versions;
- generate SBOM or dependency manifest;
- build release binaries in controlled CI;
- retain unsigned and signed artifact hashes;
- separate development and release credentials;
- prevent local unreviewed binaries from being labeled production.

Perfect bit-for-bit reproducibility may require additional work, but traceability is mandatory.

## 23. Firmware authenticity

A hash detects accidental corruption only when the expected hash is obtained through a trusted channel.

For authenticity, use a cryptographic signature or an equivalent authenticated mechanism.

The device should verify:

- signed metadata;
- firmware hash;
- compatible hardware profile;
- allowed version;
- image size and partition destination;
- signing-key status;
- rollback policy.

## 24. Signing-key hierarchy

Recommended separation:

- offline root or recovery authority;
- online release-signing key with limited scope;
- development/test signing key;
- emergency revocation or recovery key;
- update-server transport certificate.

Do not use the same key for development experiments and production releases.

### 24.1 Key identifiers

Firmware metadata should identify the verification key or key generation without exposing private material.

### 24.2 Key custody

Record:

- owner;
- storage method;
- allowed operations;
- backup method;
- rotation policy;
- revocation process;
- compromise response;
- access audit.

## 25. Secure boot

Where enabled and supported, secure boot can make the ROM or bootloader verify later executable stages.

Before enabling an irreversible configuration:

1. verify exact ESP32 revision and supported mode;
2. preserve full Flash backup;
3. verify recovery strategy;
4. test on sacrificial hardware;
5. verify signed bootloader and application flow;
6. document key backup and custody;
7. test valid and invalid images;
8. test power loss and rollback;
9. confirm service requirements;
10. approve irreversible fuse programming.

Do not enable secure boot casually on the only recoverable sample.

## 26. Flash encryption

Flash encryption may protect confidentiality of code and data at rest, but it changes programming, backup, recovery and service procedures.

Questions to resolve:

- which partitions are encrypted;
- whether UART download remains enabled and in what mode;
- how backups are made and restored;
- how development and production modes differ;
- how keys are protected;
- what happens after board replacement;
- how crash dumps are handled;
- how factory and field service operate.

Encryption does not replace signed firmware verification.

## 27. Irreversible security settings

ESP32 eFuse changes can be permanent.

Require:

- written configuration plan;
- reviewed fuse map;
- exact commands and tool versions;
- two-person verification for production;
- known-good power;
- board identity confirmation;
- post-program readback;
- retained audit record;
- sacrificial validation units.

## 28. Partition architecture for trusted updates

A robust update design may include:

```text
bootloader
partition table
NVS/configuration
OTA data
application slot A
application slot B
recovery or factory image where space permits
filesystem/log partitions
```

The exact layout depends on actual program-Flash size and application needs.

### 28.1 Separation goals

- application update should not erase identity unnecessarily;
- configuration migration should be transactional;
- logs should not block boot;
- recovery path should remain independent of optional microSD;
- update metadata should survive controlled resets;
- corrupted inactive image should not affect the running image.

## 29. Update states

Recommended update state machine:

```text
IDLE
METADATA_RECEIVED
METADATA_VERIFIED
IMAGE_DOWNLOADING
IMAGE_COMPLETE
IMAGE_VERIFIED
PENDING_BOOT
TRIAL_BOOT
CONFIRMED
ROLLBACK_REQUESTED
ROLLED_BACK
FAILED
QUARANTINED
```

Each transition should be logged without exposing secrets.

## 30. Update metadata

Metadata should include:

```text
release ID
image version
hardware-profile compatibility
minimum bootloader version
partition target
image size
cryptographic hash
signature
signing-key ID
minimum allowed version
configuration migration version
release channel
expiration or validity policy where used
```

Transport headers alone are insufficient update metadata.

## 31. Update transport

Possible transports:

- authenticated HTTPS download;
- local trusted service host;
- USB/UART service update;
- microSD package;
- application-specific management channel.

Regardless of transport, the image must be authenticated before activation.

### 31.1 TLS caution

TLS security depends on:

- certificate validation;
- trusted root set;
- correct hostname validation;
- usable device time or a safe alternative bootstrap;
- protected credentials;
- maintained libraries.

Disabling certificate validation for convenience invalidates the trust claim.

## 32. Update download behavior

The updater should:

- write only to the inactive slot;
- check available size before writing;
- stream with bounded memory;
- reject unexpected content length;
- detect truncated transfer;
- verify hash after completion;
- verify signature and metadata;
- flush storage before state transition;
- preserve the running image until trial success;
- avoid updating under marginal power.

## 33. Power-loss safety

Test power removal during:

- metadata download;
- image download at several offsets;
- final hash verification;
- OTA state update;
- first trial boot;
- configuration migration;
- confirmation write.

Expected behavior must be defined for each point.

A trusted update system is incomplete until interrupted-update recovery is demonstrated.

## 34. Trial boot and confirmation

After booting a new image:

1. identify trial state;
2. initialize essential hardware;
3. validate configuration schema;
4. run a bounded health check;
5. establish required service connectivity if policy requires;
6. commit the new image only after reaching a healthy checkpoint;
7. otherwise reset into rollback.

Do not confirm immediately at reset vector entry.

## 35. Rollback policy

Rollback may be needed for failed updates, but unrestricted rollback can reintroduce known vulnerabilities.

Define:

- automatic rollback conditions;
- operator-requested rollback authorization;
- minimum allowed security version;
- emergency downgrade procedure;
- retention period for old images;
- configuration compatibility;
- backend compatibility;
- audit requirements.

## 36. Anti-rollback

Anti-rollback may use:

- signed minimum-version metadata;
- monotonic secure version fields;
- protected counters;
- backend policy;
- irreversible silicon features where appropriate.

Before enforcing irreversible anti-rollback, test emergency recovery and signing-key-loss scenarios.

## 37. Configuration migration

Firmware and configuration versions should be independent.

A migration should:

- validate source schema;
- create a backup;
- write to a temporary or alternate record;
- validate the migrated result;
- atomically activate it;
- retain rollback information where feasible;
- never print secrets;
- fail into safe mode rather than repeatedly corrupting data.

## 38. Update channels

Recommended channels:

- `development`;
- `test`;
- `pilot`;
- `stable`;
- `emergency`.

A device should accept only channels authorized by its policy.

### 38.1 Staged rollout

Use:

1. lab hardware;
2. sacrificial or easily recoverable units;
3. pilot group;
4. limited percentage;
5. broader fleet;
6. full release after telemetry review.

## 39. Update authorization

Distinguish:

- release signing;
- distribution authorization;
- device acceptance policy;
- operator initiation;
- backend rollout control.

A validly signed image may still be unauthorized for a particular hardware profile or deployment.

## 40. Update revocation

Maintain the ability to revoke:

- compromised signing key;
- malicious or defective release;
- device certificate;
- backend token;
- service credential;
- update channel.

Revocation behavior should be testable without permanently bricking devices.

## 41. Local USB/UART security

UART0 and the ROM bootloader provide valuable recovery but may also bypass application controls.

Deployment policy should decide:

- whether physical access is considered trusted;
- whether P1 remains accessible;
- whether enclosure opening is detectable;
- whether bootloader download remains enabled;
- whether local service requires authentication;
- what data may be printed at boot;
- whether production secrets can be extracted through service commands.

### 41.1 UART log policy

Production logs should avoid:

- credentials;
- private keys;
- full tokens;
- sensitive user data;
- detailed internal addresses when unnecessary;
- unrestricted crash dumps containing secrets.

### 41.2 Service command authorization

Read-only health commands and destructive commands should have different authorization levels.

## 42. Diagnostic-interface hardening

The service architecture from Part 18 should add:

- authenticated privileged mode;
- command role mapping;
- session timeout;
- replay-resistant challenge where needed;
- state-changing action audit;
- secret redaction;
- rate limiting;
- lockout or delay policy balanced against serviceability;
- physical-presence requirements for destructive actions;
- disabled remote diagnostics by default.

## 43. Touch and on-screen UI security

The display/touch interface should not expose administrative functions merely through a hidden gesture.

For sensitive actions require one or more of:

- operator credential;
- physical enclosure access;
- service token;
- backend approval;
- time-limited challenge;
- serial service session.

Do not display full passwords, tokens or private keys.

## 44. Wi-Fi security

Define:

- approved security modes;
- whether open networks are prohibited;
- credential rotation;
- captive-portal behavior;
- provisioning-network isolation;
- fallback AP policy;
- reconnect and lockout behavior;
- certificate validation;
- network segmentation.

### 44.1 Provisioning access point

If the board exposes a temporary AP:

- use a unique unpredictable password or controlled physical-presence flow;
- limit duration;
- expose only provisioning endpoints;
- disable it after successful provisioning;
- do not reuse a fleet-wide default credential;
- log activation without logging the password.

## 45. Bluetooth security

If BLE or Bluetooth Classic is enabled:

- declare the exact profile;
- require appropriate pairing or authenticated application protocol;
- minimize advertising data;
- avoid permanent discoverable mode;
- restrict writable characteristics;
- validate lengths and states;
- protect firmware-update characteristics with signature verification;
- test unauthorized peers.

Unused Bluetooth should remain disabled.

## 46. Network-service minimization

For every listening service record:

```text
protocol
port
bind interface
authentication
encryption
purpose
owner
rate limit
log policy
update responsibility
disable procedure
```

Do not expose debug HTTP, Telnet-like consoles or unauthenticated command sockets in production.

## 47. API security

Application APIs should consider:

- mutual authentication where justified;
- short-lived tokens;
- least privilege;
- request authentication;
- replay protection;
- bounded payloads;
- rate limiting;
- schema validation;
- secure error messages;
- server-certificate validation;
- key rotation;
- backend revocation.

## 48. Input validation

All external inputs require bounds and type checks:

- UART commands;
- touch input state transitions;
- network packets;
- JSON/CBOR fields;
- file names;
- SD file contents;
- configuration values;
- update metadata;
- BLE characteristics;
- sensor values used for control.

Reject oversized, malformed and ambiguous inputs.

## 49. File and microSD security

Assume removable media is attacker-controlled.

The application should:

- validate file paths;
- reject traversal attempts;
- bound file sizes;
- verify signed update packages;
- separate user data from executable trust decisions;
- avoid automatic execution of scripts from card;
- handle malformed files safely;
- never treat a filename such as `official.bin` as proof of authenticity.

## 50. Auxiliary Flash security

Before U4 is used:

- map its pins and controller ownership;
- identify whether it is accessible from ordinary firmware;
- preserve original contents;
- define read/write authorization;
- define integrity protection;
- assess whether contents are exposed physically;
- include it in backup and decommissioning policy.

## 51. Logging and audit security

Security logs may include:

- provisioning events;
- update attempts and results;
- signature-verification failures;
- configuration changes;
- privileged service sessions;
- repeated authentication failures;
- revocation state;
- unexpected reset clusters;
- rollback events;
- factory-reset and decommission actions.

### 51.1 Audit integrity

Where important, export logs to a trusted external system. Local logs on a physically accessible board are not necessarily tamper-proof.

### 51.2 Privacy

Collect only data required for operation and incident response.

## 52. Time and freshness

Security protocols may depend on time for:

- certificate validity;
- token expiration;
- audit ordering;
- update windows;
- replay prevention.

The board has no documented battery-backed real-time clock in this reference.

Define:

- bootstrap behavior before network time;
- trusted time source;
- behavior when time moves backward;
- retained monotonic counters where possible;
- certificate-validation policy without valid time;
- offline update policy.

## 53. Randomness

Cryptographic operations require suitable entropy.

Do not use:

- `millis()`;
- ADC noise without analysis;
- MAC address;
- predictable boot count;
- C library pseudo-random generator alone;

as the sole source for cryptographic secrets.

## 54. Memory-safety and robustness

Although Arduino/C++ development commonly uses manual buffers, security-sensitive code should:

- bound all copies;
- avoid unchecked formatting;
- validate lengths before allocation;
- limit recursion;
- handle allocation failure;
- avoid parsing untrusted data into fixed arrays without checks;
- use compiler warnings;
- add unit and fuzz tests for parsers where practical.

## 55. Dependency and vulnerability management

Maintain:

```text
PlatformIO platform version
framework version
library versions
source and package origin
license
known advisories
last review date
update owner
```

A new library release should not be adopted automatically without regression and security review.

## 56. Software bill of materials

A release SBOM or dependency manifest should identify:

- direct libraries;
- transitive packages where available;
- toolchain;
- build scripts;
- binary hashes;
- license and source locations;
- known exceptions.

## 57. CI security controls

Recommended CI checks:

- build from pinned configuration;
- secret scanning;
- dependency inventory;
- static analysis where available;
- test-vector execution;
- update-metadata schema validation;
- signature verification tests;
- negative tests with modified image;
- parser tests;
- artifact hash generation;
- protected release workflow;
- separation of pull-request builds from production signing.

Untrusted pull-request code must not gain access to production signing keys.

## 58. Release signing workflow

Recommended flow:

1. reviewed source reaches protected release commit;
2. controlled CI builds unsigned artifacts;
3. tests and hardware qualification complete;
4. release manifest is generated;
5. authorized signing service signs approved artifacts;
6. signatures and hashes are verified independently;
7. artifacts enter immutable release storage;
8. rollout authorization is recorded;
9. post-release monitoring begins.

## 59. Signing-key compromise response

Prepare before compromise:

- identify affected keys and releases;
- stop distribution;
- revoke key in backend and device policy where possible;
- issue replacement trust metadata;
- sign emergency recovery release with separate authority;
- identify devices that accepted affected releases;
- preserve evidence;
- rotate related credentials;
- publish internal incident record.

## 60. Device credential compromise

Response may include:

1. suspend device identity;
2. block backend access;
3. quarantine physical board;
4. collect logs and firmware identity;
5. determine whether secrets were fleet-shared;
6. rotate affected credentials;
7. reprovision or retire the board;
8. verify no duplicate identity remains active.

## 61. Lost or stolen device

Immediately:

- mark asset lost;
- revoke device certificate/token;
- disable update or command permissions;
- rotate shared credentials if any;
- review recent activity;
- identify data exposure;
- preserve inventory and incident records.

Assume physical extraction may be attempted unless hardware protections are proven.

## 62. Security incident categories

Suggested categories:

```text
CREDENTIAL_EXPOSURE
UNAUTHORIZED_FIRMWARE
UPDATE_FAILURE
SIGNING_KEY_EVENT
NETWORK_INTRUSION
SERVICE_INTERFACE_ABUSE
LOST_DEVICE
SUPPLY_CHAIN_ANOMALY
DATA_EXPOSURE
PHYSICAL_TAMPER
MALWARE_ON_BUILD_HOST
BACKEND_COMPROMISE
```

## 63. Incident response phases

1. detect;
2. contain;
3. preserve evidence;
4. identify affected devices and credentials;
5. eradicate cause;
6. recover through trusted images and credentials;
7. requalify;
8. monitor;
9. document lessons and preventive actions.

## 64. Evidence preservation

Preserve where relevant:

- full program-Flash image;
- auxiliary-Flash image;
- microSD image;
- UART logs;
- firmware hashes;
- update metadata;
- backend logs;
- credential and revocation records;
- photographs;
- network captures;
- tool versions;
- custody record.

Do not overwrite the suspect device with a recovery image before deciding whether evidence is needed.

## 65. Recovery trust chain

Recovery artifacts must be at least as controlled as ordinary updates.

Verify:

- recovery-image signature;
- source and release ID;
- hardware compatibility;
- write addresses;
- partition layout;
- backup hash;
- operator authorization;
- post-recovery firmware identity;
- credential rotation;
- backend re-enrollment.

## 66. Factory reset security

A factory reset is not equivalent to secure decommissioning.

Define which items remain:

- asset ID;
- device identity;
- private keys;
- Wi-Fi credentials;
- logs;
- calibration;
- user data;
- update trust keys;
- microSD files;
- auxiliary-Flash data.

A user reset should normally preserve update-verification trust but may clear ordinary configuration.

## 67. Decommissioning

Recommended sequence:

1. remove device from service;
2. revoke backend identity and tokens;
3. export required records;
4. remove user data;
5. clear Wi-Fi and application credentials;
6. clear or invalidate device private keys where supported;
7. remove or sanitize microSD;
8. address auxiliary-Flash data;
9. verify the device cannot authenticate;
10. mark asset `DECOMMISSIONED`;
11. physically destroy storage only when policy requires;
12. recycle hardware through an approved route.

## 68. Credential rotation

Define rotation for:

- Wi-Fi credentials;
- device certificates;
- API tokens;
- service credentials;
- signing keys;
- trusted roots.

Rotation must include overlap and recovery planning so valid devices are not stranded.

## 69. Certificate lifecycle

Record:

```text
certificate fingerprint
issuer
subject/device ID
validity period
key algorithm
provisioning event
renewal method
revocation status
replacement certificate
```

Plan renewal before expiration and test behavior with expired, not-yet-valid and revoked credentials.

## 70. Backend authorization

The backend should not trust a device solely because TLS connected successfully.

Authorization should consider:

- device identity;
- active/revoked status;
- assigned role;
- allowed commands;
- firmware minimum version;
- hardware profile;
- lifecycle state;
- rate limits;
- tenant or deployment boundary.

## 71. Command security

Remote commands should include:

- authenticated source;
- device target;
- command ID;
- authorization scope;
- expiration;
- replay protection;
- bounded parameters;
- acknowledgement;
- audit trail.

Dangerous commands such as erase, update-channel change, credential reset or actuator control require stronger policy.

## 72. Denial-of-service resilience

Consider:

- connection limits;
- request rate limits;
- bounded queues;
- watchdog-safe parsing;
- storage-write limits;
- authentication before expensive operations;
- Wi-Fi reconnect backoff;
- log-rate limiting;
- recovery after malformed traffic.

Do not allow unauthenticated traffic to fill Flash logs indefinitely.

## 73. Safe failure behavior

On security validation failure:

- reject the action;
- preserve current trusted firmware;
- emit a bounded audit event;
- avoid leaking secret details;
- remain recoverable;
- enter safe mode if integrity is uncertain;
- avoid automatic destructive reset.

## 74. Security telemetry

Possible non-sensitive telemetry:

- firmware release ID;
- verified-boot status;
- update state;
- last update result;
- rollback count;
- authentication-failure count;
- credential expiration horizon;
- reset reason;
- lifecycle state;
- security policy version.

Telemetry must not disclose private keys or credentials.

## 75. Physical hardening

Depending on risk:

- place board in controlled enclosure;
- restrict USB and P1 access;
- label service access;
- use tamper-evident seals;
- avoid exposed microSD if sensitive data is present;
- prevent accidental BOOT-button access;
- record enclosure openings;
- keep antenna performance acceptable.

Physical hardening must not create unsafe thermal conditions.

## 76. Supply-chain security integration

Use Part 19 records to verify:

- board revision;
- module and USB bridge identity;
- Flash size and manufacturer;
- unexpected firmware;
- component substitutions;
- seller and lot history;
- package tampering;
- factory-image hash.

A board with unexplained revision differences should be quarantined before receiving credentials.

## 77. Hardware-profile binding

Firmware metadata should identify compatible hardware profiles.

Examples of relevant differences:

- CH340G versus other USB bridge;
- display controller;
- touch controller;
- regulator arrangement;
- connector routing;
- program-Flash size;
- auxiliary-Flash presence;
- GPIO ownership.

Security cannot be separated from correct hardware targeting because a wrong pin map can disable recovery or create unsafe behavior.

## 78. Backup security

Backups may contain secrets.

Protect:

- full Flash images;
- NVS dumps;
- configuration exports;
- private keys;
- microSD images;
- incident bundles.

Define encryption at rest, access control, retention, deletion and restoration audit.

## 79. Recovery-key escrow

If recovery depends on a unique key:

- maintain controlled backup;
- use dual custody where appropriate;
- test restoration;
- document loss consequences;
- separate recovery key from ordinary operator access;
- rotate after suspected exposure.

## 80. Security test strategy

### 80.1 Positive tests

- valid signed image accepted;
- authorized device authenticates;
- valid credential rotation succeeds;
- normal update and confirmation succeed;
- recovery image restores service;
- redacted logs remain useful.

### 80.2 Negative tests

- modified firmware rejected;
- wrong signing key rejected;
- incompatible hardware profile rejected;
- truncated image rejected;
- replayed command rejected;
- expired credential rejected according to policy;
- unauthorized service command rejected;
- malformed configuration does not crash parser;
- microSD package without valid signature rejected.

### 80.3 Fault tests

- power loss during update;
- network loss during update;
- corrupted OTA metadata;
- invalid trial image;
- full log partition;
- missing time source;
- backend unavailable;
- credential revoked during operation.

## 81. Penetration and abuse testing

A controlled assessment may include:

- port and service enumeration;
- unauthenticated command attempts;
- protocol fuzzing;
- oversized payloads;
- repeated login attempts;
- BLE characteristic abuse;
- malicious SD files;
- UART parser noise;
- update-package tampering;
- rollback attempts;
- physical bootloader access review.

Tests must be authorized and conducted on isolated hardware or networks.

## 82. Security acceptance levels

Suggested levels:

| Level | Meaning |
|---|---|
| S0 | security architecture documented |
| S1 | secrets excluded from source and logs |
| S2 | unique identity and controlled provisioning demonstrated |
| S3 | authenticated network operation demonstrated |
| S4 | signed update and rollback demonstrated |
| S5 | secure boot/encryption profile validated if required |
| S6 | incident, revocation and decommission drills completed |
| S7 | independent security review and field evidence obtained |

## 83. Security release gate

Before deployment require:

1. threat model approved;
2. hardware profile confirmed;
3. release binary and manifest archived;
4. signing verification passed;
5. secrets scan passed;
6. provisioning process tested;
7. device identity linked to asset record;
8. remote services enumerated;
9. default credentials absent;
10. update and rollback tested;
11. invalid image rejected;
12. logs redacted;
13. recovery procedure tested;
14. incident contacts and revocation path known;
15. decommissioning procedure defined;
16. open risks accepted.

## 84. Security provisioning record template

```text
Provisioning record ID:
Asset ID:
Hardware profile:
Lot ID:
Board revision evidence:
Firmware release:
Bootloader/partition versions:
Binary hashes:
Security profile:
Secure-boot state:
Flash-encryption state:
Device identity:
Certificate fingerprint:
Credential key IDs:
Backend enrollment:
Provisioning station:
Tool version:
Fixture ID:
Operator:
Timestamp:
Verification tests:
Result:
Exceptions:
Approval:
```

## 85. Update record template

```text
Asset/device ID:
Previous release:
Target release:
Update channel:
Metadata hash/signature:
Signing-key ID:
Transport:
Start/end time:
Power condition:
Download result:
Image hash result:
Signature result:
Trial boot result:
Health checkpoint:
Confirmation result:
Rollback result:
Configuration migration:
Operator/backend authorization:
Audit-log reference:
```

## 86. Incident record template

```text
Incident ID:
Detection time:
Reporter:
Affected assets:
Category:
Observed indicators:
Current firmware IDs:
Credential IDs:
Network/backend evidence:
Physical evidence:
Containment:
Revocations:
Images/logs preserved:
Root cause:
Recovery release:
Reprovisioning:
Requalification:
Residual risk:
Closure approval:
```

## 87. Unsupported claims

Until implementation and test evidence exist, do not claim:

- secure boot is enabled;
- Flash encryption is enabled;
- firmware confidentiality against physical access;
- signed OTA is operational;
- rollback is power-loss safe;
- anti-rollback is enforced;
- per-device certificates are provisioned;
- private keys are hardware-isolated;
- UART and ROM bootloader are disabled;
- remote diagnostics are authenticated;
- Bluetooth pairing is secure;
- auxiliary Flash is protected;
- microSD data is confidential;
- build and signing infrastructure is hardened;
- credentials are securely erased;
- the system meets a named security standard or certification.

## 88. Open implementation tasks

- Define the deployment-specific threat model.
- Choose security profiles for lab, pilot and deployed boards.
- Define device-ID and certificate schema.
- Build a controlled provisioning utility.
- Remove all production secrets from source and build configuration.
- Define protected configuration storage.
- Define signing-key hierarchy and custody.
- Add signed release manifests.
- Select and test OTA partition layout.
- Implement image signature verification.
- Implement trial boot, health confirmation and rollback.
- Test interrupted updates at multiple stages.
- Define anti-rollback policy.
- Add authenticated privileged diagnostic sessions.
- Define Wi-Fi and BLE provisioning security.
- Inventory all listening services.
- Add audit and security telemetry with redaction.
- Add dependency and vulnerability review.
- Add CI negative tests for tampered images and metadata.
- Perform recovery, key-rotation, revocation and decommission drills.
- Evaluate secure boot and Flash encryption on sacrificial boards before irreversible use.

## 89. Completion criteria

The architecture can be considered sample-qualified for a declared deployed profile when:

1. assets and threats are documented;
2. trust zones and security boundaries are reviewed;
3. every active network and physical service interface is inventoried;
4. unique asset and device identities are linked;
5. provisioning is authenticated and audited;
6. secrets are unique where policy requires;
7. no production secrets exist in the public repository or ordinary logs;
8. firmware releases have stable identity, hashes and signatures;
9. device-side authenticity verification is demonstrated;
10. incompatible or modified images are rejected;
11. update writes only the declared inactive target;
12. power-loss recovery is demonstrated;
13. trial boot and confirmation are implemented;
14. rollback policy is enforced;
15. minimum-version or anti-rollback policy is documented;
16. configuration migration is transactional;
17. UART and diagnostic privileges follow deployment policy;
18. Wi-Fi, Bluetooth and APIs use declared authentication controls;
19. security logs are bounded and redacted;
20. credential rotation and revocation are demonstrated;
21. lost-device procedure is exercised;
22. recovery artifacts are authenticated and tested;
23. backups containing secrets are access-controlled;
24. factory reset and decommissioning scopes are verified;
25. signing-key compromise response is documented;
26. supply-chain anomalies block provisioning;
27. negative and abuse tests pass;
28. independent review findings are resolved or accepted;
29. all irreversible settings have reviewed recovery plans;
30. unsupported claims remain explicit.

## 90. Summary

The trusted architecture must bind four things that are often treated separately:

```text
physical board identity
approved firmware identity
provisioned credential identity
backend authorization state
```

A device is trustworthy only while those four remain consistent and recoverable.

The central rule is:

> never install secrets before the hardware and firmware identity are established, never execute an update merely because it downloaded successfully, and never declare a device retired until its active credentials have been revoked.