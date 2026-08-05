# Manufacturing, Procurement and Revision-Control System — ESP32-2432S028

> **Document status:** sample-specific manufacturing, sourcing, incoming-inspection, and revision-control reference for the author's `ESP32-2432S028` board.  
> **Scope:** supplier evidence, incoming-lot identification, counterfeit and substitution risk, board-revision comparison, approved components and cables, traceability, qualification inheritance, change notification, procurement acceptance, and release control.  
> **Evidence rule:** a seller title, marketplace photograph, or familiar CYD appearance does not prove electrical, mechanical, firmware, or manufacturing equivalence. Each lot and revision must be identified and accepted against retained evidence.

## 1. Purpose

The ESP32-2432S028 family is sold under similar names while differing in details that materially affect firmware, wiring, enclosure fit, service procedures, and reliability.

This chapter defines a controlled system for answering:

- What exactly was ordered?
- What exactly arrived?
- Which board revision and component population are present?
- Which differences are cosmetic, functional, qualification-relevant, or unsafe?
- Which supplier claims are verified, unverified, contradictory, or obsolete?
- Can previous acceptance and reliability evidence be inherited by a new lot?
- What tests are required before boards enter development, stock, deployment, or production use?
- How are changes, substitutions, and supplier drift recorded?

The objective is not to create a fictional manufacturer BOM. The objective is to maintain a defensible sample- and lot-specific configuration record.

## 2. Procurement risks for CYD-class boards

Common risks include:

- multiple PCB revisions sold under one listing;
- suffix changes such as `ESP32-2432S028` versus `ESP32-2432S028R`;
- CH340G versus CH340C or another USB-UART variant;
- display-controller substitution;
- connector-family substitution;
- different GPIO routing;
- omitted or added components;
- regulator substitution;
- different ESP32 module or Flash population;
- altered touch-panel geometry;
- changed mounting holes or display position;
- recycled or relabeled components;
- seller photographs that do not match shipped goods;
- silent lot mixing;
- firmware examples copied from another revision;
- undocumented factory programming.

A familiar yellow PCB is not a sufficient identity criterion.

## 3. Procurement object hierarchy

Track at least these levels:

```text
product family
seller listing
purchase order
shipment
incoming lot
packaging unit
individual board
installed assembly
```

### 3.1 Product family

A broad commercial family such as `ESP32-2432S028` or CYD.

### 3.2 Seller listing

A specific marketplace or distributor listing at a specific date. Listings can change without preserving historical content.

### 3.3 Purchase order

The exact quantity, options, price, requested revision, accessories, and agreed evidence.

### 3.4 Incoming lot

Boards believed to share one manufacturing origin or shipment population.

### 3.5 Individual board

A uniquely identified physical sample with its own photographs, acceptance record, firmware history, and lifecycle state.

## 4. Required procurement record

Record:

```text
purchase record ID
supplier legal or marketplace identity
seller account/store name
listing URL or archived snapshot
listing title
order date
order number
requested model and suffix
requested quantity
received quantity
unit price and currency
shipping method
seller-stated specifications
seller-stated revision
seller-stated controller types
included cables/accessories
packaging condition
receipt date
incoming lot ID
inspector
acceptance disposition
```

Retain screenshots or exported listing evidence because online content may later change.

## 5. Supplier-evidence classes

Use explicit classes:

- **Manufacturer-controlled** — official manufacturer drawing, datasheet, BOM, PCN, or signed statement;
- **Distributor-controlled** — authorized distributor record with traceable manufacturer data;
- **Seller-controlled** — marketplace listing, seller message, or product photograph;
- **Sample-observed** — direct observation from received boards;
- **Measured** — electrical, mechanical, or software measurement;
- **Community-reported** — third-party repository, forum, or pinout;
- **Inferred** — plausible but not verified;
- **Contradicted** — evidence conflicts with the received sample;
- **Open** — not established.

Seller-controlled evidence can guide inspection but should not override the received sample.

## 6. Pre-order specification package

Before ordering a controlled lot, define the required configuration.

Example fields:

```text
PCB marking required
ESP32 module family
minimum program-Flash size
USB-UART device requirement
TFT size and resolution class
display controller requirement or allowed set
touch controller requirement
microSD presence
auxiliary Flash requirement
regulator topology requirement
audio amplifier requirement
connector count and labels
mechanical envelope
mounting-hole pattern
acceptable substitutions
prohibited substitutions
required accessories
required test evidence
labeling and packaging requirements
```

