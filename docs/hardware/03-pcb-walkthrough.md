# PCB Walkthrough — ESP32-2432S028

> **Document status:** guided, photo-led tour of the author's `ESP32-2432S028` sample.  
> **Method:** follow the board by functional signal flow, while keeping visible evidence separate from unverified net assumptions.  
> **Related chapters:** [Hardware Overview](01-hardware-overview.md) and [PCB Component Atlas](02-pcb-component-atlas.md).

![ESP32-2432S028 board overview](../../hardware/images/board-overview.jpg)

## 1. How to use this walkthrough

This chapter is not a substitute for a schematic. It explains how the visible hardware is arranged and how the main subsystems probably interact, but hidden copper routes are not treated as proven until they are traced, measured, or verified in software.

The tour follows this practical order:

```text
USB input
  -> USB-to-UART and automatic programming
  -> 3.3-V power generation
  -> ESP-WROOM-32 core
  -> TFT and touch paths
  -> storage paths
  -> user controls and indicators
  -> audio amplifier and speaker output
  -> external expansion connectors
```

Confidence labels used below:

- **Confirmed** — readable marking, PCB label, or directly visible construction;
- **Function confirmed** — component role is established by marking and placement;
- **Likely path** — strongly supported by layout and board-family behavior, but not continuity-tested;
- **Open** — requires measurement, net tracing, or a software probe.

## 2. Starting point: USB connector

The USB connector is the normal entry point for both board power and firmware upload.

### What is confirmed

- the connector supplies the board from a USB host or 5-V USB supply;
- data lines lead into the USB-to-UART area;
- the photographed sample uses a CH340G at U6.

### Functional split

```text
USB connector
  |-- 5 V power path --> regulators and board loads
  `-- USB D+/D- ------> CH340G --> ESP32 UART0
```

The exact input-protection network is not yet identified. Small passives close to the connector may provide filtering, series impedance, decoupling, or transient suppression depending on revision.

### Practical checks

- inspect the connector mechanically before repeated plugging;
- verify 5 V on the board side of the connector;
- do not assume that every visible small package is an ESD suppressor;
- use a current-limited supply during reverse-engineering work.

## 3. USB-to-UART and automatic programming zone

![CH340G macro photograph](../../hardware/images/ch340g.jpg)

### 3.1 CH340G — U6

**Status:** Confirmed from package marking.

The CH340G converts USB data into ESP32 serial programming and console signals. Its board-level tasks are:

- ROM bootloader communication;
- firmware upload;
- serial monitor output;
- application UART communication with a host.

This exact sample should be documented as **CH340G**, even when generic CYD instructions mention CH340C.

### 3.2 Auto-reset / auto-boot network

Two nearby small transistor packages and their resistors are consistent with the standard ESP32 automatic programming circuit.

Likely control flow:

```text
CH340 control outputs
  |---> transistor network ---> ESP32 EN
  `---> transistor network ---> ESP32 GPIO0
```

This allows upload tools to reset the ESP32 and select the ROM bootloader automatically.

**Open:** exact transistor types, resistor values, and final topology.

### Troubleshooting order

When automatic upload fails:

1. confirm the USB cable supports data;
2. confirm the CH340 driver is visible to the operating system;
3. press RESET once and retry;
4. hold BOOT during the start of upload;
5. inspect EN and GPIO0 activity;
6. only then suspect the ESP32 module or Flash.

## 4. Power entry and regulation

![Power regulators macro photograph](../../hardware/images/power-regulators.jpg)

The board contains two regulators marked **AMS1117-3.3**.

### 4.1 What is established

- U1 is a 3.3-V AMS1117-family linear regulator;
- U7 is a second 3.3-V AMS1117-family linear regulator;
- USB provides the principal 5-V input during normal development.

### 4.2 What is not yet established

- which loads are supplied by U1 and U7;
- whether one rail is dedicated to display/backlight or another high-load domain;
- whether the two outputs are fully separate, linked, or selectively joined;
- whether either regulator can be back-powered through an expansion connector.

### 4.3 Thermal path

For a linear regulator:

```text
PLOSS = (VIN - VOUT) × IOUT
```

At 5 V input and 3.3 V output, a 100-mA load produces about 0.17 W of heat. Wi-Fi transmit bursts, the TFT backlight, microSD activity, and audio output can increase dissipation and voltage transients.

### Measurement sequence

1. identify both regulator input pins;
2. measure both output voltages at idle;
3. observe rails during boot;
4. repeat during Wi-Fi transmission, full backlight, SD access, and audio load;
5. record regulator temperature;
6. test whether connector-supplied 3.3 V back-feeds either regulator.

