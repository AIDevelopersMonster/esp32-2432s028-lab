# Power System — ESP32-2432S028

> **Document status:** sample-specific power-system reference for the author's `ESP32-2432S028` board.  
> **Evidence rule:** visible markings and PCB labels are treated as primary evidence; hidden rail routing remains provisional until measured or continuity-tested.  
> **Related chapters:** [Hardware Overview](01-hardware-overview.md), [PCB Component Atlas](02-pcb-component-atlas.md), [PCB Walkthrough](03-pcb-walkthrough.md), and [Connector Reference](04-connector-reference.md).

![Power regulators on the examined board](../../hardware/images/power-regulators.jpg)

## 1. Scope

This chapter documents the board's power-entry points, visible regulation hardware, likely load domains, thermal limits, transient risks, back-powering hazards, and a repeatable measurement plan.

It does **not** claim a complete schematic. In particular, the exact load split between the two AMS1117-3.3 regulators is still open.

## 2. Confirmed power-related hardware

| Area | Visible evidence | Confirmed conclusion |
|---|---|---|
| USB connector | physical connector and CH340G programming path | normal 5-V input and USB data entry |
| U1 | `AMS1117 3.3` marking | 3.3-V linear regulator |
| U7 | `AMS1117 3.3` marking | second 3.3-V linear regulator |
| P1 | silkscreen includes `5V` | connector exposes a 5-V-class power node |
| CN1 | silkscreen includes `3.3` | connector exposes a 3.3-V node |
| P4 / U5 | `8002A` amplifier and speaker connector | audio stage is a potentially significant dynamic load |
| ESP-WROOM-32 | module marking | logic and radio subsystem operates from a 3.3-V-class rail |

## 3. Working block diagram

```text
USB 5 V
  |
  +--> CH340G and USB-side circuitry
  |
  +--> U1 AMS1117-3.3 --> 3.3-V load domain A
  |
  +--> U7 AMS1117-3.3 --> 3.3-V load domain B
  |
  +--> possible direct 5-V loads
  |
  +--> P1 5-V pin

CN1 3.3-V pin --> exposed regulated rail, exact source still to be mapped
```

This is a functional hypothesis. It must not be converted into a schematic claim until the rails are traced.

## 4. USB power entry

The USB connector is the normal development power source.

### 4.1 What USB provides

- approximately 5 V from the host or charger;
- USB data to CH340G;
- a common ground reference;
- possible current limiting from the host port, cable, hub, or charger.

### 4.2 Cable effects

A poor USB cable can cause:

- brownouts during Wi-Fi transmit bursts;
- unstable display initialization;
- SD-card errors;
- random resets during audio playback;
- firmware-upload failures;
- excessive voltage drop despite a nominal 5-V supply.

During testing, record voltage both at the source and directly on the board.

## 5. Dual AMS1117-3.3 regulators

The examined sample has two regulators marked `AMS1117-3.3`.

### 5.1 What is known

- both are nominal 3.3-V linear regulators;
- both dissipate the voltage difference between input and output as heat;
- both require suitable input/output capacitors;
- both may be sensitive to high load, poor cooling, low input voltage, or unstable replacement capacitors.

### 5.2 What remains open

- which regulator powers the ESP32 module;
- which regulator powers the display, touch, storage, or other peripherals;
- whether the rails are fully separate;
- whether the outputs are connected through zero-ohm links, ferrites, or jumpers;
- whether one regulator supplies a 5-V-domain load indirectly;
- which regulator feeds CN1's `3.3` pin.

### 5.3 Do not assume the rail split

Common statements such as “one regulator is for the ESP32 and the other is for the display” are plausible but not yet proven for this sample.

## 6. Linear-regulator dissipation

For each regulator:

```text
PLOSS = (VIN - VOUT) × IOUT
```

At 5.0 V input and 3.3 V output:

| Output current | Approximate regulator loss |
|---:|---:|
| 50 mA | 0.085 W |
| 100 mA | 0.17 W |
| 200 mA | 0.34 W |
| 300 mA | 0.51 W |
| 500 mA | 0.85 W |

