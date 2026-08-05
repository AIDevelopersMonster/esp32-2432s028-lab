# PCB Component Atlas — ESP32-2432S028

> **Document status:** photo-led component atlas for the author's `ESP32-2432S028` sample.  
> **Evidence rule:** readable package markings and PCB reference designators are treated as primary evidence; electrical roles that require hidden-net tracing are explicitly marked as pending verification.

![ESP-WROOM-32 macro photograph](../../hardware/images/esp-wroom-32.jpg)

## 1. Purpose and navigation

This atlas identifies the principal components visible on the PCB side of the examined board and records what can, and cannot, be concluded from photographs alone.

The atlas is organized by functional zone:

1. processing and radio;
2. display and touch;
3. non-volatile storage;
4. USB and programming;
5. audio;
6. power;
7. user-interface parts;
8. connectors and electromechanical parts;
9. passive and support circuitry;
10. unresolved or hidden parts.

A component entry uses the following confidence labels:

- **Confirmed** — package marking or PCB label is readable on the author's sample;
- **Function confirmed** — position, marking and board labeling establish the role;
- **Likely** — strongly supported by layout or common CYD design practice, but not yet continuity-tested;
- **Open** — cannot be established from the current photographs.

## 2. Atlas summary

| PCB reference / area | Identified part | Function | Status |
|---|---|---|---|
| Radio module area | ESP-WROOM-32 | main MCU, Wi-Fi, Bluetooth and module Flash | Confirmed |
| U3 | XPT2046 | resistive-touch controller | Confirmed |
| U4 | Winbond W25Q32JV | 32-Mbit auxiliary serial Flash | Confirmed; board use open |
| U5 | 8002A-family amplifier | mono BTL speaker amplifier | Function confirmed |
| U6 | WCH CH340G | USB-to-UART bridge | Confirmed |
| U1 | AMS1117-3.3 | 3.3-V linear regulator | Confirmed |
| U7 | AMS1117-3.3 | second 3.3-V linear regulator | Confirmed |
| T1, T2 | small-signal transistors | automatic BOOT/RESET interface | Likely; topology pending tracing |
| TF socket | microSD socket | removable storage | Confirmed |
| P1 | 4-pin wire-to-board connector | service UART / power | Board-label confirmed; exact pin order revision-sensitive |
| P3 | 4-pin wire-to-board connector | GPIO expansion | Board-label confirmed |
| CN1 | 4-pin wire-to-board connector | GPIO / 3.3-V expansion | Board-label confirmed |
| P4 | 2-pin wire-to-board connector | speaker output | Function confirmed |
| BOOT | tactile switch | GPIO0 boot-mode control | Board-label confirmed |
| RESET | tactile switch | ESP32 enable/reset control | Board-label confirmed |
| RGB LED area | RGB LED and current-setting resistors | user indicator | Family-typical; exact package/net assignment pending |
| LDR area | photoresistor and divider | ambient-light sensing | Family-typical; exact sample net pending |
| Display assembly | 2.8-inch TFT with resistive overlay | visual and touch interface | Physical assembly confirmed; TFT controller open |

## 3. Processing and radio zone

### 3.1 ESP-WROOM-32 module

**Visible marking:** `ESP-WROOM-32` with regulatory markings.  
**Status:** Confirmed.

The module is the central processing and radio subsystem. It contains:

- the classic ESP32 SoC family;
- dual-core Xtensa LX6 CPU architecture;
- 2.4-GHz Wi-Fi;
- Bluetooth Classic and BLE;
- RF matching and shielding;
- a PCB antenna at the unshielded end;
- the module's own boot/program Flash.

#### Board-level role

The module controls the TFT, touch controller, microSD interface, auxiliary Flash, audio path, status devices, buttons and external headers.

#### Layout observations

- The antenna is placed at the PCB edge.
- Copper, metal mounting hardware, batteries and cable bundles should be kept away from the antenna end.
- The module castellated pads are soldered directly to the carrier PCB.
- The visible marking identifies the classic WROOM-32 family, not ESP32-S2, S3, C3 or C6.

