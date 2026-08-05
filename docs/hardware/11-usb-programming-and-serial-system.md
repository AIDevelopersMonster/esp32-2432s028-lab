# USB Programming and Serial System — ESP32-2432S028

> **Document status:** sample-specific USB programming and serial reference for the author's `ESP32-2432S028` board.  
> **Scope:** USB connector, CH340G bridge, ESP32 UART0 path, automatic and manual bootloader entry, upload timing, baud-rate qualification, P1 service UART, external-adapter interaction, diagnostics, and verification.  
> **Evidence rule:** visible markings and repository settings are recorded as facts; hidden copper routes, transistor topology, resistor values, protection parts, and signal integrity limits remain open until measured.

![CH340G macro photograph](../../hardware/images/ch340g.jpg)

## 1. Purpose

The USB and serial subsystem performs several jobs that are easy to confuse:

1. it carries USB data between a host computer and the board;
2. it converts USB data into asynchronous UART data;
3. it supplies the normal 5-V board input when powered from USB;
4. it can place the ESP32 into the ROM serial bootloader automatically;
5. it carries application serial logs after firmware starts;
6. it shares the UART0 path with the P1 service connector.

A board can therefore:

- enumerate on USB but fail to upload;
- upload correctly but show no application logs;
- print logs but fail automatic bootloader entry;
- work from USB while failing through P1;
- fail at high upload speed while remaining electrically functional.

Troubleshooting should separate these layers rather than treating them as one generic “USB problem.”

## 2. Evidence summary

| Item | Current status | Evidence | Still open |
|---|---|---|---|
| USB connector physically present | Confirmed | board photograph | exact manufacturer, connector article, protection network |
| CH340G at U6 | Confirmed | readable WCH `CH340G` marking | exact surrounding passives and clock implementation |
| ESP32 UART programming function | Function confirmed | board architecture, working repository workflow | exact continuity map and any series resistors |
| T1/T2 auto-programming pair | Likely | placement beside U6 and ESP32 control path | transistor types, resistor values, exact logic topology |
| BOOT switch to GPIO0 | Strong implementation baseline | board label and repository use | exact pull-up resistance and full net continuity |
| RESET switch to EN | Strong implementation baseline | board placement and board-family function | EN RC values and exact switch routing |
| P1 labels `GND | RX | TX | 5V` | Confirmed | visible silkscreen | manufacturer pin numbering and direct VBUS relationship |
| PlatformIO upload speed 921600 | Confirmed software setting | `platformio.ini` | reliable maximum for this board, cable, host and environment |
| Serial monitor speed 115200 | Confirmed software setting | `platformio.ini` and examples | application-specific baud changes |

## 3. System block diagram

The current working model is:

```text
Host computer
    |
USB cable
    |
USB connector
    |---------------- 5 V ----------------> board power system
    |
    `-- D+ / D- --> CH340G U6
                       |-- UART TXD/RXD --> ESP32 UART0
                       |-- modem controls --> T1/T2 network
                                                |-- EN
                                                `-- GPIO0

ESP32 UART0 <------------------------------> P1 service connector
                                              GND | RX | TX | 5V
```

This diagram represents the expected function. It does not prove every hidden trace or resistor.

## 4. USB connector

### 4.1 Confirmed role

The USB connector is the normal development interface for:

- board power;
- firmware upload;
- serial monitoring;
- application communication with a host.

### 4.2 Data and power are separate paths

A USB cable contains at least two conceptually different functions:

```text
VBUS and GND  -> power
D+ and D-     -> data
```

A charge-only cable may power the display and board while providing no serial device to the computer.

The observation “the board lights up” therefore does not prove that the cable carries data.

### 4.3 Connector-mechanical checks

Inspect for:

- looseness between shell and PCB;
- cracked solder joints on shell tabs;
- lifted data pins;
- debris or oxidation;
- cable plugs that move excessively;
- intermittent enumeration when the cable is touched.

Repeated upload failures caused by connector movement should not be diagnosed first as firmware faults.

### 4.4 USB power caution

The USB connector can power multiple dynamic loads:

- ESP32 boot and Wi-Fi bursts;
- TFT and backlight;
- microSD activity;
- RGB LED;
- audio amplifier and speaker;
- external loads connected to board rails.

A weak cable, unpowered hub, overloaded host port, or high-resistance connector can produce:

