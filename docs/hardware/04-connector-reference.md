# Connector Reference — ESP32-2432S028

> **Document status:** photo-led connector reference for the author's `ESP32-2432S028` sample.  
> **Scope:** connector locations, visible signal labels, orientation discipline, electrical cautions, candidate mating families, cable procurement, and fit-test procedure.  
> **Important limitation:** the exact connector manufacturer and exact mating-part article numbers have not yet been confirmed by a physical fit test.

![ESP32-2432S028 connector area](../../hardware/images/connectors.jpg)

## 1. Purpose

The board uses several small white wire-to-board connectors that are frequently described by sellers only as `JST 1.25`, `1.25 mm`, or `CYD cable`. Those descriptions are not precise enough for reliable procurement.

This chapter separates four different questions:

1. what each board connector does;
2. what the PCB silkscreen says on this exact sample;
3. what mechanical connector family it resembles;
4. what still has to be verified with a real mating cable.

A matching pitch alone does not guarantee compatibility. Housing width, keying, contact position, insertion direction, latch geometry, terminal shape, and polarization must also match.

## 2. Connector inventory

| Reference | Positions | Board-level purpose | Sample evidence | Mechanical status |
|---|---:|---|---|---|
| P1 | 4 | UART0/service connection and power | `GND`, `RX`, `TX`, `5V` visible near the connector | 1.25-mm-class side-entry connector; exact family open |
| P3 | 4 | GPIO expansion | `GND`, `IO35`, `IO22`, `IO21` visible | same apparent family as P1/CN1 |
| CN1 | 4 | auxiliary GPIO and 3.3-V supply | `GND`, `NC`, `IO27`, `3.3V` visible | same apparent family as P1/P3 |
| P4 | 2 | differential speaker output | position next to 8002A audio stage and `SPK` function | same 1.25-mm-class family in two-position form |
| USB | 5-contact USB connector | 5-V power, programming and serial monitor | visible connector and CH340G path | standard USB cable interface; not part of the 1.25-mm family |
| microSD | card socket | removable storage | visible socket | standard microSD card interface |

## 3. Orientation convention

Connector documentation becomes dangerous when `pin 1`, `left`, or `right` is used without defining the viewing direction.

This reference therefore uses the following convention:

- **PCB-side view:** component side of the PCB facing the observer;
- **connector opening:** the direction from which the cable housing is inserted;
- **silkscreen order:** signal names read in the same direction as printed beside the connector;
- **pin number:** not assigned until the pin-1 end is positively established from a drawing, continuity map, or polarity mark.

The tables below record **visible silkscreen order**, not a universal manufacturer pin numbering scheme.

Before connecting a cable:

1. place the board in the same orientation as the photograph;
2. locate the connector reference designator;
3. read the adjacent signal names;
4. identify the ground contact with a continuity meter;
5. identify supply voltage with the board powered and no external device attached;
6. label the cable conductors yourself.

Do not use wire color as evidence. Pre-crimped cable colors are not standardized.

## 4. P1 — UART/service connector

### 4.1 Visible labels

The macro photograph shows the signal group:

```text
GND | RX | TX | 5V
```

This is the visible order printed beside P1 on the documented sample. The connector must still be viewed in the same physical orientation before this order is applied.

### 4.2 Intended use

P1 exposes a service serial interface associated with the ESP32 programming/console path. Typical uses include:

- serial console access without using USB;
- connection to an external USB-to-UART adapter;
- communication with another 3.3-V UART device;
- access to board ground and the board's 5-V rail.

### 4.3 Signal-direction rule

The labels are board signal names:

- board `TX` connects to the external adapter's `RX`;
- board `RX` connects to the external adapter's `TX`;
- `GND` connects to `GND`;
- `5V` is a power rail, not a UART logic signal.

### 4.4 Voltage caution

ESP32 UART logic is 3.3 V. Do not connect a 5-V TTL UART output directly to ESP32 `RX` merely because the connector also contains a `5V` pin.

The `5V` contact may be useful for powering a peripheral or feeding the board, but its direction, protection, and allowable current must be verified before using it as an alternate power input.

### 4.5 Recommended first test

Use only three conductors initially:

```text
GND
board TX -> adapter RX
board RX <- adapter TX
```

Leave `5V` disconnected while the board is powered from USB. This avoids accidental connection between two power sources.

## 5. P3 — GPIO expansion connector

