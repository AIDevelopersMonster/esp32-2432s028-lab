# Mechanical Integration and Enclosure System — ESP32-2432S028

> **Document status:** sample-specific mechanical-integration reference for the author's `ESP32-2432S028` board.  
> **Scope:** dimensional control, mounting, display and touch-window integration, connector and button access, antenna clearance, speaker and cable placement, thermal paths, insulation, serviceability, environmental protection, enclosure materials, manufacturing tolerances, diagnostics, and verification.  
> **Evidence rule:** no nominal board, hole, display-window or component-height dimension is treated as verified until measured on the documented sample or supported by a controlled drawing.

![ESP32-2432S028 board overview](../../hardware/images/board-overview.jpg)

## 1. Purpose

The board can operate correctly on a bench and still fail after installation because mechanical integration changes:

- connector accessibility;
- cable stress;
- touch sensitivity;
- display visibility;
- antenna performance;
- regulator temperature;
- speaker acoustics;
- button travel;
- microSD access;
- electrical clearances;
- exposure to dust, moisture, vibration and static discharge.

An enclosure is therefore part of the electrical and RF system, not merely a cosmetic shell.

This chapter defines a controlled process for creating a usable mechanical model without copying unverified seller dimensions or generic CYD drawings.

## 2. Evidence summary

| Item | Current status | Evidence | Still open |
|---|---|---|---|
| Board outline and display assembly | Physically visible | author-supplied photographs | measured X/Y outline and corner geometry |
| Mounting holes | Visible on sample | board photographs | diameter, center coordinates, plating and allowed fastener size |
| TFT active area | Functionally evident | display operation and physical assembly | exact active-area dimensions and location |
| Touch overlay | Physically present | resistive touch panel | bezel clearance and force sensitivity |
| USB, P1, P3, CN1, P4 and microSD locations | Photo-confirmed | PCB labels and connector photographs | full 3D envelopes and mating-cable bend radii |
| BOOT and RESET buttons | Photo-confirmed | board markings and photographs | actuator height, travel and external plunger dimensions |
| ESP-WROOM-32 antenna end | Photo-confirmed | module marking and geometry | final enclosure keep-out and RF qualification |
| Dual AMS1117 regulators | Photo-confirmed | package markings | enclosure thermal rise and airflow requirement |
| Speaker output | Function confirmed | 8002A and P4 | speaker mounting, acoustic volume and cable route |
| Exact board thickness and maximum assembly height | Open | not yet measured | complete dimensional survey |
| Environmental rating | Open | no sealed enclosure tested | dust, splash, condensation and corrosion qualification |

## 3. Mechanical datum system

A dimensional drawing must use a declared coordinate system.

Recommended convention:

```text
Datum A: PCB component-side plane
Datum B: left board edge in the selected top-view orientation
Datum C: lower board edge in the selected top-view orientation
Origin: intersection of projected B and C edges
X axis: left to right
Y axis: bottom to top
Z axis: normal to the PCB, positive toward the component side
```

Because the display occupies the opposite side, a second view should define negative Z or a separate display-side datum.

Do not use phrases such as “top-left hole” without attaching them to a controlled drawing orientation.

## 4. Required dimensional deliverables

The enclosure workflow should produce:

1. a 2D PCB-outline drawing;
2. a hole-coordinate table;
3. a component-side height map;
4. a display-side height map;
5. connector mating-envelope drawings;
6. a display active-area and touch-area drawing;
7. button-actuation geometry;
8. antenna keep-out volume;
9. thermal keep-out and ventilation notes;
10. an assembly stack-up;
11. a 3D model or simplified collision model;
12. a tolerance and revision record.

A screenshot of a seller drawing is not a controlled mechanical deliverable.

## 5. Dimensional survey procedure

### 5.1 Measurement tools

Use tools appropriate to the required tolerance:

- digital caliper for general board dimensions;
- pin gauges or drill shanks for hole checks where suitable;
- depth gauge for height stack-up;
- optical comparator or calibrated macro image for small offsets;
- flat reference plate;
- feeler gauges;
- coordinate-measuring equipment if production tolerance demands it.

Record tool model, calibration status and measurement resolution.

### 5.2 Sample conditioning

Before measurement:

- remove loose cables and cards;
- avoid pressing on the display;
- place the board on a flat nonconductive support;
- allow the board to reach room temperature;
- inspect for warped PCB or bent connectors;
- identify whether protective display film is installed;
- photograph the exact measured sample.