- brownout resets;
- USB disconnects;
- upload corruption;
- resets at the start of Wi-Fi transmission;
- resets while erasing or writing Flash;
- serial garbage caused by repeated reboots.

Do not assume that every upload error is a logic-level or driver problem.

## 5. CH340G — U6

### 5.1 Identification

The photographed sample is fitted with a WCH **CH340G** at U6.

This suffix matters because some generic CYD documentation calls the device `CH340C`. The software driver may cover the broader CH340 family, but the hardware reference for this sample must retain the observed `CH340G` identification.

### 5.2 Board-level functions

The bridge provides:

- USB enumeration;
- host-to-board serial transmission;
- board-to-host serial reception;
- modem-control outputs used by the probable auto-programming network;
- a persistent UART path for application logs.

### 5.3 What is not yet claimed

The current photographs do not establish:

- the exact crystal or clock network used by U6;
- values of USB series resistors;
- presence and type of ESD suppression;
- CH340G I/O supply voltage on this sample;
- exact connection of every modem-control output;
- whether UART lines include series resistors or test pads;
- the maximum error-free baud rate.

### 5.4 Host enumeration record

For each operating system tested, record:

```text
Host OS and version:
Driver source/version:
USB device name:
VID/PID as reported:
Assigned serial port:
Enumeration time:
Reconnect behavior:
Suspend/resume behavior:
Result:
```

Do not hard-code one COM number or `/dev` path into documentation. Port assignment can change between hosts and reconnects.

## 6. USB driver layer

### 6.1 Driver symptom separation

A missing serial port can result from:

- no USB data connection;
- missing or failed driver;
- damaged USB connector;
- failed CH340G power or clock;
- operating-system permission rules;
- another process holding the device;
- cable or hub instability.

### 6.2 Safe driver practice

Use the operating system's trusted driver path or the semiconductor manufacturer's driver package. Avoid random third-party driver archives.

After driver installation or update:

1. disconnect the board;
2. reboot if required by the operating system;
3. reconnect directly to the host;
4. verify device enumeration;
5. record the assigned port;
6. test at 115200 before using a higher upload speed.

### 6.3 Port permissions

On systems where serial ports require group membership or device permissions, a visible port may still fail to open.

Distinguish:

```text
port does not exist
port exists but permission is denied
port opens but no ESP32 response is received
```

These are different faults.

## 7. UART0 path

### 7.1 Functional role

The classic ESP32 uses UART0 for its standard ROM download protocol and boot messages. After the application starts, the same UART is commonly used by Arduino `Serial`.

### 7.2 Current software baseline

The repository examples normally initialize:

```cpp
Serial.begin(115200);
```

PlatformIO currently configures:

```ini
monitor_speed = 115200
upload_speed = 921600
```

These are software settings, not proof that every host, cable and board will work reliably at the higher upload speed.

### 7.3 Direction naming

Signal names must always be interpreted from the device being described.

Conceptually:

```text
CH340G TXD -> ESP32 RXD
CH340G RXD <- ESP32 TXD
```

At P1:

```text
board TX -> external adapter RX
board RX <- external adapter TX
```

The P1 silkscreen labels are board-side signal names.

### 7.4 Logic standard

The service UART is 3.3-V logic.

It is not:

- RS-232 voltage level;
- RS-485;
- 5-V TTL simply because P1 also carries a `5V` contact;
- USB signalling.

Never connect a true RS-232 transmitter directly to the ESP32 UART pins.

## 8. P1 service connector

![Connector area](../../hardware/images/connectors.jpg)

### 8.1 Visible signal group

The documented sample shows:

```text
GND | RX | TX | 5V
```

This is visible silkscreen order, not a universal connector-manufacturer pin-number convention.

### 8.2 Conservative first connection

When the board is already powered through USB, connect only:

```text
P1 GND     <-> adapter GND
P1 TX      -> adapter RX
P1 RX      <- adapter TX
```

Leave P1 `5V` disconnected.

### 8.3 Why the 5-V contact should initially remain open

Connecting USB power and an external adapter's 5-V output simultaneously can create:

- power-source contention;
- reverse current into a host or adapter;
- back-powering through an unknown protection path;
- different ground-reference startup timing;
- misleading voltage readings.

The exact relationship between P1 `5V` and USB VBUS remains open until continuity, diode-drop and powered measurements are recorded.

### 8.4 External adapter voltage

The external adapter TX output must be 3.3-V compatible.