### 5.1 Visible labels

The documented sample shows:

```text
GND | IO35 | IO22 | IO21
```

### 5.2 GPIO35

GPIO35 is input-only on the classic ESP32.

Suitable uses include:

- digital input;
- ADC input, subject to ESP32 ADC limitations;
- interrupt-capable input where supported by software.

It cannot drive an LED, relay input, clock line, or output signal directly.

GPIO35 also has no general-purpose internal pull-up or pull-down in the same way as ordinary bidirectional GPIOs. An external bias resistor may therefore be required.

### 5.3 GPIO22

GPIO22 is a normal bidirectional 3.3-V GPIO. It is often used as an I²C clock in ESP32 examples, but that is a software convention, not a fixed hardware function.

Before assigning it, confirm that the current firmware and board revision do not already use it for an onboard device.

### 5.4 GPIO21

GPIO21 is a normal bidirectional GPIO, commonly used as I²C data in generic ESP32 projects.

On CYD-family boards it may also be connected to an onboard function, frequently display-backlight control on some revisions. Using it externally can therefore cause:

- backlight flicker;
- unwanted boot-time transitions;
- contention between an external device and an onboard driver;
- failure of examples that assume exclusive control.

The exact shared function must be verified on this sample before treating GPIO21 as free.

### 5.5 Powering external devices

P3 provides `GND` but no dedicated supply pin in the visible four-signal group. A peripheral connected to P3 may need power from CN1 or another verified rail.

Never allow the peripheral signal pins to power the unpowered ESP32 through protection diodes.

## 6. CN1 — auxiliary GPIO and 3.3-V connector

### 6.1 Visible labels

The documented sample shows:

```text
GND | NC | IO27 | 3.3V
```

The visible `NC` position is revision-significant. Some online pinout images for similar CYD boards assign a GPIO to a corresponding position. That must not override the silkscreen on this sample.

### 6.2 NC contact

`NC` means no connection is claimed by the board marking. It must not automatically be used as:

- a spare GPIO;
- a second ground;
- a power contact;
- a mechanical key indication.

A continuity measurement may later show a test pad or revision option, but until then the contact should remain unused.

### 6.3 GPIO27

GPIO27 is a bidirectional 3.3-V GPIO. It may support digital I/O, PWM and other ESP32 peripheral-matrix functions.

Board-family examples sometimes use GPIO27 for a specific peripheral or expansion role. Confirm the repository's pin map and running firmware before connecting a low-impedance external driver.

### 6.4 3.3-V contact

The `3.3V` contact is connected to a regulated board rail. It is not a 5-V-tolerant input and should not be driven from an external 5-V supply.

Before powering a peripheral from it, determine:

- which AMS1117 regulator supplies the contact;
- available current under Wi-Fi, display and SD load;
- voltage drop and thermal rise;
- whether the rail remains powered when USB is absent;
- whether external 3.3-V injection back-powers the board.

For early tests, use small loads such as sensors rather than motors, relays, servos or high-brightness LED assemblies.

## 7. P4 — speaker connector

### 7.1 Function

P4 is the two-position output from the 8002A audio amplifier.

The correct conceptual pin names are:

```text
SPK+
SPK-
```

They are polarity labels for a differential bridge output, not `signal` and `ground`.

### 7.2 Critical BTL rule

Neither P4 contact may be connected to board ground.

Do not connect P4 directly to:

- grounded headphones;
- a common-ground stereo jack;
- an oscilloscope probe whose ground clip is earth-referenced;
- an external amplifier with single-ended grounded input;
- another board's grounded audio input.

The safe normal load is a floating loudspeaker connected only between the two P4 contacts.

### 7.3 Speaker selection

A conservative test selection is:

- 8-ohm miniature dynamic speaker for the first powered test;
- 4-ohm speaker only after supply and thermal behavior are checked;
- power rating at least 1 W for basic tests, with higher ratings acceptable.

A speaker's wattage rating is its handling capability; it does not force the amplifier to produce that power.

Actual board output power is still an open measurement and must not be inferred from the nominal maximum stated for generic 8002A devices.

### 7.4 Polarity

Reversing both speaker leads changes acoustic phase but normally does not damage a single isolated speaker. Maintain consistent polarity when using more than one acoustic source.

## 8. Mechanical connector identification

### 8.1 What the photographs establish

The P1, P3, CN1 and P4 board connectors have the following visible properties:

- nominal pitch close to 1.25 mm;
- white polarized housing;
- side-entry / right-angle cable insertion;
- surface-mount signal contacts;
- larger metal board-retention tabs at both sides;
- external side retention/keying features;
- the same apparent family in two- and four-position forms.

### 8.2 Current best candidate

The closest documented commercial family found during visual comparison is the **Scondar SCT1251 1.25-mm family**, described by its manufacturer as a PicoBlade-compatible connector family.

This supports the broader identification:

> **PicoBlade-compatible, 1.25-mm-class wire-to-board connector — candidate identification.**

It does **not** yet prove that the populated PCB header was manufactured by Molex or Scondar.

Chinese-compatible parts may be sold under labels such as:

- `PicoBlade 1.25`;
- `MX1.25`;
- `SCT1251 compatible`;
- `JST 1.25`;
- `1.25 mm micro connector`.

Only the first three descriptions meaningfully suggest the target geometry. `JST 1.25` is a marketplace umbrella term and may refer to incompatible families.

### 8.3 Families that must not be assumed compatible

The following may share a similar pitch yet differ mechanically:

- JST GH;
- JST SH;
- JST GHD;
- generic A1257/A1251 variants;
- Molex PicoBlade clones with altered latch geometry;
- unbranded `1.25 mm` housings.

A connector may begin to enter and still be wrong. Do not force it.

### 8.4 Candidate mating components

For procurement research, the cable-side part should be described as:

- **female receptacle housing**;
- 4 positions for P1, P3 and CN1;
- 2 positions for P4;
- female crimp terminals matched to the selected wire gauge;
- PicoBlade/SCT1251-compatible geometry;
- 1.25-mm pitch.

Possible cross-reference starting points include Molex PicoBlade `51021`-series cable housings and Scondar SCT1251 cable housings. These are **comparison references**, not yet approved BOM items for this sample.

No exact ordering code should be promoted to `confirmed` until the housing physically mates with the board connector without force and passes retention and electrical checks.

## 9. Buying ready-made cables

For a low-risk trial order, search for:

```text
PicoBlade compatible 1.25 mm 4 pin female pigtail
PicoBlade compatible 1.25 mm 2 pin female pigtail
51021 compatible 4P cable
51021 compatible 2P cable
SCT1251 4P cable assembly
SCT1251 2P cable assembly
```

Prefer listings that provide:

- front, side and top photographs of the housing;
- housing dimensions;
- explicit compatibility with PicoBlade or 51021;
- actual crimped wire gauge;
- contact material;
- cable length;
- a drawing rather than only a generic stock image.

Avoid buying a large pack before testing one or two samples.

Marketplace `male` and `female` terminology is often reversed. The needed cable part is the **socket/receptacle housing with female crimp contacts**, regardless of the seller's title.

## 10. Crimping a custom cable

A complete cable requires:

1. polarized plastic receptacle housing;
2. one female crimp terminal per position;
3. correctly sized stranded wire;
4. a crimp tool suitable for miniature open-barrel terminals;
5. strain relief and labeling.

### 10.1 Wire size

Use the terminal manufacturer's supported wire range. Very thick wire may not enter the housing, while very thin wire may produce a mechanically weak crimp.

For short low-current signal pigtails, fine stranded wire in the approximate 26–30 AWG range is commonly practical, but the selected terminal datasheet is authoritative.

### 10.2 Crimp quality

A correct open-barrel crimp has separate zones for:

- conductor strands;
- insulation support.

Do not fill the crimp with solder as a substitute for correct mechanical crimping. Solder wicking can create a rigid fatigue point at the rear of the terminal.

### 10.3 Terminal insertion

After crimping:

1. inspect for loose strands;
2. identify the terminal locking lance;
3. insert in the housing's correct orientation;
4. pull lightly to verify retention;
5. perform continuity testing end-to-end;
6. verify there are no shorts between adjacent positions.

## 11. Physical fit-test protocol

A candidate cable becomes approved only after this procedure.

### Stage A — unpowered mechanical check

1. compare position count and pitch;
2. compare the keyed shape and side rails;
3. align without twisting;
4. insert using light finger pressure only;
5. stop immediately if the housing jams or spreads the board header;
6. verify that it seats fully and evenly;
7. verify that moderate cable movement does not release it;
8. remove by gripping the housing, not pulling the wires.