## 5. Central processing and radio zone

![ESP-WROOM-32 macro photograph](../../hardware/images/esp-wroom-32.jpg)

The ESP-WROOM-32 module is the center of the board.

### 5.1 Visible construction

The module includes:

- classic ESP32 SoC family;
- RF shield;
- module program Flash;
- crystal and RF support circuitry;
- PCB antenna at the edge-facing end.

### 5.2 Board-level responsibilities

The ESP32 coordinates:

- TFT image transfer and control;
- resistive-touch acquisition;
- microSD access;
- auxiliary Flash access, if used;
- audio generation;
- RGB LED and light-sensor input;
- BOOT and RESET handling;
- external UART and GPIO headers;
- Wi-Fi and Bluetooth.

### 5.3 Antenna keep-out

The antenna end should remain free of:

- metal enclosure walls;
- batteries;
- grounded copper plates;
- dense cable bundles;
- speaker magnets placed directly against it.

Poor enclosure placement can reduce radio range even when firmware is correct.

## 6. TFT display path

The 2.8-inch display occupies the opposite side of the board.

### Confirmed

- TFT assembly is physically present;
- resolution class is associated with the 2432 model designation;
- a resistive touch overlay is fitted.

### Working model

The common software configuration uses a serial display interface and often an ILI9341-compatible controller. For this sample, the exact controller remains open until one of the following is obtained:

- reliable controller-ID readout;
- visible flex marking;
- successful initialization with a documented controller-specific sequence;
- manufacturer BOM or schematic for this exact revision.

### Likely signal groups

```text
ESP32
  |-- display clock/data
  |-- TFT chip select
  |-- data/command
  |-- reset
  `-- backlight control
```

Do not confuse TFT signals with XPT2046 touch signals. They are separate functional paths even when both use serial buses.

### Practical inspection

- verify image orientation and color order;
- test all four rotations;
- check whether backlight control is inverted;
- test whether GPIO21 conflicts with external use on this sample;
- record maximum stable display clock rather than copying a generic value.

## 7. Resistive-touch path

![XPT2046 macro photograph](../../hardware/images/xpt2046.jpg)

### 7.1 XPT2046 — U3

**Status:** Confirmed from marking.

The XPT2046 measures the resistive overlay. It does not drive the LCD image.

Functional path:

```text
Touch overlay electrodes
        |
      XPT2046
        |
  serial data + IRQ
        |
      ESP32
```

### 7.2 What software must establish

- raw X/Y range;
- axis swap and inversion;
- touch-pressure threshold;
- IRQ polarity;
- calibration coefficients for each display rotation.

Touch calibration belongs to the individual physical panel. Values from another board may be visibly wrong even on the same nominal model.

## 8. Non-volatile storage paths

The board has at least three distinct storage-related elements:

1. program Flash inside the ESP-WROOM-32 module;
2. auxiliary W25Q32JV on the carrier PCB;
3. removable microSD card.

### 8.1 ESP32 module Flash

This is the normal boot and firmware storage. It is hidden under or within the module assembly and should not be confused with U4.

### 8.2 Auxiliary W25Q32JV — U4

![W25Q32 macro photograph](../../hardware/images/w25q32.jpg)

**Status:** Part confirmed; routing and use open.

The visible W25Q32JV is a separate 32-Mbit / 4-MB serial NOR Flash.

Possible uses include resource storage, but the board photograph alone does not prove that it contains fonts, images, factory data, or a filesystem.

Safe investigation order:

1. continuity-map supply, ground, CS, clock, and data lines;
2. perform a non-destructive JEDEC-ID read;
3. back up the complete contents;
4. only then experiment with writing or erasing.

### 8.3 microSD socket

The card socket is the removable-storage path. Typical uses include:

- images and fonts;
- logs;
- configuration files;
- firmware resources;
- data exchange with a PC.

Open questions include exact GPIO assignment, bus sharing, card-detect support, and maximum stable clock on this revision.

## 9. User controls and indicators

### 9.1 BOOT button

The BOOT button is associated with ESP32 GPIO0. Holding it low during reset enters the serial ROM bootloader.

### 9.2 RESET button

RESET acts through the ESP32 enable/reset path.

### 9.3 RGB LED

A three-channel status LED is present on common CYD revisions. Software must verify:

- exact GPIOs;
- channel order;
- active-high or active-low behavior.

### 9.4 LDR / photoresistor

The light sensor is used as a simple ambient-light input through a resistor divider. It should be treated as a relative sensor, not a calibrated lux meter.

## 10. Audio path

![8002A macro photograph](../../hardware/images/ns8002a.jpg)

### 10.1 8002A — U5

**Status:** Function confirmed from marking, placement, and the adjacent speaker connector.

Likely path:

```text
ESP32 audio-capable output
        |
  coupling / gain network
        |
      8002A
     /     \
  OUT+     OUT-
     \     /
   P4 speaker connector