A board labelled “5 V” or a jumper-configurable adapter must be checked with a meter or oscilloscope before connection.

### 8.5 P1 does not expose a complete automatic-programming interface

The visible P1 labels include UART and power only. They do not expose dedicated DTR, RTS, EN or GPIO0 contacts.

Therefore an ordinary three-wire adapter connection normally provides:

- serial logging;
- application UART communication;
- ROM bootloader data after manual bootloader entry;

but not automatic BOOT/RESET control unless additional verified connections are made elsewhere.

### 8.6 CH340G contention risk

The onboard CH340G remains electrically connected to UART0 unless the board contains unverified isolation resistors, jumpers or tri-state behavior sufficient to prevent contention.

Do not simultaneously drive board `RX` from:

- the onboard CH340G TX output; and
- an external adapter TX output.

Likewise, two host-side serial programs should not assume exclusive ownership of the same UART path.

A safe P1 test should use one active transmitter at a time.

## 9. ESP32 boot modes relevant to programming

### 9.1 Normal boot

For a normal application boot, GPIO0 must not be held in the ROM-download selection state when EN rises.

### 9.2 Serial ROM bootloader

The practical manual sequence is:

1. hold BOOT;
2. assert RESET or pull EN low;
3. release RESET so EN rises;
4. release BOOT after the boot mode has been sampled;
5. start or continue the upload tool.

Tool timing varies, so the exact moment to release BOOT may be adjusted during troubleshooting.

### 9.3 BOOT is not RESET

BOOT changes the level sampled for boot-mode selection.

RESET acts through the ESP32 enable path and restarts the chip.

Pressing BOOT alone during normal execution does not necessarily reset the board.

### 9.4 Strapping sensitivity

External circuits attached to boot-related pins can alter startup mode.

For the USB programming subsystem, the principal concerns include:

- GPIO0 held low unintentionally;
- EN held low or rising too slowly;
- external circuits driving UART lines during ROM synchronization;
- other board strapping pins forced to invalid levels by peripherals.

## 10. Automatic reset and automatic bootloader entry

### 10.1 Visible evidence

Two small transistor packages, T1 and T2, are located near U6 with associated resistors. Their location is consistent with an automatic ESP32 programming circuit.

### 10.2 Likely control model

The expected function is:

```text
CH340G modem-control outputs
        |
        v
transistor / resistor logic
        |--------------> ESP32 EN
        `--------------> ESP32 GPIO0