If a parameter is unknown, mark it `OPEN` rather than silently accepting any implementation.

## 7. Approved configuration baseline

The current documented sample baseline includes:

- PCB marking `ESP32-2432S028`;
- ESP-WROOM-32 module;
- 2.8-inch 240 × 320-class TFT;
- XPT2046 touch controller;
- W25Q32JV-class 32-Mbit auxiliary Flash;
- CH340G USB-to-UART bridge;
- 8002A mono BTL audio amplifier;
- two AMS1117-3.3 regulators;
- microSD socket;
- BOOT and RESET buttons;
- P1, P3, CN1, and P4 connectors;
- CN1 with a visible `NC` contact position;
- current repository pin profile documented in Part 15.

This is a sample baseline, not proof that every board sold under the same name matches it.

## 8. Incoming-lot identity

Assign each shipment a lot identifier before boards are mixed with existing stock.

Record:

```text
lot ID
supplier and order ID
receipt date
quantity
outer-package photographs
inner-package photographs
label text and barcodes
ESD packaging condition
moisture or contamination evidence
visible lot/date codes
sample-selection method
inspection level
lot disposition
```

### 8.1 Do not mix lots early

Keep lots physically separated until identity and acceptance are complete.

### 8.2 Mixed lots

If visible or measured differences appear within one shipment, split it into sub-lots.

## 9. Sampling plan

The sampling plan should depend on risk, quantity, supplier history, and intended use.

Possible levels:

- 100% visual identity inspection;
- sampled dimensional inspection;
- sampled component-marking audit;
- sampled electrical acceptance;
- 100% firmware quick test;
- increased sampling after any discrepancy;
- full inspection after supplier or revision change.

Do not claim a statistical quality level unless the sample plan and acceptance-number method are explicitly defined.

## 10. Receiving quarantine

New boards should begin in `QUARANTINED` or `ACCEPTANCE_PENDING` state.

Do not immediately:

- install them into products;
- mix them with known-good stock;
- flash over unknown factory contents;
- discard labels and packaging;
- remove protective films needed for evidence;
- assume the previous firmware profile is safe.

## 11. Incoming visual inspection

Inspect both sides and compare against the approved baseline.

Check:

- PCB model marking;
- solder-mask color and finish;
- silkscreen revision identifiers;
- ESP32 module marking and antenna orientation;
- CH340 package marking;
- touch-controller marking;
- auxiliary-Flash marking;
- regulator count and marking;
- audio-amplifier marking;
- connector count and population;
- CN1/P3/P1/P4 labels;
- microSD socket style;
- TFT flex and touch-panel alignment;
- mounting holes;
- missing, rotated, damaged, or contaminated parts;
- solder bridges and tombstoned passives;
- rework evidence;
- scratched traces or cracked PCB;
- counterfeit-warning signs.

## 12. Photographic evidence standard

For each new lot retain:

- full component-side image;
- full display-side image;
- module and antenna area;
- power and USB area;
- touch and auxiliary-Flash area;
- audio area;
- connector labels;
- PCB marking and revision text;
- packaging labels;
- representative anomalies.

Use a scale, stable orientation, adequate focus, and non-destructive lighting.

## 13. Board-revision fingerprint

Create a fingerprint that combines visible and measured features.

Example:

```text
PCB text and revision
board dimensions
hole coordinates
TFT position
connector positions
component population
major IC markings
regulator count
USB connector type
microSD socket type
CN1/P3 labels
GPIO profile
TFT controller identity
touch controller identity
program-Flash identity
auxiliary-Flash identity
factory firmware hash if preserved
```

A board revision should not be defined by only one ambiguous marking.

## 14. Revision naming policy

Use internal revision IDs independent of seller naming.

Example:

```text
CYD-SAMPLE-A
CYD-LOT-2026-08-A
CYD-HWPROFILE-01
CYD-HWPROFILE-02
```

Map external names to internal profiles:

```text
seller name: ESP32-2432S028R
internal profile: CYD-HWPROFILE-02
confidence: measured
```

## 15. Revision-difference classification

Classify each difference:

- **D0 Cosmetic** — no intended functional effect;
- **D1 Documentation** — labels or markings changed;
- **D2 Mechanical** — dimensions, holes, connectors, display position;
- **D3 Component substitution** — nominally equivalent device or package;
- **D4 Electrical** — net, rail, pull-up, protection, or loading change;
- **D5 Firmware-visible** — controller, pin map, timing, Flash, or protocol change;
- **D6 Safety/reliability** — thermal, voltage, BTL, isolation, or environmental impact;
- **D7 Unknown** — significance not yet determined.

A D7 difference blocks automatic qualification inheritance.

## 16. Change significance matrix

| Change | Typical minimum response |
|---|---|
| silkscreen typography only | document and compare |
| connector color change | fit and material review |
| CH340G to CH340C | USB/UART and auto-reset regression |
| regulator brand or type change | rail, load, transient, and thermal requalification |
| TFT controller change | display-driver and visual regression |
| touch controller change | raw data, IRQ, calibration, and stress requalification |
| ESP32 module change | identity, Flash, RF, power, firmware, and compliance review |
| pin-routing change | full pin-map and subsystem regression |
| mounting-hole change | enclosure requalification |
| audio amplifier change | BTL topology, load, power, distortion, and thermal review |
| unknown missing component | quarantine and schematic/measurement investigation |

## 17. Component-marking audit

For representative samples, record readable markings for:

- ESP32 module;
- USB-UART IC;
- touch controller;
- auxiliary Flash;
- audio amplifier;
- regulators;
- crystals;
- significant transistors or protection devices;
- display flex codes where accessible.

### 17.1 Marking limitations

A matching top mark is not proof of authenticity. A differing mark is not automatically failure if an approved substitution exists.

## 18. Counterfeit and suspect-part indicators

Potential indicators include:

- inconsistent fonts or laser marking;
- sanding or resurfacing evidence;
- date codes inconsistent within one lot;
- mixed package finishes;
- poor lead plating;
- unexplained rework residue;
- incorrect package dimensions;
- device identity inconsistent with electrical behavior;
- seller refusal to provide stable configuration information;
- impossible or contradictory specifications;
- large unexplained price anomaly.

These indicators require investigation; they are not conclusive alone.

## 19. Substitution-control policy

Every substitution should be classified as:

- approved equivalent;
- conditionally approved;
- evaluation-only;
- prohibited;
- unknown.

Record:

```text
original part or function
substitute
supplier/manufacturer
reason
package compatibility
electrical compatibility
firmware impact
mechanical impact
thermal impact
reliability impact
required tests
approval
applicable lots
```

## 20. Approved-vendor and approved-item lists

Maintain separate controlled lists for:

- complete boards;
- USB cables;
- connector housings;
- crimp terminals;
- preassembled pigtails;
- speakers;
- microSD cards;
- power supplies;
- enclosures;
- mounting hardware;
- service adapters.

Approval should reference a tested article number or retained sample, not only a generic description.

## 21. Connector procurement

The current connector family is a procurement candidate resembling 1.25-mm-pitch PicoBlade-compatible or SCT1251-style products. Exact manufacturer identity remains open.

Before volume purchase verify:

- pitch;
- cavity count;
- latch geometry;
- polarization;
- housing dimensions;
- contact dimensions;
- wire gauge range;
- insertion force;
- retention force;
- current rating;
- mating durability;
- side-entry orientation;
- fit on each board connector.

Do not approve by pitch alone.

## 22. Cable acceptance

For each cable or pigtail record:

- supplier and article number;
- conductor count;
- pitch and housing;
- wire gauge;
- insulation;
- length;
- color sequence;
- terminal plating if known;
- continuity map;
- pull/retention result;
- voltage-drop result where relevant;
- approved connector orientation.

Wire color must not define electrical pin identity.

## 23. USB cable control

A USB cable can affect:

- power-drop behavior;
- upload reliability;
- serial stability;
- brownout susceptibility;
- current measurement.

Approve cables using:

- data connectivity;
- resistance or voltage-drop test;
- connector fit;
- repeated programming;
- movement test;
- declared length and gauge.

## 24. microSD procurement

Approved microSD records should include:

- brand and exact model;
- capacity;
- card identity where readable;
- filesystem preparation;
- read/write/hash test;
- power-cycle behavior;
- operating-temperature requirement;
- endurance class if relevant;
- supplier traceability;
- counterfeit-screening method.