```

### 10.2 Critical rule: BTL output

P4 is a differential bridge output. Neither speaker lead is ground.

Do not connect either P4 pin to:

- board GND;
- oscilloscope protective earth;
- common-ground headphones;
- a stereo amplifier input with shared ground.

Use a floating speaker or a differential measurement method.

### 10.3 Speaker starting point

A small 4-ohm or 8-ohm speaker is a practical test load. Actual safe output power must be measured on this board; it should not be inferred from optimistic seller listings.

## 11. External connector zone

![Connector macro photograph](../../hardware/images/connectors.jpg)

The external wire-to-board connectors are approximately 1.25-mm-pitch parts with visible side-retention geometry.

### 11.1 P1 — UART / service connector

Used for serial access and possibly power reference. Exact pin sequence must be verified against the silkscreen and continuity measurements.

### 11.2 P3 — GPIO connector

Visible labels on this sample:

```text
GND | IO35 | IO22 | IO21
```

Important consequences:

- GPIO35 is input-only;
- GPIO21 may conflict with an onboard function such as backlight control;
- external devices must use 3.3-V logic.

### 11.3 CN1 — auxiliary connector

Visible labels on this sample:

```text
GND | NC | IO27 | 3.3
```

The printed `NC` is a revision-specific fact and should not be replaced by a generic online pinout.

### 11.4 P4 — speaker connector

Two-wire differential speaker output from the 8002A stage.

### 11.5 Connector family status

The mechanical form is close to PicoBlade-compatible 1.25-mm families, including Scondar SCT1251-type products. Exact manufacturer and mating-part number remain unconfirmed until a physical fit test is recorded.

## 12. Passive networks: what can be inferred safely

Small resistors, capacitors, transistors, and possible diodes surround every major subsystem.

Their likely roles include:

- decoupling and regulator stability;
- USB signal conditioning;
- boot-strap biasing;
- TFT and touch pull-ups or series damping;
- audio input bias, gain, and coupling;
- LED current limiting;
- LDR voltage division;
- connector-line protection.

Package placement supports these general roles, but component values and exact nets must not be invented from appearance alone.

## 13. Recommended board bring-up order

A disciplined first-power sequence reduces debugging ambiguity:

1. inspect for solder bridges, loose connectors, and damaged flex cable;
2. power from a current-limited USB source;
3. confirm 5-V input and both 3.3-V regulator outputs;
4. confirm CH340 enumeration;
5. read ESP32 information with `esptool.py`;
6. run a minimal serial blink/BOOT test;
7. test TFT with conservative clock settings;
8. test raw XPT2046 coordinates;
9. mount and read a microSD card;
10. probe auxiliary Flash non-destructively;
11. test audio at low duty cycle and volume;
12. test external connectors one signal at a time.

## 14. Diagnostic flow by symptom

### Board is completely dead

Check USB cable, 5-V input, both regulator outputs, shorts, and ESP32 EN.

### Serial port appears but upload fails

Check BOOT/RESET timing, GPIO0, EN, CH340 control lines, and whether another device is driving UART0.

### Display is white or blank

Check backlight separately from LCD initialization, then verify power, reset, CS, DC, clock/data routing, and controller selection.

### Touch gives no data

Check XPT2046 CS, clock, data lines, IRQ level, and panel flex connection. Do not diagnose the LCD controller first.

### microSD fails intermittently

Reduce SPI clock, improve supply stability, try another card, and check bus conflicts.

### Audio is distorted

Reduce signal amplitude and duty cycle, verify speaker impedance, measure the amplifier supply, and avoid grounding either output.

### Wi-Fi range is poor

Move metal, the speaker magnet, battery, and wiring away from the antenna end.

## 15. Walkthrough conclusions

The board is best understood as several compact subsystems around the ESP-WROOM-32:

- USB programming and reset control;
- dual 3.3-V regulation;
- TFT and resistive touch;
- internal, auxiliary, and removable storage;
- local indicators and controls;
- a mono BTL audio stage;
- revision-sensitive expansion connectors.

The photographs establish the physical component set and major functional zones. They do not yet establish the complete netlist. Future chapters should replace the remaining “likely path” statements with continuity measurements, software probes, and electrical test results.

## 16. Next chapter

The next hardware-reference chapter should be a dedicated **Connector Reference** covering mechanical identification, pin orientation, mating housings, cable assemblies, signal tables, voltage cautions, and fit-test evidence.