These are electrical losses only. Actual safe current depends on package thermal resistance, copper area, ambient temperature, enclosure airflow, component quality, and input voltage.

## 7. Why the printed current rating is not the board rating

A regulator family may advertise a high maximum current under ideal conditions. That does not mean the assembled board can continuously deliver that current.

The practical limit may be lower because of:

- small copper heat-spreading area;
- high ambient temperature;
- enclosed plastic case;
- poor-quality regulator clone;
- shared loading by Wi-Fi, TFT, touch, SD, Flash, and LEDs;
- input voltage drop;
- inadequate decoupling;
- backlight or audio peaks.

Therefore, this reference does not assign a guaranteed current capacity without measurement.

## 8. Principal load domains

### 8.1 ESP-WROOM-32

The ESP32 creates short current bursts, especially during radio transmission. Average current alone is not sufficient to judge supply quality.

Symptoms of a weak rail include:

- brownout detector resets;
- boot loops;
- Wi-Fi connection failures;
- resets when starting Bluetooth;
- serial corruption caused by repeated restarts.

### 8.2 TFT and backlight

The display system contains at least two distinct load types:

- display-controller logic;
- LED backlight.

The backlight may dominate steady display-related current. Its exact supply and switching topology remain open.

### 8.3 XPT2046 touch controller

Touch-controller current is normally modest compared with Wi-Fi or backlight current, but poor decoupling or bus contention can still cause functional failures that resemble power faults.

### 8.4 microSD card

SD cards can draw sharp current peaks during initialization and writes. Different cards can behave differently on the same board.

### 8.5 Auxiliary W25Q32JV

The external serial Flash is a smaller load, but erase and program operations can create measurable transient demand.

### 8.6 8002A audio amplifier

The audio stage can produce rapid load variation. Current depends on:

- amplifier supply voltage;
- speaker impedance;
- signal amplitude;
- clipping;
- duty cycle;
- efficiency and device variant.

Do not evaluate the power system using an unloaded or silent amplifier if the final application includes audio.

### 8.7 RGB LED and external loads

RGB LED channels and devices attached to P1, P3, or CN1 add to the board budget. External loads must not be treated as free capacity.

## 9. 5-V and 3.3-V connector pins

### 9.1 P1 `5V`

The `5V` label indicates a 5-V-class node, but the following must still be established:

- whether it is directly connected to USB VBUS;
- whether a diode, fuse, resistor, or switch is in series;
- whether external 5 V can safely power the whole board;
- whether external 5 V back-feeds the USB connector.

### 9.2 CN1 `3.3`

The `3.3` pin exposes a regulated rail. Before using it as a supply output, determine:

- which regulator feeds it;
- available current under worst-case onboard load;
- voltage droop during Wi-Fi, SD, and audio activity;
- whether external injection back-powers either regulator;
- whether the board remains safe when USB is simultaneously connected.

## 10. Back-powering hazards

Back-powering occurs when voltage enters a rail from an unexpected direction.

Possible paths include:

- external 5 V on P1 while USB is connected;
- external 3.3 V on CN1 while onboard regulators are unpowered;
- GPIO signals driven high while the board is off;
- USB-UART or peripheral modules supplying current through protection diodes;
- external audio or sensor boards sharing power incorrectly.

### Safe rule

Use one deliberate primary power source until reverse-current behavior is measured.

### Before dual-source operation

Check:

1. voltage difference between sources;
2. current direction;
3. regulator reverse-current tolerance;
4. USB-host exposure;
5. ground continuity;
6. startup and shutdown order.

## 11. GPIO injection while unpowered

A powered external device connected to an unpowered ESP32 GPIO can inject current into the 3.3-V rail through internal structures.

Consequences may include:

- partial board powering;
- unstable boot;
- excessive pin current;
- regulator reverse bias;
- latch-up or device damage.

Use series resistors, level translators, or coordinated power sequencing where required.