### Stage B — unpowered electrical check

1. identify board ground by continuity to USB shield/known ground;
2. map every cable conductor to its board contact;
3. verify no adjacent shorts;
4. record the orientation with photographs;
5. mark conductor functions with labels or heat-shrink.

### Stage C — powered voltage check

1. power the board from a current-limited USB source;
2. leave external equipment disconnected;
3. measure the expected supply contacts;
4. confirm UART/GPIO contacts do not show unexpected supply voltage;
5. power off before attaching the peripheral.

### Stage D — functional check

- P1: verify serial output with power pin initially disconnected;
- P3/CN1: test through high-value resistors or high-impedance inputs first;
- P4: connect only a floating speaker or differential dummy load.

## 12. Cable-orientation record template

After a successful fit test, add a record in this form:

```text
Board revision: ESP32-2432S028, photographed sample
Connector: P1 / P3 / CN1 / P4
Candidate family:
Seller/manufacturer:
Housing article:
Terminal article:
Cable listing URL:
Insertion orientation photograph:
Silkscreen order:
Cable color -> signal map:
Mechanical fit: PASS / FAIL
Retention: PASS / FAIL
Continuity: PASS / FAIL
Powered voltage check: PASS / FAIL
Date tested:
Tester:
Notes:
```

The record should be committed together with a clear photograph. This converts a marketplace guess into repeatable engineering evidence.

## 13. Common failure modes

### Correct pitch, wrong housing

The cable aligns with the contacts but cannot seat because the key, height or latch differs.

**Action:** stop; do not trim or force the housing for a permanent assembly.

### Reversed cable order

The housing fits, but the pre-crimped color order does not correspond to the board silkscreen.

**Action:** depin and rearrange the terminals, or document and label the actual mapping.

### UART appears dead

Possible causes:

- TX connected to TX instead of RX;
- missing common ground;
- wrong baud rate;
- external adapter uses 5-V logic;
- USB and external UART both drive the same line;
- wrong connector orientation.

### Board resets when a peripheral is attached

Possible causes:

- excessive load on 3.3 V;
- shorted cable;
- peripheral back-powering the board;
- GPIO contention;
- supply transient through a long cable.

### Speaker produces no sound

Possible causes:

- active buzzer used instead of passive speaker;
- one P4 lead incorrectly grounded;
- firmware uses the wrong audio GPIO;
- amplifier supply or shutdown state not verified;
- connector contact not fully seated.

## 14. Approved claims and open claims

### Approved for this sample

- P1, P3 and CN1 are four-position connectors;
- P4 is a two-position speaker connector;
- the connectors are in the approximately 1.25-mm pitch class;
- external retention/keying features are visible;
- P1 silkscreen identifies `GND`, `RX`, `TX`, `5V`;
- P3 silkscreen identifies `GND`, `IO35`, `IO22`, `IO21`;
- CN1 silkscreen identifies `GND`, `NC`, `IO27`, `3.3V`;
- P4 is associated with the 8002A differential speaker stage.

### Still open

- exact connector manufacturer;
- exact board-header article numbers;
- exact mating-housing and terminal article numbers;
- manufacturer-defined pin-1 location;
- verified current rating of the populated connectors;
- exact cable wire gauge supplied by any marketplace seller;
- physical compatibility of Molex 51021 or Scondar SCT1251 housings with this sample;
- whether P1's 5-V contact is protected for external power injection;
- safe continuous current available from CN1's 3.3-V contact.

## 15. Recommended next actions

1. Obtain one two-position and one four-position PicoBlade/SCT1251-compatible pigtail.
2. Perform the unpowered mechanical fit test.
3. Photograph the fully seated connector from top and side.
4. Map conductor order with a continuity meter.
5. Record dimensions using calipers.
6. Promote the successful housing and terminal articles to a verified BOM only after the test.
7. Keep rejected connector families in a compatibility table so future users do not repeat the same purchases.

## 16. External comparison references

The following are useful for dimensional and visual comparison, but they do not replace sample testing:

- Scondar SCT1251 1.25-mm connector family;
- Molex PicoBlade wire-to-board family and 51021-series receptacle housings;
- manufacturer drawings for any candidate board header and mating housing;
- the author's macro photographs in this repository.

Commercial links can disappear or change. The final reference should preserve manufacturer name, article number, drawing revision, measured dimensions and a fit-test photograph rather than relying only on a seller URL.