Marketplace capacity labels alone are insufficient.

## 25. Speaker and audio-load procurement

Record:

- nominal impedance;
- power rating;
- diameter and depth;
- connector/cable;
- polarity convention for assembly only;
- enclosure compatibility;
- measured current and thermal behavior;
- audible/mechanical artifacts.

Remember that the board output is BTL; neither speaker lead is ground.

## 26. Power-supply procurement

An approved supply record should include:

- rated voltage/current;
- regulation and ripple evidence;
- connector and cable;
- overload behavior;
- isolation and safety status appropriate to use;
- observed 5-V rail at the board under combined load;
- thermal behavior;
- regional compliance where required.

## 27. Factory firmware preservation

Before flashing a representative sample from each lot:

- capture boot log;
- identify Flash size and mode;
- read full program Flash if possible;
- hash the image;
- record partition table;
- photograph factory UI;
- record visible version strings;
- preserve any auxiliary-Flash contents read-only;
- preserve included microSD contents where applicable.

Factory firmware may contain useful board-identification or calibration evidence.

## 28. Incoming electrical inspection

Minimum staged checks may include:

1. unpowered resistance inspection;
2. current-limited first power-up;
3. 5-V and 3.3-V rail measurements;
4. USB enumeration;
5. ROM bootloader communication;
6. program-Flash identity;
7. conservative golden diagnostic firmware;
8. QUICK suite;
9. selected STANDARD tests on sampled units;
10. thermal observation under declared load.

Use Part 16 procedures and Part 18 tooling.

## 29. Incoming functional matrix

Recommended functions:

| Subsystem | 100% or sample | Evidence |
|---|---|---|
| visual identity | 100% | inspection record/photo as required |
| USB enumeration | 100% for controlled deployment | log |
| firmware upload | 100% or risk-based | tool log |
| TFT | 100% quick visual | test result |
| RGB/BOOT/LDR | 100% quick | serial result |
| touch | sampled or 100% by use | raw/corner result |
| microSD | sampled or 100% by use | read/hash/write result |
| Wi-Fi | sampled or 100% by use | scan/association result |
| audio | sampled or 100% by use | approved-load result |
| expansion | sampled by profile | fixture result |
| thermal | sampled per lot/revision | measurement record |

The actual plan must be declared before inspection.

## 30. Measurement-system control

Procurement acceptance depends on controlled tools.

Record:

- instrument identity;
- calibration or verification status;
- fixture revision;
- firmware build;
- test script version;
- operator;
- environmental conditions where relevant.

A supplier dispute is difficult to support with an undocumented fixture.

## 31. Golden comparison sample

Maintain one or more golden boards that are:

- fully identified;
- accepted;
- photographed;
- electrically characterized;
- protected from casual modification;
- periodically reverified;
- representative of a specific approved profile.

A golden sample is not immortal; its condition and calibration history must be tracked.

## 32. Comparison methods

Useful comparisons include:

- side-by-side macro photography;
- component-population overlay;
- measured dimensions;
- continuity comparison;
- resistance-to-ground comparison;
- idle current;
- rail voltages;
- boot logs;
- controller IDs;
- Flash identities;
- firmware test results;
- thermal images;
- Wi-Fi baseline;
- connector fit.

## 33. Qualification inheritance

A new lot may inherit prior qualification only when similarity is demonstrated.

Possible inheritance levels:

- **Q0 None** — entirely new or unknown;
- **Q1 Documentation only** — same seller claim, no sample evidence;
- **Q2 Visual similarity** — matching photographs and population;
- **Q3 Functional similarity** — matching QUICK/STANDARD results;
- **Q4 Electrical/mechanical similarity** — measured equivalence within declared criteria;
- **Q5 Controlled revision continuity** — manufacturer/supplier change evidence plus validation;
- **Q6 Full inheritance approved** — explicit engineering approval for declared use.

Visual similarity alone should not grant full inheritance.

## 34. Requalification triggers

Mandatory review should occur after:

- supplier change;
- listing change;
- price or lead-time anomaly suggesting source change;
- PCB marking or revision change;
- component-marking change;
- connector or mechanical change;
- TFT/touch behavior change;
- USB-UART identity change;
- regulator or power behavior change;
- program-Flash size or mode change;
- GPIO mapping discrepancy;
- thermal trend change;
- failure-rate increase;
- packaging or handling change;
- unexplained mixed population;
- counterfeit concern;
- long gap between purchases.