## 12. Brownout and transient behavior

A multimeter may show a normal average voltage while fast dips still reset the ESP32.

### Useful instruments

- oscilloscope with a short ground spring;
- differential probe where needed;
- current-limited bench supply;
- USB power meter for coarse measurements;
- thermal camera or contact thermocouple;
- current probe or small shunt resistor.

### Recommended observation points

- USB 5 V at the board;
- input and output of U1;
- input and output of U7;
- ESP32 3.3-V rail near the module;
- CN1 3.3-V pin;
- amplifier supply rail;
- backlight supply or control node.

## 13. Ground and probing discipline

The board has a common logic ground, but P4 speaker output is differential.

### Safe probing

- logic and regulator rails may be measured relative to board ground;
- neither P4 lead should be clipped to oscilloscope earth;
- use a differential probe or two-channel subtraction for BTL audio;
- keep oscilloscope ground leads short when observing fast transients;
- avoid accidental shorts around the dense regulator and connector areas.

## 14. Suggested measurement matrix

Record at least the following operating modes:

| Test mode | Display | Wi-Fi | SD | Audio | External load |
|---|---|---|---|---|---|
| idle boot | normal | off | none | silent | none |
| full backlight | full | off | none | silent | none |
| radio load | normal | transmit | none | silent | none |
| SD read | normal | off | read | silent | none |
| SD write | normal | off | write | silent | none |
| audio 8 Ω | normal | off | none | active | none |
| audio 4 Ω | normal | off | none | active | none |
| combined load | full | transmit | write | active | none |
| expansion load | normal | transmit | none | silent | documented load |

For each mode record:

- USB voltage at the board;
- total input current;
- U1 output voltage;
- U7 output voltage;
- regulator temperatures;
- reset or error behavior;
- test duration;
- ambient temperature.

## 15. Rail-mapping procedure

### Phase A — unpowered continuity

1. disconnect USB and all peripherals;
2. verify capacitors have discharged;
3. identify regulator ground pins;
4. map U1 and U7 outputs to nearby capacitors;
5. test continuity to ESP32 3.3-V pins, CN1 3.3 V, XPT2046, Flash, and amplifier supply;
6. record low-resistance paths without assuming every near-zero value means a direct copper connection.

### Phase B — powered voltage mapping

1. use a current-limited 5-V source;
2. measure both regulator inputs and outputs;
3. compare rails during boot and steady state;
4. switch major loads one at a time;
5. note which rail responds to each load.

### Phase C — confirmation

A rail assignment should be marked confirmed only when at least one strong method supports it:

- direct continuity;
- clear voltage response under selective load;
- schematic or BOM for the exact revision;
- trace visibility in high-resolution imagery;
- controlled component isolation.

## 16. Current-budget method

Do not use a single guessed “board current.” Build the budget by operating state.

```text
ITOTAL = IESP32 + IDISPLAY + IBACKLIGHT + ITOUCH + ISD + IFLASH + IAUDIO + ILED + IEXTERNAL
```

Use measured values and include transient margin.

A practical engineering reserve should account for:

- component spread;
- low USB voltage;
- warm enclosure;
- radio bursts;
- SD-card variation;
- speaker impedance tolerance;
- future external modules.

## 17. Thermal test procedure

1. place the board in the intended orientation;
2. use the intended enclosure or clearly mark open-bench results;
3. run the highest realistic combined load;
4. log temperatures at U1, U7, ESP-WROOM-32, U5, and nearby capacitors;
5. continue until temperature stabilizes;
6. repeat at the highest expected ambient temperature;
7. inspect for display artifacts, resets, audio distortion, or rail sag.

Do not rely on a brief fingertip test as the only thermal assessment.

## 18. Powering from an external bench supply

Recommended initial setup:

- 5.0 V nominal;
- conservative current limit;
- short, low-resistance leads;
- common ground;
- USB data disconnected unless the back-feed path is understood.

Increase the current limit only after observing normal startup behavior.

