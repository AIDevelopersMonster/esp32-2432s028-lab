# Hardware Overview — ESP32-2432S028

> **Document status:** photo-verified overview of the author's board sample.  
> **Scope:** board identification, subsystem map, visible components, interfaces, and confidence notes.  
> **Not yet covered:** complete schematic reconstruction, verified net tracing, electrical measurements, and display-controller readout.

![ESP32-2432S028 board overview](../../hardware/images/board-overview.jpg)

## 1. Board identity

The examined sample is marked **ESP32-2432S028** on the PCB. It belongs to the family commonly known as the **Cheap Yellow Display (CYD)**: an ESP32 module, 2.8-inch TFT, resistive touch panel, microSD slot, USB-to-UART interface, audio amplifier, and several expansion connectors integrated on one yellow PCB.

This document deliberately describes the **specific photographed sample**. Closely related boards are sold as `ESP32-2432S028`, `ESP32-2432S028R`, or under generic CYD names, and component choices or pin routing may differ between revisions and batches.

## 2. Identification method and confidence levels

The component list below is based on three evidence classes:

- **Photo verified** — marking is readable on the author's board.
- **Board-marking verified** — function or signal name is printed on the PCB.
- **Typical / pending verification** — common for this board family, but not yet confirmed by direct measurement or controller-ID readout on this sample.

No hidden connection is treated as proven merely because it appears in a third-party pinout.

## 3. Main hardware at a glance

| Subsystem | Component or implementation | Evidence | Confidence |
|---|---|---|---|
| Main MCU and radio | **ESP-WROOM-32** module | marking visible in macro photo | High |
| Display | 2.8-inch, 240 × 320 class TFT module | physical module and board model | High for size/resolution class; controller pending |
| Touchscreen | resistive panel with **XPT2046** controller | `XPT2046` marking on U3 | High |
| Auxiliary serial Flash | **Winbond W25Q32JV**, 32 Mbit / 4 MB | package marking on U4 | High |
| USB-to-UART | **WCH CH340G** | package marking on U6 | High |
| Audio amplifier | **8002A** mono BTL amplifier | package marking on U5 and adjacent `SPK` connector | High |
| Voltage regulation | two **AMS1117-3.3** LDO regulators | markings on U1 and U7 | High |
| Removable storage | microSD / TF-card socket | visible socket | High |
| User controls | BOOT and RESET buttons | PCB labels and visible buttons | High |
| Expansion | UART, GPIO and speaker connectors | PCB reference designators and signal labels | High for connector purpose; exact connector manufacturer pending |

## 4. Physical-side overview

### 4.1 PCB side

The component side contains nearly all service electronics:

- ESP-WROOM-32 module and antenna area;
- CH340G USB-to-UART converter;
- automatic BOOT/RESET transistor network;
- XPT2046 resistive-touch controller;
- W25Q32JV auxiliary SPI Flash;
- 8002A audio power amplifier;
- two AMS1117-3.3 regulators;
- microSD socket;
- USB connector;
- UART, GPIO and speaker connectors;
- BOOT and RESET buttons;
- RGB LED, light sensor and supporting passives found on common CYD revisions.

The ESP32 antenna end must remain clear of metal, wiring bundles and enclosure walls whenever possible.

### 4.2 Display side

The opposite side is occupied by the 2.8-inch TFT and its resistive touch overlay. The LCD controller is not directly visible in the available photographs; it may be implemented on the display flex or beneath the display assembly.

**Important:** the common ILI9341 assumption is useful for initial software setup, but it remains a **revision-dependent working hypothesis** until confirmed on this sample by successful initialization, controller-ID readout, or display-flex documentation.

## 5. Functional block view

```text
USB 5 V
  |
  +--> CH340G USB-UART ----> ESP32 UART0 / boot-reset network
  |
  +--> 3.3 V regulation ---> ESP-WROOM-32 and low-voltage peripherals
                              |
                              +--> TFT interface and backlight control
                              +--> XPT2046 resistive touch controller
                              +--> microSD interface
                              +--> W25Q32JV auxiliary SPI Flash
                              +--> RGB LED / LDR / buttons
                              +--> audio signal ---> 8002A ---> SPK connector
                              +--> UART and GPIO expansion connectors
```

This is a subsystem-level diagram, not a reconstructed schematic. Exact rail partitioning between the two regulators and exact shared-bus routing remain to be verified.

## 6. Photo-verified component summary

### ESP-WROOM-32

The main module integrates the ESP32 SoC, RF circuitry, antenna and the module's own program Flash. The visible certification marking identifies the classic ESP-WROOM-32 family rather than ESP32-S2, S3, C3 or C6.

### XPT2046 (U3)

A 12-bit resistive-touch controller communicating over SPI. It measures X/Y position and touch pressure-related channels. It controls the touch panel only, not the TFT image interface.