### 5.3 Repeatability

Measure critical dimensions at least three times, ideally by two operators for the first controlled drawing.

For each value record:

```text
measurement 1
measurement 2
measurement 3
mean
range
selected design value
measurement uncertainty
```

## 6. Board-outline measurement

Record:

```text
overall PCB width X
overall PCB height Y
PCB thickness
corner radius or chamfer geometry
edge cut-outs
protrusions beyond nominal rectangle
maximum connector overhang
maximum display overhang
```

Do not infer the PCB outline from display size or model name.

### 6.1 Edge irregularities

Inspect each edge for:

- USB connector shell overhang;
- microSD socket overhang;
- connector bodies extending beyond the board;
- display flex or frame projection;
- castellated module or antenna position;
- manufacturing tabs or remnants;
- solder fillets that affect fit.

### 6.2 Enclosure allowance

The enclosure cavity should not equal the measured board outline exactly.

Allowance must account for:

- PCB manufacturing tolerance;
- enclosure manufacturing tolerance;
- thermal expansion;
- coating or paint;
- assembly angle;
- cable insertion force;
- board replacement without damage.

The final allowance is a design decision and must be documented rather than copied from a generic rule.

## 7. Mounting-hole survey

For each mounting hole record:

| Hole ID | X center | Y center | Measured diameter | Plated/unplated | Nearby copper/component | Proposed fastener | Status |
|---|---:|---:|---:|---|---|---|---|
| H1 | | | | | | | open |
| H2 | | | | | | | open |
| H3 | | | | | | | open |
| H4 | | | | | | | open |

Do not assume all holes have identical diameter or symmetric placement.

### 7.1 Hole-center measurement

Possible methods:

- measure edge-to-near-side and add half the measured diameter;
- use fitted gauge pins and measure pin centers;
- use calibrated top-view photography;
- use a coordinate measuring machine.

### 7.2 Plating and electrical status

Determine whether each hole is:

- non-plated;
- plated but isolated;
- connected to ground;
- surrounded by exposed copper;
- close to signal traces or components.

Do not use a metal standoff as an electrical ground connection unless continuity and current capability are intentionally verified.

## 8. Fastener selection

### 8.1 Fastener diameter

Select fastener size from the measured hole and required clearance.

A fastener that enters the hole does not automatically have adequate:

- radial clearance;
- washer clearance;
- head clearance;
- tool access;
- electrical isolation;
- vibration resistance.

### 8.2 Washers and insulation

Use insulating washers or shoulder bushings when needed to prevent:

- copper damage;
- solder-mask wear;
- accidental grounding;
- excessive local pressure;
- contact with nearby components.

### 8.3 Tightening torque

Excess torque can:

- crack the PCB;
- distort the display;
- damage resistive touch alignment;
- strip plastic bosses;
- crush spacers;
- change button or connector alignment.

Torque must be controlled for production assemblies.

### 8.4 Thread locking

Thread-locking compounds may attack plastics or contaminate electronics. Use only a material verified for the selected enclosure and assembly process.

## 9. Standoff height and board stack-up

The standoff height must clear the tallest component or solder joint on the supported side.

Record:

```text
PCB thickness
tallest component-side height
tallest display-side height
solder-joint projection
connector shell projection
button-body height
display frame height
required airflow gap
assembly tolerance
```

### 9.1 Support plane

Do not allow the board to rest directly on:

- solder joints;
- USB shell tabs;
- regulator packages;
- connector anchors;
- the display frame;
- the touch flex;
- the antenna region.

### 9.2 Board flex

A large unsupported span can flex during:

- touch operation;
- USB insertion;
- microSD insertion;
- button pressing;
- cable pulling.

Support should resist these loads without creating pressure points under sensitive components.

## 10. Display opening

The front-panel opening must be based on measured display geometry.

Record separately:

- module outer frame;
- visible glass area;
- active pixel area;
- touch-sensitive area;
- black mask area;
- flex-tail location;
- display offset relative to PCB datums.

### 10.1 Active area versus visible opening

A front opening can:

- expose only the active pixels;
- expose the active area plus black border;
- expose the full glass.

The choice affects aesthetics, touch access, sealing and assembly tolerance.

### 10.2 Bezel overlap

Excessive overlap can:

- hide pixels;
- obstruct touch near the edge;
- press on the resistive overlay;
- create false touch or calibration error;
- concentrate stress in the glass.

Insufficient overlap can expose adhesive, flex or unsightly gaps.

### 10.3 Corner geometry