#### Verification tasks

- read chip revision and Flash size with `esptool.py`;
- record MAC and efuse information only in private test logs;
- test radio performance with and without the intended enclosure;
- confirm whether the module Flash is 4 MB on this exact unit.

## 4. Display and touch zone

### 4.1 TFT display assembly

**Visible evidence:** 2.8-inch-class TFT assembly with a resistive touch overlay.  
**Status:** Display size and physical construction confirmed; controller open.

The LCD controller is not visible on the accessible PCB side. It may be mounted on the flex tail or implemented as chip-on-glass/chip-on-flex beneath the display assembly.

The commonly reported controller for this board family is ILI9341, but this atlas does not treat that as sample-specific proof.

#### Safe documentation claim

- 240 × 320 class, 2.8-inch TFT assembly: high confidence from board model and physical format;
- SPI-style host connection: supported by known board-family routing and current software configuration, but should be verified against this sample;
- exact controller and initialization sequence: open until tested.

### 4.2 XPT2046 — U3

**Visible marking:** `XPT2046`.  
**Status:** Confirmed.

U3 is a resistive-touchscreen controller. It is a 12-bit ADC-based device used to measure the X and Y electrode voltages of the resistive panel and, where used by software, pressure-related Z channels.

#### Important distinction

XPT2046 does **not** generate the LCD image. It handles only the resistive touch layer.

#### Expected signals

- serial clock;
- controller-select;
- data input and output;
- touch interrupt;
- X+, X−, Y+, Y− panel connections;
- 3.3-V supply and ground.

#### Sample-specific open questions

- confirmed GPIO assignment by continuity test;
- IRQ polarity and pull-up implementation;
- raw coordinate range and orientation;
- stable calibration coefficients for each display rotation.

## 5. Non-volatile storage zone

### 5.1 Module program Flash

The ESP-WROOM-32 module includes its own boot/program Flash beneath or within the RF-shielded module assembly. It is distinct from U4.

### 5.2 Winbond W25Q32JV — U4

**Visible marking:** Winbond `25Q32JV...`.  
**Status:** Part confirmed; system use open.

W25Q32JV is a 32-Mbit serial NOR Flash device:

- capacity: 32 Mbit = 4 MB;
- 3.3-V supply class;
- SPI and extended serial modes depending on wiring;
- non-volatile resource or data storage.

#### What the photograph proves

- a separate Winbond W25Q32JV-class device is populated on the carrier PCB;
- it is not the hidden Flash already contained in the ESP-WROOM-32 module.

#### What the photograph does not prove

- which ESP32 GPIOs are connected to it;
- whether it shares a bus with the TFT, touch or SD card;
- whether production firmware uses it;
- whether it stores fonts, graphics, filesystem data or factory resources.

#### Recommended verification

1. continuity-map CS, CLK and data pins;
2. inspect boot logs and existing firmware for JEDEC-ID reads;
3. run a non-destructive JEDEC-ID probe;
4. do not erase or program the chip until its existing contents have been backed up.

### 5.3 microSD / TF socket

**Status:** Confirmed.

The board includes a push-style microSD socket. The card interface is used for files, images, logs and firmware resources in many CYD projects.

Open questions for this exact sample:

- bus assignment and pin sharing;
- card-detect implementation, if any;
- maximum stable SPI clock;
- power-up behavior with high-current cards.

## 6. USB and programming zone

### 6.1 CH340G — U6

**Visible marking:** WCH `CH340G`.  
**Status:** Confirmed.

The CH340G converts USB signalling to ESP32 UART-level serial data. It supports:

- firmware upload through the ESP32 ROM bootloader;
- serial console output;
- application UART communication with a host computer.

#### Revision note

Some CYD documentation names CH340C. The photographed sample is explicitly fitted with **CH340G**, so software-driver discussion should cover the CH340 family while the hardware reference should retain the exact `G` suffix.

### 6.2 T1 and T2 transistor pair

**Visible evidence:** two small transistor packages near U6 and associated resistors.  
**Status:** Likely automatic programming circuit; exact topology pending tracing.