## 35. Supplier change notification

For controlled purchases, request notification before changes to:

- PCB revision;
- BOM;
- component manufacturer;
- display module;
- touch panel/controller;
- ESP32 module;
- Flash devices;
- USB-UART bridge;
- regulator;
- audio amplifier;
- connector family;
- factory firmware;
- manufacturing site;
- packaging.

Marketplace sellers may not provide formal PCNs; therefore incoming verification remains necessary.

## 36. Nonconforming-material process

When a discrepancy is found:

1. stop stock mixing;
2. identify affected lot and quantity;
3. quarantine material;
4. preserve packaging and evidence;
5. describe the requirement and observed result;
6. assess safety and downstream exposure;
7. increase sampling if appropriate;
8. notify supplier;
9. choose return, rework, conditional use, evaluation, or scrap;
10. record disposition and approval.

## 37. Nonconformance categories

- identity mismatch;
- quantity mismatch;
- visible damage;
- workmanship defect;
- component substitution;
- mechanical incompatibility;
- firmware incompatibility;
- electrical failure;
- safety concern;
- documentation discrepancy;
- packaging/ESD issue;
- counterfeit/suspect material;
- mixed lot;
- unknown deviation.

## 38. Supplier corrective-action evidence

For significant issues request, where practical:

- affected-lot definition;
- containment action;
- root cause;
- corrective action;
- recurrence-prevention action;
- replacement/credit plan;
- evidence of revised inspection.

Do not close a recurring issue solely because replacement boards were sent.

## 39. Workmanship inspection

Inspect representative or all units for:

- solder bridges;
- insufficient solder;
- disturbed joints;
- tombstoned passives;
- rotated polarized parts;
- connector misalignment;
- lifted pads;
- flux or ionic residue;
- debris under display;
- damaged flex;
- loose buttons;
- scratched antenna region;
- poor module seating;
- cracked solder around USB and microSD.

Acceptance criteria should use photographs or recognized workmanship references where needed.

## 40. ESD and handling control

Define:

- ESD-safe receiving area;
- grounded handling tools;
- packaging retention;
- touch/display protection;
- connector insertion method;
- board stacking prohibition unless protected;
- antenna-area protection;
- storage humidity and contamination controls appropriate to use.

Do not make formal ESD-compliance claims unless the process is actually audited.

## 41. Storage control

Record:

- lot and board IDs;
- location;
- packaging state;
- environmental limits;
- first-in/first-out or other policy;
- periodic inspection requirement;
- firmware/configuration state;
- spare-board qualification date.

Long-stored boards should be rechecked before deployment.

## 42. Traceability labels

A board or packaging label may include:

```text
asset ID
incoming lot ID
internal hardware profile
acceptance status/date
firmware state
```

Do not cover the antenna, component markings, connectors, display, or ventilation.

## 43. Digital traceability

Link each board to:

- order and supplier;
- lot;
- photographs;
- hardware profile;
- incoming results;
- firmware history;
- installed application;
- maintenance and repair records;
- retirement record.

MAC address may assist but should not be the sole asset key.

## 44. Configuration-controlled records

Control versions of:

- hardware profile;
- pin map;
- approved-item list;
- inspection checklist;
- test firmware;
- test limits;
- fixture wiring;
- procurement specification;
- supplier evidence;
- golden-sample record;
- deviation approvals.

## 45. Engineering change process

Recommended states:

```text
PROPOSED
UNDER_REVIEW
EVALUATION
APPROVED
IMPLEMENTED
VERIFIED
REJECTED
SUPERSEDED
```

Every change should identify:

- reason;
- affected items;
- old and new state;
- compatibility;
- risks;
- required tests;
- inventory impact;
- documentation updates;
- rollback plan;
- approval.

## 46. Deviation and concession

A deviation allows a known difference for a defined scope.

Record:

```text
deviation ID
requirement
observed condition
affected lot/serials
risk assessment
allowed use
prohibited use
additional tests
expiration or quantity limit
approver
closure evidence
```

A temporary concession must not silently become the new baseline.

## 47. Firmware-profile compatibility

Each approved hardware profile should map to:

- pin definitions;
- display driver/profile;
- touch settings;
- Flash configuration;
- partition table;
- enabled features;
- known conflicts;
- minimum diagnostic version;
- calibration requirements.

Firmware should report the selected profile and reject incompatible configurations where practical.

## 48. Mechanical-profile compatibility

Control:

- board outline;
- mounting holes;
- display active area;
- touch area;
- USB and microSD access;
- connector clearances;
- button positions;
- antenna keep-out;
- speaker placement;
- enclosure pressure points.

A mechanically different lot may require enclosure requalification even if firmware works.

## 49. RF-profile compatibility

Changes near the antenna, module, enclosure, cables, or ground geometry may affect Wi-Fi and Bluetooth.

Recheck:

- module identity;
- antenna orientation;
- metal clearance;
- cable routing;
- enclosure state;
- controlled RSSI/throughput baseline;
- reconnect behavior.

Do not infer regulatory equivalence for a finished product solely from a module marking.

## 50. Thermal-profile compatibility

Requalify when there is a change in:

- regulator type;
- regulator package or copper area;
- input voltage;
- TFT backlight current;
- Wi-Fi workload;
- audio amplifier;
- enclosure ventilation;
- external load;
- ambient requirement.

## 51. Security and provenance

Procurement affects security through:

- unknown factory firmware;
- altered Flash contents;
- undocumented wireless configuration;
- suspect USB bridge;
- changed modules;
- preloaded credentials or files;
- untrusted microSD content.

For controlled deployment:

- preserve evidence from representative units;
- erase/reflash through an approved process;
- install unique credentials;
- verify firmware hashes;
- avoid shared factory secrets;
- record provisioning.

## 52. Provisioning control

Record:

- asset ID;
- hardware profile;
- firmware image and hash;
- partition table;
- configuration schema;
- calibration;
- generated credentials;
- provisioning station;
- operator;
- timestamp;
- verification result.

Secrets should not appear in ordinary manufacturing logs.

## 53. Production-test boundary

Incoming inspection, board acceptance, provisioning, and final-product test are different gates.

```text
incoming board accepted
-> firmware provisioned
-> assembly completed
-> final product tested
-> release authorized
```

Passing one gate does not imply the later gates passed.

## 54. Lot acceptance decision

Possible dispositions:

- ACCEPTED;
- ACCEPTED_WITH_DEVIATION;
- PARTIAL_ACCEPTANCE;
- EVALUATION_ONLY;
- REWORK_REQUIRED;
- RETURN_TO_SUPPLIER;
- SCRAP;
- HOLD_PENDING_EVIDENCE.

The disposition should state allowed applications.

## 55. Incoming-lot report template

```text
Report ID:
Lot ID:
Supplier/store:
Order ID:
Listing snapshot:
Order date:
Receipt date:
Requested model/revision:
Received quantity:
Packaging condition:
Inspector:

Visible PCB markings:
Observed hardware-profile candidate:
Component markings:
Mechanical observations:
Connector observations:
Factory firmware preserved:
Program-Flash identity:
Auxiliary-Flash evidence:
Sample plan:
Tests executed:
Failures/deviations:
Photograph set:
Comparison to golden sample:
Qualification-inheritance level:
Disposition:
Approver:
Evidence location/hash:
```

## 56. Revision-comparison report template

```text
Comparison ID:
Baseline profile:
Candidate profile:
Baseline lot/sample:
Candidate lot/sample:

PCB marking difference:
Population difference:
Component-marking difference:
Pin-map difference:
Power difference:
Display/touch difference:
Storage difference:
USB/UART difference:
Audio difference:
Mechanical difference:
RF difference:
Thermal difference:
Factory firmware difference:

Difference classes:
Risk assessment:
Required regression:
Results:
Inheritance decision:
New internal profile ID:
Documentation updates:
Approver/date:
```

## 57. Approved-item record template

```text
Item class:
Internal item ID:
Manufacturer:
Manufacturer part number:
Supplier:
Supplier article number:
Description:
Critical dimensions/specifications:
Approved use:
Prohibited use:
Qualification evidence:
Reference sample location:
Inspection method:
Substitution rules:
Approval date:
Review date:
Status:
```

## 58. Nonconformance report template