Measure whether the visible glass and active area have square or rounded corners. Do not use a generic screen-cutout radius.

## 11. Resistive-touch integration

### 11.1 No preload on active surface

The bezel, gasket or protective window must not apply uncontrolled pressure to the active touch area.

Constant pressure can cause:

- false touches;
- reduced sensitivity;
- coordinate drift;
- permanent deformation;
- visible Newton-ring-like artifacts;
- premature wear.

### 11.2 Edge clearance

Provide clearance around the touch active area and flex transition.

### 11.3 Protective overlay

Any added protective sheet changes touch force and optical behavior.

Test:

- minimum reliable touch force;
- stylus and finger use;
- edge sensitivity;
- coordinate calibration;
- glare;
- color and contrast;
- surface durability.

### 11.4 Front-window air gap

A separate rigid front window may require a controlled gap so it does not transfer pressure to the resistive overlay.

### 11.5 Gasket design

A gasket should contact only a verified safe border area. Record:

- material;
- thickness;
- compression percentage;
- compression set;
- adhesive;
- chemical compatibility;
- temperature range.

## 12. Display optical integration

Evaluate:

- viewing angle;
- bezel shadow;
- reflected ambient light;
- protective-window transmission;
- color shift;
- internal reflections;
- condensation;
- fingerprint visibility;
- display brightness needed in the intended environment.

A dark-tinted window may improve appearance while requiring higher backlight power and increasing thermal load.

## 13. USB access

The USB opening must support:

- plug shell insertion;
- molded cable-body clearance;
- cable bend radius;
- connector-shell movement tolerance;
- tool-free connection;
- strain relief;
- service replacement.

### 13.1 Opening size

Do not dimension only for the metal plug. Cable overmolds vary substantially.

Test at least:

- intended production cable;
- common straight cable;
- larger molded cable;
- right-angle cable if considered.

### 13.2 Insertion load

The enclosure should absorb or redirect cable load so repeated USB insertion does not flex the PCB excessively.

### 13.3 Connector alignment

Misalignment can force the plug against the board connector and damage solder joints.

## 14. P1, P3, CN1 and P4 access

![Connector area](../../hardware/images/connectors.jpg)

For each connector define:

- board position;
- mating direction;
- housing envelope;
- latch or friction-feature clearance;
- finger or tool access;
- wire exit direction;
- cable bend radius;
- strain-relief point;
- service removal path.

### 14.1 Cable colors are not geometry

Do not design an opening from a photograph of one colored cable. Use the actual housing and crimp assembly.

### 14.2 Cable insertion force

Small 1.25-mm-class connectors can impose significant local force relative to board size. Support the PCB near the connector during service.

### 14.3 Cable removal

Provide a way to grip the housing, not the wires.

### 14.4 P4 speaker cable

The speaker cable carries a differential BTL output. Keep it mechanically isolated from sharp edges and away from the antenna region.

## 15. microSD access

The enclosure design must decide whether the card is:

- externally user-accessible;
- accessible only during service;
- permanently installed internally.

### 15.1 Insertion and removal envelope

Provide clearance for:

- card alignment;
- finger or tool access;
- socket push-push or friction mechanism as fitted;
- card projection during insertion;
- card ejection travel.

### 15.2 Card retention

Vibration can affect exposed cards. Do not add a retaining feature that presses on the socket or prevents normal ejection.

### 15.3 Dust path

An external card slot is also an ingress path. Sealing and serviceability requirements must be balanced.

## 16. BOOT and RESET access

### 16.1 Direct access

A service opening may allow a small tool to press the tactile switch directly.

### 16.2 External plungers

An enclosure plunger must be designed for:

- switch location tolerance;
- button height;
- required force;
- overtravel limitation;
- lateral guidance;
- no constant preload;
- clear distinction between BOOT and RESET.

### 16.3 Accidental activation

RESET should not be easily pressed during ordinary handling unless intentional.

BOOT access can be service-only if automatic programming is reliable.

### 16.4 Labeling

Labels should match actual function and orientation.

## 17. RGB LED and LDR optical paths

### 17.1 RGB LED

If the status LED must remain visible, use:

- a direct opening;
- a light pipe;
- a translucent window;
- an internal diffuser.

Test color mixing and brightness through the final material.

### 17.2 Light pipe coupling

A poorly positioned light pipe can load the LED package or leak light into the LDR.

### 17.3 LDR opening

The LDR requires a controlled view of ambient light.

The opening should avoid:

- direct illumination from the RGB LED;
- backlight leakage;
- shadows from bezel geometry;
- dust accumulation;
- water path into the enclosure.

### 17.4 Calibration after assembly

Any LDR calibration must be repeated with the final enclosure because window material and geometry change the optical response.

## 18. Antenna keep-out volume

![ESP-WROOM-32 antenna area](../../hardware/images/esp-wroom-32.jpg)

The antenna end requires a three-dimensional keep-out, not only a two-dimensional board-edge note.

Keep clear where possible:

- metal walls;
- metal standoffs;
- speaker magnets and frames;
- batteries;
- cable bundles;
- ground planes on adjacent boards;
- conductive coatings;
- foil labels;
- heat sinks;
- user-added shielding.

### 18.1 Mechanical versus RF compromise

A mechanically convenient bracket near the antenna can reduce range. The final placement must be RF-tested, not accepted from CAD clearance alone.

### 18.2 Plastic wall distance

Even plastic can detune the antenna. Record wall material, thickness and distance.

### 18.3 Production consistency

Cable routing and battery position near the antenna must be fixed by clips or channels if RF performance depends on them.

## 19. Speaker integration

### 19.1 Speaker type

Use the electrically verified speaker load from the audio chapter.

Mechanical design must additionally define:

- diameter and depth;
- mounting method;
- cone excursion clearance;
- frame material;
- magnet location;
- acoustic vent;
- rear volume;
- gasket;
- cable route.

### 19.2 Acoustic opening

The grille or opening affects volume and frequency response.

### 19.3 Enclosure cavity

An uncontrolled cavity can create resonance, rattling or muffled output.

### 19.4 Vibration isolation

Speaker vibration can loosen connectors or create touch artifacts if mechanically coupled to the display assembly.

### 19.5 Antenna separation

Keep the speaker magnet, frame and cable away from the antenna region where possible.

## 20. Cable routing

Define routes for:

- USB cable;
- P1 serial cable;
- P3 and CN1 expansion cables;
- speaker cable;
- external sensors;
- power wires;
- any future battery leads.

### 20.1 Routing rules

Cables should not:

- cross the antenna keep-out without qualification;
- press on the display or touch flex;
- contact regulator packages;
- obstruct buttons or microSD;
- rub against sharp PCB edges;
- carry connector load directly into solder joints;
- obstruct ventilation.

### 20.2 Strain relief

Use enclosure-level strain relief for external cables.

### 20.3 Bend radius

Respect the cable and housing bend radius. A cable forced immediately through a sharp 90-degree turn can pull the connector sideways.

## 21. Thermal integration

The board contains two AMS1117-3.3 regulators and multiple dynamic loads.

An enclosure can raise temperature by reducing convection and trapping heat.

### 21.1 Heat sources

Potential heat sources include:

- AMS1117 regulators;
- ESP32 during radio activity;
- TFT backlight path;
- audio amplifier;
- microSD card;
- external loads powered from the board.

### 21.2 Measurement points

Record temperature at:

- U1 regulator package;
- U7 regulator package;
- ESP-WROOM-32 shield;
- display rear surface;
- audio amplifier;
- enclosure internal air;
- enclosure external surface;
- ambient air.

### 21.3 Test conditions

Use worst declared combinations:

- maximum backlight;
- sustained Wi-Fi traffic;
- microSD writing;
- audio output;
- RGB LED active;
- external 3.3-V load;
- highest ambient temperature;
- closed enclosure.

### 21.4 Ventilation

Ventilation openings can improve cooling but reduce ingress protection.

Do not place vents where dust or water falls directly onto:

- USB connector;
- microSD socket;
- display flex;
- high-impedance analog nodes;
- exposed contacts.

### 21.5 Heat spreading

Do not attach a metal heat spreader near the antenna without RF testing.

## 22. Airflow and convection

Natural convection depends on orientation.

Test the enclosure:

- upright;
- flat display-up;
- flat display-down if allowed;
- wall-mounted;
- in the intended recess or cabinet.

A vent layout effective on the bench may be ineffective after wall mounting.

## 23. Electrical insulation

The enclosure must prevent conductive objects from contacting:

- PCB pads;
- USB shell and nearby circuitry;
- regulator tabs;
- connector pins;
- solder joints;
- speaker outputs;
- expansion GPIOs.

### 23.1 Metal enclosure

A metal enclosure requires controlled insulation, grounding strategy and edge clearance.

### 23.2 Conductive debris