The location and arrangement are consistent with the common ESP32 auto-reset/auto-boot circuit, in which CH340 control outputs manipulate:

- `EN` / reset;
- `GPIO0` / boot selection.

This allows normal flashing without manually holding BOOT, provided the driver and upload tool toggle the serial control lines correctly.

### 6.3 USB connector and protection parts

The USB connector supplies board power and data to U6. Visible nearby passives may include:

- USB series resistors;
- local decoupling;
- input filtering;
- ESD or transient protection, depending on revision.

The current photographs do not establish the exact protection topology.

## 7. Audio zone

### 7.1 8002A amplifier — U5

**Visible marking:** `8002A`.  
**Status:** Function confirmed by marking, layout and adjacent `SPK` connector.

The 8002A family is a small mono audio power amplifier typically used in bridge-tied-load configuration.

#### Board-level signal path

```text
ESP32 audio/PWM/DAC signal
          |
       input network
          |
        8002A
       /     \
    OUT+     OUT-
       \     /
      P4 speaker
```

#### Critical electrical rule

The output is BTL: the speaker is connected **between OUT+ and OUT−**. Neither speaker lead should be connected to board ground, oscilloscope earth, grounded audio equipment or a common-ground stereo input.

#### Speaker guidance

Practical starting loads are usually:

- 4-ohm miniature speaker, conservatively rated for the board's tested output;
- 8-ohm miniature speaker for lower current and typically lower volume.

The frequently repeated claim of “3 W” must not be applied to this board without confirming amplifier supply voltage, clipping level, thermal conditions and load impedance.

#### Verification tasks

- trace amplifier supply rail;
- measure idle current;
- identify the ESP32 audio GPIO and coupling network;
- measure differential output into a dummy load;
- establish unclipped power and regulator temperature.

## 8. Power zone

### 8.1 AMS1117-3.3 — U1

**Visible marking:** `AMS1117 3.3`.  
**Status:** Confirmed.

### 8.2 AMS1117-3.3 — U7

**Visible marking:** `AMS1117 3.3`.  
**Status:** Confirmed.

The photographed board contains two nominal 3.3-V linear regulators. Their presence is established; the rail split is not.

Possible functions include separate regulation for high-load and logic domains, but examples such as “one regulator for ESP32 and one for display” remain hypotheses until net tracing or measurements establish them.

#### Thermal significance

A linear regulator dissipates approximately:

```text
PLOSS = (VIN - VOUT) × IOUT
```

With a 5-V input and 3.3-V output, every 100 mA produces roughly 0.17 W in the regulator. Wi-Fi, TFT backlight, SD-card and audio loads can therefore make thermal behavior relevant.

#### Associated passives

Capacitors adjacent to U1 and U7 provide input/output decoupling and loop stability. Their values cannot be read from the photographs and should not be guessed.

#### Verification tasks

- identify each regulator input and output rail;
- measure rail voltages during boot and heavy load;
- record regulator case temperature;
- test minimum stable USB supply voltage;
- determine whether any solder jumpers isolate the rails.

## 9. User-interface components

### 9.1 BOOT switch

**PCB marking:** `BOOT`.  
**Status:** Confirmed.

The BOOT switch is expected to pull ESP32 GPIO0 low. Pressing it during reset selects the ROM serial bootloader.

### 9.2 RESET switch

**PCB marking:** reset function indicated by placement and board family.  
**Status:** Function confirmed; exact printed text should be checked against the high-resolution board image.

The switch drives the ESP32 enable/reset network.

### 9.3 RGB status LED

**Status:** Present on common revisions; sample-level pin and polarity should be confirmed by the board test.

Common CYD configurations use three ESP32 GPIOs and active-low channels. The firmware must not assume polarity solely from generic examples.

### 9.4 Photoresistor / LDR

**Status:** Present on common revisions; exact sample net pending test.

The LDR is normally used in a resistor divider connected to an ADC-capable input. It can provide ambient-light indication but is not a calibrated lux sensor.

## 10. Connectors and electromechanical parts

### 10.1 P1 — service UART connector