```

Upload software toggles serial control signals so that:

1. GPIO0 enters the bootloader-selected state;
2. EN is pulsed low;
3. EN is released;
4. GPIO0 is released;
5. the ROM downloader answers on UART0.

### 10.3 Important uncertainty

The exact T1/T2 circuit has not yet been continuity-traced.

Do not publish as fact:

- transistor part numbers;
- NPN versus PNP assignment;
- exact DTR/RTS mapping;
- resistor values;
- inversion truth table;
- pulse duration;
- whether a capacitor shapes EN;
- whether both controls are wired identically to a reference schematic.

### 10.4 Serial monitor side effect

Some serial-terminal programs assert or change DTR/RTS when opening the port. On a board with automatic programming circuitry, this can reset the ESP32.

A board that resets whenever the monitor opens may therefore be functioning as wired rather than suffering a random brownout.

To diagnose:

1. note whether reset occurs exactly when the port opens;
2. compare terminals with different DTR/RTS settings;
3. capture EN and GPIO0;
4. compare against power-rail behavior;
5. distinguish a control-line reset from a supply collapse.

## 11. Upload lifecycle

A typical upload contains several distinct phases.

### 11.1 Port opening

The tool opens the CH340 serial port and configures its initial baud and control lines.

Possible failures:

- port busy;
- permission denied;
- stale port name;
- device disconnected;
- terminal still open.

### 11.2 Bootloader entry

The tool attempts automatic reset and GPIO0 sequencing.

Possible failures:

- T1/T2 path not operating;
- DTR/RTS behavior incompatible;
- BOOT or RESET switch fault;
- external circuit loading GPIO0 or EN;
- poor power at reset.

### 11.3 ROM synchronization

The host sends synchronization traffic and waits for the ESP32 ROM downloader.

Possible failures:

- TX/RX crossed incorrectly;
- wrong voltage level;
- serial contention;
- ESP32 not in download mode;
- incorrect port;
- severe baud error;
- board held in reset.

### 11.4 Baud-rate transition

Upload tools commonly begin at a conservative rate and may then request a faster transfer rate.

A board can synchronize successfully yet fail after switching to a higher baud.

### 11.5 Flash erase and write

The ESP32 receives blocks and programs module Flash.

Failures in this phase may be caused by:

- unstable power;
- marginal high-speed serial transfer;
- wrong Flash configuration;
- write protection or Flash damage;
- host timeout;
- USB cable errors;
- thermal or current transients.

### 11.6 Verification and reset

After writing, the tool may verify data and reset into the application.

A successful write followed by no application output can indicate:

- wrong partition or image;
- application crash;
- wrong monitor baud;
- reset not released;
- boot strap still selecting download mode;
- TFT code blocking before the first log;
- serial port reopened with an unintended control-line reset.

## 12. Upload-speed qualification

### 12.1 Repository baseline

The repository currently uses:

```ini
upload_speed = 921600
```

This is a convenient target, not a guaranteed board rating.

### 12.2 Qualification ladder

Use a controlled progression such as:

```text
115200
230400
460800
921600
```

At each rate:

1. upload the same known image;
2. repeat at least 20 times;
3. record connection, erase, write and verification failures separately;
4. avoid moving the cable during the test;
5. repeat through the intended hub or extension only after direct-host testing;
6. repeat with TFT, Wi-Fi, SD and audio disabled, then enabled where relevant.

### 12.3 Acceptance rule

Use the highest rate that is repeatably error-free in the intended environment, not the highest rate that succeeds once.

A practical production or lab setting should include margin below the first rate at which intermittent failures appear.

### 12.4 Error-rate record

```text
Board sample:
Host:
Cable:
Hub/direct:
Upload tool/version:
Image size:
Upload speed:
Trials:
Connection failures:
Mid-write failures:
Verify failures:
Average time:
Result:
```

## 13. UART throughput

For a conventional 8-N-1 UART frame, each payload byte normally consumes ten serial bits:

```text
1 start + 8 data + 1 stop = 10 bits
```

The ideal payload ceiling is therefore approximately:

```text
bytes_per_second = baud / 10
```

| Baud | Ideal 8-N-1 payload ceiling |
|---:|---:|
| 115200 | 11,520 B/s |
| 230400 | 23,040 B/s |
| 460800 | 46,080 B/s |
| 921600 | 92,160 B/s |

Real application throughput is lower because of:

- protocol framing;
- host scheduling;
- buffering;
- USB latency;
- logging format;
- task scheduling;
- retransmissions or parser overhead.

Do not confuse upload baud with application serial throughput or USB bulk-transfer speed.

## 14. Serial monitor operation

### 14.1 Current baseline

Repository examples use 115200 baud.

A monitor must match the firmware's configured baud, framing and line-ending assumptions.

### 14.2 Boot messages and application messages

The monitor may show multiple phases:

1. ROM boot text;
2. bootloader output;
3. application logs;
4. reset loops if the application fails.

Capturing from the instant of reset is more informative than opening the monitor after the failure has already occurred.

### 14.3 Line endings

Interactive commands may expect:

- no terminator;
- line feed;
- carriage return;
- carriage return plus line feed.

A command that appears ignored may be waiting for a different terminator rather than suffering a UART hardware fault.

### 14.4 Excessive logging

High-volume synchronous logging can:

- block application tasks;
- alter timing;
- reduce TFT frame rate;
- disturb touch sampling;
- overflow host or device buffers;
- make a race condition disappear or appear.

Diagnostic builds should record the amount and timing of serial output.

## 15. External UART use while USB is connected

### 15.1 One receiver, multiple transmitters

Multiple listeners may sometimes observe one TX line, but multiple actively driven transmitters must not be connected together without a verified arbitration or isolation method.

### 15.2 Safe observation

To observe board TX externally while USB remains connected:

- connect external receiver RX to board TX;
- connect grounds;
- leave external TX disconnected;
- verify that the receiver input is high impedance and 3.3-V compatible.

### 15.3 Bidirectional communication

Before adding an external TX connection:

1. determine whether the onboard CH340G TX output is active or high impedance;
2. identify any series resistor;
3. ensure the USB host will not transmit simultaneously;
4. use a current-limited or series-resistor test approach if appropriate;
5. inspect both transmitters with an oscilloscope.

### 15.4 Alternative application UART

For long-term peripheral communication, a different free ESP32 UART mapped to verified GPIOs may be preferable to sharing UART0 with the programmer.

This avoids:

- upload contention;
- boot-message leakage into the peripheral;
- peripheral data entering the ROM downloader;
- resets when a host opens the programming port.

The external connector chapter must be consulted before assigning alternate pins.

## 16. Safe programming through P1

### 16.1 Required minimum signals

```text
GND
adapter TX -> board RX
adapter RX <- board TX
```

### 16.2 Manual entry sequence

With the adapter connected and the board safely powered:

1. hold BOOT;
2. press and release RESET;
3. release BOOT;
4. start the upload at 115200;
5. increase speed only after reliable operation is established.

### 16.3 Power choices

Preferred first test:

- board powered from USB;
- adapter signal side powered from its own USB connection;
- grounds connected;
- P1 `5V` left open.

Do not connect two independent 5-V outputs together without establishing their source/sink behavior.

### 16.4 Adapter selection

Required characteristics:

- 3.3-V UART-compatible TX level;
- reliable RX threshold for ESP32 output;
- explicit ground reference;
- documented driver support;
- no automatic 5-V injection through the signal header.

## 17. Continuity-mapping plan

Perform continuity checks with all power removed.

### 17.1 UART lines

Map:

```text
P1 TX -> ESP32 UART0 RX/TX candidate pads
P1 RX -> ESP32 UART0 RX/TX candidate pads
CH340G TXD -> ESP32 UART0 input path
CH340G RXD -> ESP32 UART0 output path
```

Use the actual CH340G and ESP-WROOM-32 pin documentation appropriate to the package and module when assigning pin numbers.

### 17.2 Control lines

Map:

```text
CH340G modem-control output A -> T1/T2 network
CH340G modem-control output B -> T1/T2 network
T1/T2 network -> EN
T1/T2 network -> GPIO0
BOOT switch -> GPIO0 path
RESET switch -> EN path
```

### 17.3 Power and ground

Map:

```text
USB VBUS -> board 5-V path
USB GND -> board ground
P1 5V -> USB VBUS or protected/intermediate node
CH340G supply -> regulator or USB-derived rail
```

Continuity alone cannot prove safe current direction or back-power behavior.

## 18. Oscilloscope and logic-analyzer plan

### 18.1 Recommended channels

Capture simultaneously where possible:

```text
CH1: EN
CH2: GPIO0
CH3: ESP32 UART0 RX
CH4: ESP32 UART0 TX
Digital: CH340 control outputs, if accessible
```

### 18.2 Automatic upload capture

Trigger on EN falling or rising and record:

- GPIO0 level before EN rises;
- EN pulse width;
- delay to first synchronization byte;
- UART idle levels;
- transition to high upload baud;
- reset after completion.

### 18.3 Manual upload capture

Repeat the same recording with BOOT and RESET buttons. Compare automatic and manual sequences.

### 18.4 UART electrical checks

Measure:

- idle voltage;
- low and high levels;
- rise and fall time;
- overshoot and ringing;
- noise during Wi-Fi transmission;
- behavior with USB and P1 adapters connected;
- whether either transmitter drives against another.

### 18.5 USB probing caution

USB D+ and D- are high-speed differential signals relative to ordinary UART work. Do not attach long ground leads or high-capacitance probes casually. For board-level diagnostics, enumeration and UART-side captures are usually safer first steps.

## 19. Power-integrity tests during upload

Monitor:

- USB VBUS at the connector;
- board 5-V node;
- both known 3.3-V regulator outputs;
- ESP32 EN;
- host USB disconnect events.

Test conditions:

1. display and backlight off;
2. backlight on;
3. microSD inserted;
4. Wi-Fi application running before reset;
5. speaker connected but audio disabled;
6. direct host port;
7. intended hub;
8. short cable;
9. intended long cable.

The upload process resets the ESP32, so application loads may disappear during flashing. The important events are power transients before reset, at reset, and during Flash operations.

## 20. Serial data-integrity test

A useful application test sends framed records containing:

```text
sequence number
timestamp
payload length
pseudorandom payload
CRC
```

The host verifies:

- missing frames;
- duplicate frames;
- CRC failures;
- reordered data;
- long latency gaps;
- reconnect behavior.

Run both directions separately before full duplex.

Test durations should include:

- short functional run;
- 10-minute run;
- one-hour run;
- overnight run if serial reliability is important to the intended project.

## 21. Failure diagnosis

### 21.1 Board powers but no serial port appears

Check in order:

1. data-capable cable;
2. direct host connection;
3. operating-system device list;
4. CH340 driver;
5. USB connector soldering;
6. U6 supply and clock;
7. USB D+/D- continuity;
8. ESD or series components if identified.

### 21.2 Port appears and disappears repeatedly

Possible causes:

- loose connector;
- weak cable;
- VBUS collapse;
- short or overcurrent;
- hub instability;
- CH340G reset or power loss;
- host power management.

Correlate host disconnect timestamps with VBUS and 3.3-V captures.

### 21.3 Port opens but uploader cannot connect

Check:

- correct port;
- terminal closed;
- manual BOOT/RESET sequence;
- GPIO0 level at reset;
- EN pulse;
- UART line crossing;
- external-device contention;
- 3.3-V logic level;
- 115200 upload speed.

### 21.4 Upload works only while BOOT is held

Likely areas:

- automatic reset control timing;
- T1/T2 network;
- DTR/RTS handling;
- GPIO0 pull-up or loading;
- EN pulse shape;
- upload-tool configuration.

Manual success proves more than “USB is good”: it suggests that USB enumeration, CH340 UART transfer and ROM protocol can operate, while automatic control remains suspect.

### 21.5 Upload starts but fails midway

Reduce upload speed and inspect:

- cable quality;
- host/hub;
- UART waveform;
- 3.3-V rail;
- Flash configuration;
- serial contention;
- thermal behavior;
- error repeatability at the same address.

A failure at random addresses suggests transport or power. A repeatable failure at one address may suggest Flash or image-layout issues.

### 21.6 Upload succeeds but application does not start

Check:

- GPIO0 released after upload;
- EN released;
- selected board and Flash settings;
- partition scheme;
- application crash logs;
- monitor baud;
- watchdog or brownout reset;
- wrong image or offset.

### 21.7 Serial output is unreadable

Check:

- monitor baud;
- framing settings;
- repeated resets;
- ground reference;
- 5-V adapter output accidentally connected to RX;
- two transmitters contending;
- binary data interpreted as text.

### 21.8 Opening the monitor resets the board

Investigate DTR/RTS behavior before diagnosing power instability.

Record EN and GPIO0 while:

- opening the monitor;
- closing the monitor;
- changing baud;
- reconnecting USB;
- using a different terminal program.

### 21.9 P1 receives data but cannot transmit to the board

Possible causes:

- TX/RX naming reversed;
- adapter TX at wrong voltage;
- CH340G TX contention;
- application not reading UART0;
- line-ending mismatch;
- damaged P1 contact;
- external adapter ground missing.

### 21.10 Upload reliability changes with Wi-Fi, display or audio hardware

Even though the application is reset during upload, peripheral hardware can affect:

- supply loading;
- boot straps;
- UART lines;
- EN/GPIO0;
- external ground noise.

Disconnect external hardware and restore it one subsystem at a time.

## 22. Minimum verification firmware

A simple serial test should:

1. initialize at 115200;
2. print a fixed boot banner;
3. print reset reason;
4. increment and print a sequence counter once per second;
5. echo received printable data with sequence and length;
6. avoid initializing TFT, touch, SD, Wi-Fi or audio initially;
7. later enable each subsystem separately.

Example output format:

```text
CYD USB/UART TEST
boot_count=1
seq=0 uptime_ms=1000
seq=1 uptime_ms=2000
rx_seq=1 length=5 data=hello
```

Do not include unique device identifiers in public logs unless needed and intentionally disclosed.

## 23. Test matrix

| Test | USB power | USB data | P1 adapter | Automatic reset | Expected result |
|---|---|---|---|---|---|
| normal PlatformIO upload | yes | yes | no | yes | repeated successful upload |
| manual USB upload | yes | yes | no | bypassed with BOOT/RESET | successful at conservative baud |
| USB serial monitor | yes | yes | no | terminal-dependent | stable 115200 logs |
| P1 receive-only observation | yes | optional | RX only | no | same board TX data observed |
| P1 bidirectional application UART | yes | preferably disconnected or controlled | yes | no | no contention, correct echo |
| P1 manual ROM upload | safe external power plan | optional | yes | manual | reliable at 115200 first |
| high-speed upload qualification | yes | yes | no | yes | zero errors across repeated trials |
| weak-cable comparison | yes | yes | no | yes | failures correlated and documented |
| monitor-open reset test | yes | yes | no | terminal-driven | EN/GPIO0 behavior captured |
| full subsystem stress | yes | yes | no | yes | upload and logging remain reliable |

## 24. Measurement record template

```text
Board identification:
PCB marking/revision:
Photograph set:
Date:
Operator:

USB connector type observed:
Cable:
Cable length:
Direct host or hub:
Host OS:
Driver:
Port identifier:

CH340 marking:
CH340 supply voltage:
UART idle high voltage:
UART low voltage:
Series resistor values, if measured:

Automatic reset:
EN pulse width:
GPIO0 level at EN rise:
Control-line mapping:
Manual upload result:
Automatic upload result:

Upload tests:
115200:
230400:
460800:
921600:
Selected qualified rate:

P1 mapping:
GND:
RX:
TX:
5V relationship:
External adapter used:
Contention observed:

Open questions:
Evidence files:
Conclusion:
```

## 25. Evidence levels for future updates

Use explicit labels:

- **Photo-confirmed** — readable marking, connector label or visible construction;
- **Repository-configured** — value used by current source code or build configuration;
- **Software-observed** — behavior reproduced by a test program;
- **Continuity-verified** — unpowered electrical path measured;
- **Waveform-verified** — timing and voltage captured;
- **Stress-qualified** — repeated operation under a declared test matrix;
- **Family-typical** — common ESP32/CH340 behavior not yet confirmed on this sample;
- **Open** — unresolved.

Do not upgrade a repository setting directly to a hardware-verified claim.

## 26. Claims that must not be made yet

Until measurements are recorded, do not claim:

- the exact USB connector manufacturer or protection circuit;
- exact CH340G support-component values;
- exact DTR/RTS-to-T1/T2 mapping;
- transistor types and inversion truth table;
- exact EN RC time constant;
- exact GPIO0 pull-up value;
- direct, unprotected equivalence of P1 `5V` and USB VBUS;
- guaranteed safe powering through P1;
- guaranteed 921600 upload reliability;
- a maximum UART baud rate;
- electrical isolation between CH340G and P1;
- that an external UART transmitter may safely operate in parallel with U6;
- that opening every serial monitor will or will not reset the board.

## 27. Completion criteria

This subsystem can be considered sample-verified when all of the following are recorded:

1. CH340G marking and USB connector photographs are archived;
2. U6 supply and UART logic levels are measured;
3. USB D+/D- path is identified sufficiently for fault diagnosis;
4. CH340G TXD/RXD to ESP32 UART0 continuity is established;
5. P1 RX/TX continuity and direction are established;
6. P1 `5V` relationship and safe-use limits are measured;
7. T1/T2 transistor types or at least full functional truth table are established;
8. DTR/RTS, EN and GPIO0 timing is captured;
9. automatic and manual bootloader entry are repeatable;
10. upload-speed qualification is completed;
11. serial-monitor reset behavior is characterized;
12. external P1 interaction is tested without line contention;
13. power integrity is verified during upload and reconnect;
14. one-hour bidirectional serial integrity test passes;
15. all evidence is linked from the verification record.

## 28. Open questions

- What exact USB connector article is fitted?
- Which USB protection and series components are populated?
- What supplies CH340G I/O on this revision?
- Which CH340G control outputs feed T1 and T2?
- What are the transistor types and resistor values?
- What is the EN RC network?
- Does opening the repository's preferred monitor assert reset control lines?
- Are UART0 lines direct or series-isolated?
- Is P1 `5V` directly connected to USB VBUS, diode-isolated, fused or otherwise protected?
- What is the highest repeatably error-free upload speed on the intended host and cable?
- Can the onboard CH340G and an external P1 adapter coexist safely in any receive-only or tri-stated configuration?
- Does USB suspend, host sleep or cable reconnect reset the application?

## 29. Summary

The photographed board uses a confirmed **CH340G**, not the `CH340C` named by some generic CYD instructions. The current repository uses a 115200 serial monitor and a 921600 upload target. P1 exposes the visible group `GND | RX | TX | 5V`, but it should initially be used with only ground and crossed 3.3-V UART signals.

The probable T1/T2 automatic programming network should be treated as a functional hypothesis until its control mapping, transistor topology and EN/GPIO0 timing are measured. Reliable operation must be established separately for USB enumeration, automatic bootloader entry, UART transfer, Flash writing, application startup and serial monitoring.