Avoid trapped screws, metal shavings and clipped wire strands.

### 23.3 Adhesive insulation

Adhesive films must tolerate temperature and avoid peeling into connectors or vents.

## 24. ESD access points

Likely user-accessible ESD paths include:

- display surface;
- touch bezel;
- USB connector;
- microSD card;
- external connectors;
- buttons;
- exposed screw heads;
- speaker grille.

Mechanical design should avoid routing discharge directly toward sensitive board regions.

Formal ESD performance requires a separate controlled test plan.

## 25. Enclosure materials

### 25.1 3D-printed plastics

Potential issues:

- dimensional shrinkage;
- warping;
- layer-direction weakness;
- heat deformation;
- moisture absorption;
- inconsistent surface finish;
- limited thread durability;
- flammability uncertainty.

### 25.2 Injection-molded plastics

Consider:

- draft angles;
- wall thickness;
- ribs;
- sink marks;
- boss design;
- tooling tolerance;
- material shrinkage;
- UL or other material ratings where required.

### 25.3 Machined plastics

Consider stress cracking, tool marks and thickness tolerance.

### 25.4 Metal

Metal improves stiffness and shielding but creates antenna, insulation and touch-access complications.

### 25.5 Transparent window materials

Evaluate:

- optical clarity;
- scratch resistance;
- impact resistance;
- chemical compatibility;
- UV aging;
- touch-force effect;
- adhesive bonding.

## 26. 3D-print prototype workflow

1. create a simplified board collision model from measured datums;
2. print an open-frame fit gauge first;
3. verify holes, USB, microSD and display opening;
4. test connector insertion with real cables;
5. test button plungers;
6. test antenna clearance;
7. test thermal behavior in a partially closed shell;
8. revise before printing the final cosmetic enclosure.

A full decorative print should not be the first dimensional test.

## 27. CAD model discipline

The mechanical model should contain named bodies or envelopes for:

- PCB;
- display frame;
- touch glass;
- active display area;
- mounting holes;
- USB mating envelope;
- microSD insertion envelope;
- P1/P3/CN1/P4 mating envelopes;
- BOOT and RESET actuator volumes;
- antenna keep-out;
- component-side maximum height;
- thermal keep-out;
- cable routes.

### 27.1 Model simplification

Small passive components need not all be modeled individually if a verified height envelope is used.

### 27.2 Revision metadata

Embed:

```text
board sample ID
PCB marking
measurement date
model revision
source photographs
drawing units
critical assumptions
open dimensions
```

## 28. Tolerance stack-up

The design must combine:

- PCB outline tolerance;
- hole-position tolerance;
- display placement tolerance;
- connector placement tolerance;
- enclosure feature tolerance;
- standoff tolerance;
- gasket compression;
- assembly shift;
- print or mold shrinkage;
- cable housing variation.

### 28.1 Worst-case check

For each critical interface calculate or physically test the worst case:

```text
minimum clearance
maximum clearance
maximum misalignment
minimum engagement
maximum preload
```

### 28.2 Cosmetic versus functional gaps

A cosmetic gap may be small while a functional connector opening requires more margin.

## 29. Assembly sequence

Define the intended sequence, for example:

1. install display gasket or window;
2. install board standoffs;
3. route internal cables;
4. connect speaker and expansion cables;
5. place board without twisting cables;
6. install fasteners in controlled order;
7. verify touch surface is unloaded;
8. close enclosure;
9. perform electrical self-test;
10. verify RF and thermal behavior.

A design that can only be assembled by bending the PCB is unacceptable.

## 30. Serviceability

Decide which operations are field-serviceable:

- USB programming;
- BOOT/RESET access;
- microSD replacement;
- speaker replacement;
- board replacement;
- cable replacement;
- cleaning;
- firmware recovery.

### 30.1 Fastener choice

Service fasteners should survive the expected number of cycles.

### 30.2 Captive hardware

Captive screws can reduce lost hardware but require compatible boss design.

### 30.3 Connector labeling

Internal connectors should be labeled so they are not reversed during service.

### 30.4 Tamper control

If access must be restricted, define seals or security fasteners without blocking necessary thermal or RF features.

## 31. Environmental protection

### 31.1 Dust

Dust can accumulate on:

- display edges;
- touch overlay;
- connectors;
- LDR opening;
- regulator surfaces;
- microSD socket;
- speaker grille.

### 31.2 Splash and condensation

The bare board should not be assumed splash-resistant.

Condensation can form even without direct liquid entry when temperature changes.