Four-position, 1.25-mm-pitch-class wire-to-board connector. Board markings identify the UART function.

The exact pin sequence, voltage pin and mating housing should be checked against the connector chapter before wiring.

### 10.2 P3 — GPIO connector

Visible sample labels:

- `GND`;
- `IO35`;
- `IO22`;
- `IO21`.

GPIO35 is input-only. GPIO21 may have a second on-board function, commonly backlight control, so external use can create conflicts.

### 10.3 CN1 — auxiliary GPIO/power connector

Visible sample labels:

- `GND`;
- `NC`;
- `IO27`;
- `3.3`.

The visibly printed `NC` is revision-significant and should not be replaced in the documentation by a later online pinout.

### 10.4 P4 — speaker connector

Two-position connector connected to the differential amplifier output. It carries `SPK+` and `SPK−`, not speaker and ground.

### 10.5 Candidate connector family

Mechanical appearance is close to 1.25-mm-pitch PicoBlade-compatible families, including the Scondar SCT1251 family examined during procurement research.

Current status:

- pitch class: high confidence;
- side-retention geometry: photo verified;
- exact manufacturer and article number: open;
- bulk procurement: not recommended until a sample cable has been fit-tested.

## 11. Passive and support circuitry

The board contains numerous resistors, capacitors, transistors and possible diodes that support:

- regulator stability and rail decoupling;
- USB signal conditioning;
- ESP32 boot strapping;
- LCD and touch signal conditioning;
- audio gain, bias and coupling;
- LED current limiting;
- LDR voltage division;
- pull-ups and pull-downs;
- connector-line protection or series damping.

Package appearance alone is insufficient to assign values or guaranteed roles. Passive-network documentation belongs in the later PCB walkthrough and reverse-engineering chapters, where each claim can be linked to a traced net.

## 12. Hidden and unresolved components

The following cannot currently be identified from the available accessible surfaces:

- exact TFT controller;
- TFT flex/COG driver markings;
- display backlight driver topology;
- full touch-panel electrode routing;
- exact values of most passives;
- complete power-tree partitioning;
- auxiliary Flash bus assignment;
- USB protection-device identity;
- exact transistor types at T1/T2;
- possible differences beneath the display module.

The display should not be removed solely for documentation unless the risk of damaging the flex, adhesive or touch overlay is acceptable.

## 13. Photo and evidence checklist for future revisions

For every newly documented board revision, capture:

1. full PCB side, square to the camera;
2. full display side;
3. ESP-WROOM marking and antenna area;
4. U3 touch-controller marking;
5. U4 Flash marking;
6. U5 amplifier marking;
7. U6 USB-UART marking;
8. both regulators and nearby jumpers;
9. every external connector from front, top and side;
10. PCB model/revision silkscreen;
11. display-flex markings, only when safely visible;
12. measured connector pitch with a scale in frame.

## 14. Atlas conclusions

The author's sample is a comparatively feature-rich ESP32 carrier board rather than merely an ESP32 attached to a screen. Photographs establish a classic ESP-WROOM-32 platform with dedicated resistive touch, auxiliary serial Flash, USB-UART, audio amplification, dual 3.3-V regulation, removable storage and multiple expansion connectors.

The highest-value unresolved items are the TFT controller identity, the purpose and routing of U4, the split between U1 and U7, and exact mating connector part numbers. These should be resolved by software probing, continuity measurement and fit testing rather than by repeating generic CYD assumptions.

## 15. Manufacturer-document references

Use current manufacturer documentation for electrical limits and package pinouts:

- Espressif ESP32-WROOM-32 datasheet and hardware-design guidelines;
- Winbond W25Q32JV serial-Flash datasheet;
- WCH CH340 family documentation;
- XPT2046-compatible resistive-touch-controller documentation;
- AMS1117-compatible regulator datasheets, while noting vendor-to-vendor variation;
- the actual 8002A vendor datasheet matching the package source, if the manufacturer can be established.

A third-party component advertised as “compatible” must not automatically be assumed to have identical absolute-maximum ratings, thermal performance or pin behavior.