### Winbond W25Q32JV (U4)

A 32-Mbit serial Flash device, equivalent to 4 MB. Because ESP-WROOM-32 already contains module Flash, U4 is treated as an auxiliary memory device. Its actual firmware use and GPIO routing require software or continuity verification.

### 8002A (U5)

A small mono bridge-tied-load audio amplifier feeding the two-pin `SPK` connector. A speaker must be connected between the two amplifier outputs; neither speaker lead should be tied to board ground.

### CH340G (U6)

The USB-to-UART bridge used for programming and serial monitoring. Nearby transistors and resistors are consistent with the usual automatic ESP32 BOOT/RESET circuit.

### AMS1117-3.3 (U1 and U7)

Two 3.3-V linear regulators are present. Their exact load division is not yet proven. The dual-regulator arrangement may separate major board loads, but this must be established by net tracing or measurements before being documented as fact.

## 7. External interfaces visible on this sample

| Reference | Purpose | Visible / documented signals | Notes |
|---|---|---|---|
| USB | power, programming, serial monitor | 5 V USB and CH340G data path | main development connection |
| P1 | service UART | TX, RX and supply/ground according to PCB labeling and revision documentation | duplicates or exposes ESP32 UART0 on common revisions |
| P3 | GPIO expansion | `GND`, `IO35`, `IO22`, `IO21` visible on the examined board | GPIO35 is input-only; GPIO21 may also control the backlight |
| CN1 | auxiliary GPIO/power | `GND`, `NC`, `IO27`, `3.3` visible on the examined board | this photographed sample has an `NC` position |
| P4 | speaker output | two-wire BTL output | do not connect either pin to GND |
| microSD | removable storage | SPI-connected card socket on common revisions | exact bus assignment is documented separately in pinout notes |

### Connector family status

The board connectors are 1.25-mm-pitch wire-to-board parts with side retention features. Their geometry closely resembles **Scondar SCT1251 / Molex PicoBlade-compatible** products. This is currently a procurement candidate, not a manufacturer-confirmed BOM entry. Mechanical fit should be checked before ordering large quantities.

## 8. Revision-specific observations

The photographed sample differs from some online CYD descriptions in details that matter:

1. The PCB is marked `ESP32-2432S028` without relying on an assumed `R` suffix.
2. U6 is visibly **CH340G**; documentation that says CH340C describes another production variant or a generic CYD configuration.
3. CN1 visibly includes an **NC** contact position, while some later diagrams route another GPIO there.
4. Two AMS1117-3.3 regulators are fitted.
5. The TFT controller has not been physically identified from the available photographs.

Software and wiring examples should therefore state which revision they target.

## 9. Electrical cautions

- ESP32 GPIOs are **3.3-V logic** and are not generally 5-V tolerant.
- GPIO34, GPIO35, GPIO36 and GPIO39 are input-only on the classic ESP32.
- GPIO0 is a boot-strapping pin; holding it low during reset enters the ROM bootloader.
- The speaker output is BTL. Do not ground either speaker terminal.
- An AMS1117 can dissipate significant heat when dropping 5 V to 3.3 V at high current.
- Display backlight, Wi-Fi transmission, SD-card activity and audio peaks can create supply transients.
- Do not infer connector pin order from cable color; verify orientation against PCB silkscreen.

## 10. What is established and what remains open

### Established from the sample

- board model marking;
- main ESP32 module family;
- touch, Flash, USB-UART, audio-amplifier and regulator part markings;
- physical presence of TFT, touch overlay, microSD and external connectors;
- connector labels visible on the PCB.

### Still open

- exact TFT controller and display-init variant;
- complete netlist and bus sharing;
- rail partitioning between U1 and U7;
- auxiliary W25Q32 GPIO assignment and intended use;
- exact connector manufacturer and mating-part article numbers;
- measured current consumption and thermal limits;
- speaker-output power on this particular PCB;
- differences between this sample and other ESP32-2432S028/R batches.

## 11. Recommended next documentation steps

1. Build a photo-indexed **PCB Component Atlas**.
2. Trace and verify all connector pins.
3. Read or experimentally identify the TFT controller.
4. Measure 5-V and 3.3-V rails under display, Wi-Fi, SD and audio load.
5. Confirm the auxiliary Flash bus in software or with continuity measurements.
6. Record connector dimensions and test candidate mating cables.

## 12. Reference sources

The sample photographs are the primary source for all component-identification claims in this chapter. General board-family information should be cross-checked against:

- Espressif ESP-WROOM-32 documentation;
- WCH CH340 documentation;
- Winbond W25Q32JV documentation;
- XPT2046-compatible controller documentation;
- the board documentation maintained by Apache NuttX;
- community pinout references, treated as revision-dependent secondary sources.

Third-party sources are supporting references only; where they conflict with the photographed board, the sample-specific observation takes precedence.