### 31.3 Conformal coating

Conformal coating may affect:

- connectors;
- buttons;
- antenna;
- display flex;
- LDR response;
- serviceability;
- heat transfer.

Do not coat the antenna or connector contacts without a controlled process.

### 31.4 Corrosion

High humidity and contaminants can corrode exposed metal. Material and gasket selection should consider the deployment environment.

## 32. Cleaning and chemical compatibility

Define approved cleaning agents for:

- display surface;
- touch overlay;
- enclosure;
- transparent window;
- labels;
- seals.

Avoid solvents that attack:

- polarizer films;
- adhesives;
- 3D-printed plastics;
- cable insulation;
- silkscreen;
- gasket material.

## 33. Vibration and shock

Potential failure points include:

- heavy connectors;
- display attachment;
- speaker;
- microSD card;
- USB cable;
- standoffs;
- solder joints;
- unsupported cable masses.

### 33.1 Basic vibration test

Record:

- mounting orientation;
- input profile;
- duration;
- powered or unpowered state;
- cable configuration;
- pre/post electrical test;
- visible damage;
- connector loosening;
- touch-calibration change.

### 33.2 Drop testing

A drop test must define:

- enclosure mass;
- drop height;
- surface;
- orientations;
- number of drops;
- pass criteria.

Do not perform uncontrolled drop testing with an exposed powered board.

## 34. Cable pull and connector retention

For each external cable define a pull test that loads the enclosure strain relief rather than the PCB connector.

Record:

```text
cable type
pull direction
force
hold time
permanent displacement
connector damage
intermittent electrical failure
result
```

## 35. Button life and alignment test

Test external plungers across:

- enclosure tolerance;
- board-position tolerance;
- temperature range;
- repeated operation;
- side loading.

A plunger must not jam or hold the switch permanently pressed.

## 36. Display and touch endurance

Check:

- repeated touches near center and corners;
- stylus pressure;
- bezel pressure;
- enclosure twist;
- temperature cycling;
- protective-window scratching;
- calibration retention.

## 37. Thermal-cycle fit

Different materials expand differently.

Inspect after temperature cycling for:

- bezel pressure on touch glass;
- loosened fasteners;
- cracked bosses;
- cable tension;
- display-window gaps;
- gasket extrusion;
- connector misalignment.

## 38. Moisture and condensation test plan

When relevant to the intended product, define a safe environmental test with:

- humidity range;
- temperature profile;
- powered/unpowered periods;
- condensation observation;
- insulation resistance where appropriate;
- post-test corrosion inspection;
- recovery time.

The exact procedure depends on product requirements and applicable standards.

## 39. Accessibility and usability

Mechanical integration should consider:

- touch targets near bezel edges;
- display readability;
- cable insertion by intended users;
- button labeling;
- card orientation;
- sharp edges;
- finger pinch points;
- wall-mount access;
- visual status indication.

## 40. Wall, panel and desktop mounting

### 40.1 Wall mounting

Check:

- cable exit;
- convection direction;
- access to USB and microSD;
- antenna orientation;
- screw-head clearance;
- wall material near antenna.

### 40.2 Panel mounting

Check:

- front-panel thickness;
- display opening tolerance;
- gasket compression;
- rear service space;
- cable bend radius;
- fastener access.

### 40.3 Desktop enclosure

Check:

- viewing angle;
- anti-slip feet;
- stability during touch;
- cable tipping force;
- speaker opening;
- ventilation underneath.

## 41. Mechanical interaction with board subsystems

| Mechanical feature | Electrical/RF subsystem affected | Failure mechanism |
|---|---|---|
| tight bezel | touch/display | false touch, hidden pixels, glass stress |
| metal rear plate | antenna | detuning and attenuation |
| low standoff | power/logic | solder-joint short or pressure |
| blocked vent | power/audio/Wi-Fi | thermal rise and reset |
| cable over antenna | Wi-Fi/Bluetooth | pattern distortion |
| speaker near antenna | wireless/audio | RF degradation and vibration |
| USB opening misalignment | programming/power | connector fatigue |
| unsupported board | touch/connectors | PCB flex and solder damage |
| blocked microSD slot | storage | impossible service or forced card insertion |
| preloaded button plunger | BOOT/RESET | bootloader lock or reset loop |
| sealed LDR | sensor | incorrect ambient-light response |

## 42. Prototype inspection checklist

Before power-up:

- board enters without force;
- no solder joint contacts enclosure;
- all standoffs contact intended areas;
- fasteners do not touch copper or components;
- display and touch surface have no preload;
- connectors are accessible;
- cables are not pinched;
- microSD travel is clear;
- buttons move freely;
- antenna volume is clear;
- speaker wiring is correct and insulated;
- no loose metal debris remains.

After power-up:

- display operates normally;
- touch calibration remains valid;
- BOOT and RESET work;
- USB upload works;
- microSD works;
- Wi-Fi performance is compared to bare-board baseline;
- temperatures are measured;
- audio is free of rattles;
- LDR response is checked;
- no cable or enclosure region becomes hot.

## 43. Mechanical diagnostic cases

### 43.1 Display works outside enclosure but not inside

Check:

- PCB flex;
- display pressure;
- cable or fastener contact;
- power short;
- backlight obstruction;
- connector strain.

### 43.2 Touch becomes inaccurate after assembly

Check:

- bezel preload;
- protective-window contact;
- gasket compression;
- board flex;
- changed rotation or calibration;
- speaker vibration.

### 43.3 Wi-Fi range falls after assembly

Check:

- antenna wall clearance;
- metal hardware;
- speaker or battery position;
- cable routing;
- conductive coating;
- wall mounting against metal.

### 43.4 Random resets after enclosure closure

Check:

- regulator temperature;
- pinched wires;
- conductive contact;
- button plunger preload;
- USB power drop;
- speaker or external-load current.

### 43.5 USB connection is intermittent

Check:

- opening alignment;
- cable overmold collision;
- connector shell movement;
- PCB flex;
- strain relief;
- cable bend radius.

### 43.6 microSD cannot be removed

Check:

- card ejection travel;
- front or side wall interference;
- card orientation;
- tool access;
- socket damage caused by forced insertion.

### 43.7 Audio rattles

Check:

- speaker mounting;
- grille clearance;
- loose cable;
- enclosure resonance;
- fastener torque;
- display or PCB vibration.

### 43.8 LDR reading changes unexpectedly

Check:

- optical window transmission;
- RGB or backlight leakage;
- shadowing;
- dust;
- enclosure color and internal reflections.

## 44. Mechanical verification matrix

| Test | Prototype state | Primary measurement | Pass condition |
|---|---|---|---|
| PCB fit | open frame | insertion force and clearance | no board bending or interference |
| hole alignment | open frame | fastener engagement | all fasteners enter without forcing |
| display opening | front bezel | visible active area | no hidden required pixels |
| touch edge test | assembled | raw and calibrated coordinates | no false touch or dead edge |
| USB access | assembled | repeated plug cycles | no shell/PCB strain |
| microSD access | assembled | insertion/ejection cycles | reliable user or service access |
| connector access | assembled | mating/removal | housing can be gripped safely |
| button alignment | assembled | actuation force/travel | no preload or jamming |
| antenna comparison | assembled | RSSI/loss/throughput | within declared degradation limit |
| thermal stress | closed | component temperatures | within declared limits |
| speaker test | closed | audible artifacts | no rattle or enclosure contact |
| cable pull | assembled | retention | no PCB-connector loading failure |
| vibration | assembled | electrical continuity | no loosening or intermittent fault |
| service cycle | assembled | repeated opening | no stripped bosses or damaged cables |

## 45. Dimensional record template

```text
Board sample ID:
PCB marking/revision:
Photograph set:
Measurement date:
Operator:
Measurement tools:
Units:
Datum definition:

PCB width:
PCB height:
PCB thickness:
Corner geometry:
Maximum component-side height:
Maximum display-side height:

H1 center X/Y and diameter:
H2 center X/Y and diameter:
H3 center X/Y and diameter:
H4 center X/Y and diameter:
Hole plating/ground status:

Display frame X/Y:
Visible glass X/Y:
Active area X/Y:
Active-area offset:
Touch area X/Y:
Touch-area offset:
Safe bezel border:

USB mating envelope:
P1 mating envelope:
P3 mating envelope:
CN1 mating envelope:
P4 mating envelope:
microSD insertion/ejection envelope:
BOOT actuator location/height/travel:
RESET actuator location/height/travel:

Antenna keep-out volume:
Thermal keep-out:
Cable routes:
Speaker envelope:

Critical tolerances:
Open dimensions:
CAD revision:
Drawing revision:
Conclusion:
```

## 46. Enclosure test record template