```text
NCR ID:
Date:
Lot/asset IDs:
Supplier/order:
Requirement:
Observed nonconformance:
Detection stage:
Quantity affected:
Evidence/photos:
Immediate containment:
Safety impact:
Functional impact:
Downstream exposure:
Supplier notification:
Root cause:
Disposition:
Additional tests:
Approval:
Closure evidence:
```

## 59. Procurement acceptance criteria

A controlled lot is acceptable only when:

1. supplier and order evidence are retained;
2. lot identity is assigned;
3. packaging and visual inspection are complete;
4. representative photographs are retained;
5. internal hardware profile is assigned or explicitly open;
6. critical component and connector differences are assessed;
7. sample plan is recorded;
8. required electrical and functional tests pass;
9. deviations are approved and scoped;
10. qualification inheritance is explicit;
11. boards are traceable to lot and acceptance results;
12. approved firmware profile exists for intended use;
13. unresolved D7 differences do not enter uncontrolled deployment;
14. nonconforming units are segregated;
15. disposition is authorized.

## 60. Evidence levels

Use explicit procurement evidence levels:

- **Listing-observed** — seller claim only;
- **Received-observed** — visible on received material;
- **Sample-measured** — measured on one sample;
- **Lot-sampled** — verified using a declared sample plan;
- **Lot-100%-tested** — every unit passed the stated test;
- **Supplier-traceable** — manufacturer/distributor provenance available;
- **Revision-qualified** — candidate revision passed declared regression;
- **Field-supported** — deployed history exists;
- **Open** — insufficient evidence.

## 61. Claims that must not be made yet

Do not claim without evidence:

- that all `ESP32-2432S028` and `ESP32-2432S028R` boards are equivalent;
- that seller photographs identify the shipped revision;
- that CH340G and CH340C variants are fully interchangeable in every process;
- that the TFT controller is always ILI9341;
- that the connector is genuine Molex PicoBlade;
- that every W25Q32-marked device is authentic;
- that two visually identical lots share the same netlist;
- that previous qualification automatically covers a new supplier;
- that one sampled board proves the whole lot;
- that module certification alone certifies the final product;
- that marketplace sourcing provides formal PCN control;
- that a lower price proves counterfeit material;
- that a successful boot proves acceptable workmanship or reliability.

## 62. Completion criteria

The manufacturing and procurement system can be considered operational when:

1. procurement specifications are version-controlled;
2. incoming lots receive unique IDs;
3. boards remain segregated until disposition;
4. seller evidence and listing snapshots are retained;
5. a hardware-profile fingerprint is defined;
6. golden comparison samples exist;
7. revision-difference classes are used;
8. approved-item lists exist for critical boards, cables, connectors, cards, speakers, and supplies;
9. sample plans are declared before inspection;
10. incoming visual and functional checklists are versioned;
11. representative factory firmware is preserved;
12. test firmware and fixtures are traceable;
13. qualification inheritance is explicitly approved;
14. requalification triggers are enforced;
15. nonconforming material is quarantined;
16. deviations are scoped and time/quantity limited;
17. provisioning records include firmware hashes;
18. security-sensitive factory contents and credentials are controlled;
19. lot, board, firmware, maintenance, and deployment records are linked;
20. unresolved differences remain visible.

## 63. Open implementation tasks

- Define the first internal hardware-profile ID for the photographed sample.
- Create a structured lot and asset registry.
- Archive seller-listing evidence for future purchases.
- Build a photo-comparison checklist.
- Measure the board outline and connector geometry for procurement control.
- Confirm the exact connector family and mating parts.
- Preserve factory firmware from representative future lots.
- Add automated hardware-profile reporting to diagnostic firmware.
- Define lot sampling rules for small and larger purchases.
- Define numerical rail, thermal, and functional acceptance limits from measured golden boards.
- Create approved USB cable, microSD, speaker, and power-supply records.
- Create an incoming-test fixture and version its wiring.
- Define a revision-comparison report in the repository.
- Test how CH340G/CH340C and TFT-controller substitutions affect the workflow.
- Establish supplier-performance metrics after multiple lots.

## 64. Summary

Procurement control for the ESP32-2432S028 family must be based on retained evidence, internal hardware profiles, lot segregation, controlled inspection, and explicit qualification inheritance.

The governing rule is:

> Same product name is not the same as same controlled configuration.

A board becomes approved only after its supplier evidence, received identity, revision differences, measured behavior, intended use, and acceptance disposition are linked in a traceable record.