## 19. Powering from 3.3 V

Direct 3.3-V injection may bypass onboard regulation, reverse-bias regulators, or leave 5-V peripherals unpowered.

Therefore it should be treated as an advanced test mode, not a default powering method.

Before attempting it, establish:

- exact rail topology;
- required 5-V loads;
- reverse-current behavior of both regulators;
- CH340G behavior without VBUS;
- whether audio and backlight need 5 V.

## 20. Battery operation

No battery-management circuit has been confirmed on this sample.

A battery-powered design may require:

- charger;
- protection circuit;
- regulated 5-V or 3.3-V conversion;
- undervoltage cutoff;
- safe USB/battery source selection;
- transient capability for Wi-Fi and audio.

Do not connect a lithium cell directly to a rail without a complete power design.

## 21. Common power-related symptoms

### Board does not start

Check USB cable, 5-V entry, regulator inputs, both 3.3-V outputs, EN state, and for shorts.

### Reboots when Wi-Fi starts

Suspect cable drop, weak regulator rail, insufficient decoupling, current limiting, or thermal overload.

### SD works intermittently

Check 3.3-V dips, card current peaks, contact quality, SPI clock, and ground integrity.

### Audio causes resets

Check amplifier supply, speaker impedance, clipping, total input current, regulator temperature, and shared-rail transients.

### Display dims or flickers

Check USB voltage, backlight supply, backlight-control signal, connector integrity, and regulator load.

### Works on bench but fails in enclosure

Check regulator temperature, antenna obstruction, speaker magnet placement, cable resistance, and airflow.

## 22. Evidence and result table

Use this table when measurements are completed:

| Item | Result | Method | Conditions | Confidence |
|---|---|---|---|---|
| U1 input source | pending | continuity / voltage | board unpowered and powered | Open |
| U1 load domain | pending | selective-load response | documented firmware | Open |
| U7 input source | pending | continuity / voltage | board unpowered and powered | Open |
| U7 load domain | pending | selective-load response | documented firmware | Open |
| CN1 3.3-V source | pending | continuity | USB disconnected | Open |
| P1 5-V relation to USB | pending | continuity / diode test | USB disconnected | Open |
| reverse-current behavior | pending | current measurement | controlled dual-source test | Open |
| maximum combined load | pending | thermal/electrical test | specified ambient | Open |

## 23. Safe interim operating guidance

Until the rail map is complete:

- power primarily through USB;
- use a good-quality short cable;
- avoid simultaneous external power injection;
- keep external 3.3-V loads modest;
- do not assume P1 or CN1 can supply a particular current;
- use 8-Ω audio loads for conservative initial tests;
- monitor regulator temperature during combined load;
- preserve brownout protection during development;
- document every external load.

## 24. Open questions

- exact U1/U7 rail partitioning;
- direct or protected relationship between P1 5 V and USB VBUS;
- source regulator for CN1 3.3 V;
- backlight supply voltage and switching topology;
- amplifier supply voltage;
- regulator manufacturer authenticity and thermal performance;
- output-capacitor values and types;
- reverse-current paths;
- measured board current in defined modes;
- safe continuous external-load budget.

## 25. Completion criteria for the power chapter

This chapter can move from provisional to measured-reference status when the repository contains:

1. rail-continuity map;
2. labeled voltage test points;
3. U1/U7 load-domain identification;
4. input-current table by operating mode;
5. oscilloscope captures of worst transients;
6. regulator thermal results;
7. back-powering test results;
8. explicit safe external-load guidance.

## 26. Conclusion

The photographed ESP32-2432S028 sample has a more substantial power architecture than a single-regulator development board: USB 5-V entry, two visible AMS1117-3.3 regulators, exposed 5-V and 3.3-V connector nodes, and several bursty loads.

The visible hardware is sufficient to define a safe measurement program, but not to claim the final rail split or current limits. Until those measurements are completed, conservative USB powering, avoidance of dual-source injection, and explicit thermal/transient testing are the correct engineering approach.