```text
Enclosure revision:
Material/process:
Wall thickness:
Window material:
Gasket material/compression:
Fasteners and torque:
Standoffs:
Assembly date:

Board fit:
Display opening:
Touch result:
USB result:
microSD result:
Connector result:
Button result:
LDR/RGB result:
Speaker result:
Cable strain relief:

Bare-board Wi-Fi baseline:
Enclosed Wi-Fi result:
RF degradation:

Ambient temperature:
U1 temperature:
U7 temperature:
ESP32 shield temperature:
Audio amplifier temperature:
Internal-air temperature:

Vibration/drop/service tests:
Observed failures:
Corrective actions:
Result:
```

## 47. Evidence levels for future updates

Use explicit labels:

- **Photo-confirmed** — feature visible on the documented board;
- **Measured** — dimension recorded with tool and uncertainty;
- **CAD-controlled** — included in a versioned model or drawing;
- **Fit-verified** — physically assembled without interference;
- **Function-verified** — subsystem operates in the enclosure;
- **Thermally-qualified** — passed declared load and ambient tests;
- **RF-qualified** — enclosure antenna effect measured;
- **Environmental-qualified** — passed a declared ingress, vibration, humidity or cycle test;
- **Production-qualified** — verified across manufacturing tolerances and multiple samples;
- **Open** — unresolved.

A visually successful fit is not equivalent to production qualification.

## 48. Claims that must not be made yet

Until measurements and tests are recorded, do not claim:

- exact board dimensions;
- exact mounting-hole spacing or diameter;
- exact display active-area dimensions;
- exact safe bezel overlap;
- an official or universal CYD enclosure fit;
- compatibility with every ESP32-2432S028 or `R` revision;
- an IP dust or water rating;
- a maximum safe enclosure temperature;
- sufficient passive cooling;
- vibration or drop resistance;
- connector cycle life;
- safe metal-enclosure operation;
- unchanged Wi-Fi performance in the enclosure;
- a verified speaker acoustic volume;
- safe external battery placement;
- a production tolerance stack.

## 49. Completion criteria

The mechanical-integration system can be considered sample-qualified when:

1. board outline is measured;
2. PCB thickness is measured;
3. all mounting-hole centers and diameters are recorded;
4. hole electrical status is checked;
5. maximum heights on both sides are measured;
6. display frame, visible area and active area are mapped;
7. touch active area and safe bezel border are verified;
8. USB, microSD and all connector mating envelopes are measured;
9. BOOT and RESET actuator geometry is verified;
10. antenna keep-out volume is documented;
11. speaker and cable routes are fixed;
12. CAD and 2D drawings are versioned;
13. tolerance stack is reviewed;
14. prototype fit passes without board flex;
15. touch passes after assembly;
16. USB upload and service access pass;
17. microSD and connector access pass;
18. RF degradation is measured;
19. thermal stress passes in the closed enclosure;
20. cable strain relief is tested;
21. service-cycle testing passes;
22. relevant vibration, humidity or ingress tests are defined and completed;
23. multiple board samples are checked for fit if production use is intended;
24. all open assumptions remain explicitly labeled.

## 50. Open questions

- What are the exact board outline dimensions?
- What is the PCB thickness and warp?
- What are the mounting-hole coordinates and diameters?
- Are mounting holes grounded or isolated?
- What are the maximum component heights on both sides?
- What are the exact display visible and active areas?
- What bezel overlap is safe for the resistive touch panel?
- What front-window material and gap preserve touch performance?
- Which cable housings and bend radii must be supported?
- How much microSD insertion and ejection clearance is required?
- What external button-plunger geometry is reliable?
- What antenna clearance is achievable in the final product?
- How much does the enclosure reduce Wi-Fi performance?
- Which regulator becomes hottest in the closed enclosure?
- Is ventilation required at the maximum ambient temperature?
- What speaker size and rear volume are intended?
- What environmental exposure must the product survive?
- Which enclosure process will be used: printed, machined or molded?
- What manufacturing tolerances must be absorbed?
- How many service cycles are required?

## 51. Summary

Mechanical integration must be based on a controlled measurement of the documented board sample, not on generic CYD dimensions. The enclosure must preserve display visibility, resistive-touch freedom, connector access, button travel, microSD service, antenna clearance, cable strain relief, insulation and thermal margin.

The most important rule is to treat the enclosure as part of the full system. A change in bezel pressure, cable route, speaker position, metal hardware or ventilation can alter touch behavior, RF performance, power stability and component temperature. Final acceptance therefore requires dimensional, functional, thermal and RF tests on the complete assembly.