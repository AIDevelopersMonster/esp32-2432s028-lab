# Expansion GPIO and Bus System — ESP32-2432S028

> **Document status:** sample-specific expansion-I/O reference for the author's `ESP32-2432S028` board.  
> **Scope:** P3 and CN1 electrical behavior, GPIO capabilities and restrictions, candidate I2C/UART/SPI use, 3.3-V compatibility, external powering, bus sharing, protection, cable effects, diagnostics, and verification.  
> **Evidence rule:** visible connector labels and current repository assignments are treated as the implementation baseline; hidden routing, loading, protection, pull resistors, and safe current limits remain open until measured.

![ESP32-2432S028 connector area](../../hardware/images/connectors.jpg)

## 1. Purpose

The expansion connectors expose only a small subset of the ESP32 GPIO matrix, and several of those signals are not automatically “free.” On this board:

- GPIO21 is currently assigned to the TFT backlight;
- GPIO35 is input-only;
- GPIO22 is not used by the current board header but may be suitable for an external bus;
- GPIO27 is exposed on CN1;
- one CN1 position is explicitly marked `NC`;
- CN1 provides 3.3 V, while P3 provides no supply contact;
- all signal pins are 3.3-V logic;
- external devices can disturb boot, power, display, UART, ADC, or other subsystems if connected carelessly.

The objective is not merely to list pins. It is to define a safe process for deciding what each exposed signal may do on this exact board.

## 2. Confirmed connector labels

### 2.1 P3

Visible silkscreen order on the documented sample:

```text
GND | IO35 | IO22 | IO21
```

### 2.2 CN1

Visible silkscreen order:

```text
GND | NC | IO27 | 3.3V
```

These are board-side labels in the photographed orientation. They are not manufacturer pin numbers and must not be transferred to another CYD revision without checking that board's silkscreen.

## 3. Current board assignments relevant to expansion

The repository currently defines:

```text
GPIO21 -> TFT backlight control
GPIO22 -> not assigned in CYD_Board.h
GPIO35 -> exposed input-only GPIO
GPIO27 -> exposed bidirectional GPIO
```

The absence of a software assignment does not prove electrical freedom. A signal may still connect to an uncharacterized board feature, test pad, passive network, or alternate revision option.

## 4. Connector orientation discipline

Before making a cable:

1. place the board in the same orientation as the connector photograph;
2. locate `P3` or `CN1` on the PCB;
3. read the printed labels beside the connector;
4. identify ground by continuity to a known ground point;
5. measure the CN1 supply contact before attaching a peripheral;
6. label both ends of the cable;
7. ignore wire color as evidence.

A 1.25-mm-pitch connector can be inserted into an incorrectly wired cable housing while still looking mechanically correct.

## 5. Electrical summary

| Signal | Direction capability | Current board role | Main caution |
|---|---|---|---|
| P3 GND | reference | board ground | ground loops and high return current |
| GPIO35 | input only | exposed input | no output drive; no ordinary internal pull-up/down |
| GPIO22 | input/output | currently unassigned | verify hidden loading before use |
| GPIO21 | input/output | TFT backlight control | external use can flicker or disable backlight |
| CN1 GND | reference | board ground | same ground-domain cautions |
| CN1 NC | no claimed function | printed `NC` | leave unused until continuity-tested |
| GPIO27 | input/output | currently unassigned | verify boot and board interaction |
| CN1 3.3V | regulated supply | board rail | limited current; back-powering risk |

## 6. GPIO35

### 6.1 Input-only restriction

GPIO35 cannot function as a digital output.

Do not assign it to:

- LED drive;
- SPI clock or MOSI;
- I2C SCL or SDA where the controller must actively pull low;
- UART TX;
- PWM;
- relay or transistor control;
- chip select;
- one-wire bus master output.

### 6.2 Suitable roles

GPIO35 may be used for:

- digital input;
- interrupt input;
- ADC input;
- pulse or frequency input;
- external comparator output;
- button or switch input with an external bias resistor;
- receive-only serial input if timing and voltage are suitable.

### 6.3 Pull-resistor limitation

Input-only ESP32 pins in this range do not provide the same general-purpose internal pull-up and pull-down facilities as ordinary GPIOs.

Use an external resistor when a defined idle state is required.

Typical initial values for experimentation may be in the 4.7-kohm to 100-kohm range, but the selected value should be based on:

- source impedance;
- cable length;
- noise environment;
- power consumption;
- required edge rate;
- leakage current;
- interrupt sensitivity.

### 6.4 Analog use

GPIO35 is ADC-capable on the classic ESP32, but the external connector environment adds uncertainty:

- connector and cable capacitance;
- coupled TFT, Wi-Fi, audio and SD noise;
- source impedance;
- ground offset;
- no input overvoltage protection proven;
- ADC nonlinearity and board-to-board variation.

Use it first for relative measurements. Calibrated voltage or sensor results require a known reference and sample-specific characterization.

### 6.5 External voltage

Do not exceed the ESP32 input range. A sensor powered from 5 V may output 5 V even when its digital interface is described as “TTL.”

Use a divider, level shifter, open-drain interface, buffer, or other appropriate circuit where needed.

## 7. GPIO22

### 7.1 General capability

GPIO22 is a normal bidirectional 3.3-V GPIO on the classic ESP32.

Potential uses include:

- digital input or output;
- interrupt input;
- PWM;
- UART mapping;
- I2C clock or data;
- SPI control or data;
- peripheral enable or chip select.

### 7.2 Common I2C convention

Generic ESP32 examples frequently use:

```text
GPIO22 -> SCL
GPIO21 -> SDA
```

On this board, GPIO21 is already assigned to the TFT backlight. Therefore the generic pair must not be copied blindly.

Possible alternatives include:

- use GPIO22 plus GPIO27 as a software-selected I2C pair;
- move backlight control only after proving the hardware path and accepting loss of independent control;
- use a software I2C implementation where timing requirements are modest;
- add an external GPIO expander or bus multiplexer.

The pair `GPIO22/GPIO27` is a candidate, not a verified board standard.

### 7.3 Hidden-board loading check

Before using GPIO22:

1. measure resistance to ground and 3.3 V with power removed;
2. observe boot-time waveform with no external device;
3. run an input-only monitor;
4. drive high and low through a series resistor;
5. inspect current and edge shape;
6. verify display, touch, SD, audio and boot behavior.

## 8. GPIO21

### 8.1 Current role

The repository uses GPIO21 as `TFT_BACKLIGHT_PIN` and drives it high to enable the backlight.

Therefore GPIO21 is not presently a free expansion pin.

### 8.2 Conflict symptoms

External use can produce:

- backlight flicker;
- permanently dark display;
- unintended brightness modulation;
- contention with an external driver;
- high current through the board's backlight-control stage;
- altered boot-time light behavior;
- unreliable external bus communication.

### 8.3 Read-only observation

Even connecting a high-impedance logic analyzer or input can add capacitance and cable pickup. Keep leads short and verify that the backlight remains stable.

### 8.4 Reusing GPIO21 intentionally

Reassignment should be considered only after:

- the backlight circuit has been traced;
- active level is measured;
- transistor or direct-drive topology is known;
- external bus pull-ups are evaluated;
- software is modified consistently;
- a replacement method for backlight control is accepted.

An I2C pull-up on GPIO21 can keep the backlight control node high and prevent normal PWM or off control.

## 9. GPIO27

### 9.1 General capability

GPIO27 is a normal bidirectional 3.3-V GPIO.

Potential uses include:

- digital I/O;
- interrupt input;
- PWM;
- UART mapping;
- I2C;
- SPI chip select or data;
- one-wire buses;
- peripheral enable.

### 9.2 Verification requirement

The connector label establishes that GPIO27 is exposed. It does not prove:

- no onboard loading;
- no pull resistor;
- no boot-time use;
- no connection to the auxiliary Flash or another revision option;
- a particular maximum frequency.

### 9.3 First-use sequence

Use a 1-kohm to 4.7-kohm series resistor during early output testing where compatible with the load. Then:

1. boot with pin configured as input;
2. record idle voltage;
3. drive a slow square wave;
4. measure high and low levels;
5. verify current;
6. repeat while display, touch, SD, Wi-Fi and audio operate.

## 10. CN1 NC position

The printed `NC` means no connection is claimed for this board revision.

Do not reinterpret it as:

- GPIO33;
- GPIO32;
- second ground;
- 5 V;
- 3.3 V;
- a mechanical key;
- an undocumented interrupt.

Safe handling:

- leave it unconnected;
- measure continuity to ground, supplies and ESP32 pads only with power removed;
- record any measurable network before using it;
- retain the label `NC` in public pinout drawings until a verified alternate function is established.

## 11. Logic-voltage compatibility

### 11.1 Native level

The exposed GPIO signals are 3.3-V logic.

### 11.2 Not 5-V tolerant

Do not directly connect a push-pull 5-V output to GPIO21, GPIO22, GPIO27 or GPIO35.

### 11.3 Open-drain buses

Some 5-V devices use open-drain outputs and can operate safely only when the bus pull-ups are connected to 3.3 V and the device's input thresholds accept 3.3 V.

Verify both directions:

- ESP32 high level must be recognized by the peripheral;
- peripheral must not pull the line above 3.3 V;
- any onboard pull-up on the peripheral module must be identified.

### 11.4 Level-shifter selection

Different buses need different translators:

| Interface | Preferred translation approach |
|---|---|
| slow bidirectional I2C | open-drain MOSFET or dedicated I2C translator |
| UART | unidirectional buffer/divider in each direction |
| push-pull SPI | directional level translator with suitable bandwidth |
| analog input | divider, buffer, clamp and source-impedance analysis |
| interrupt output | open-drain with 3.3-V pull-up or unidirectional translator |

Avoid generic auto-direction level shifters on high-speed SPI unless their timing behavior is proven.

## 12. Ground and return-current rules

### 12.1 Common reference

Digital signals require a ground reference between the board and external device.

### 12.2 Ground is not unlimited

The small connector contact and cable conductor should not carry large motor, heater, servo, relay, speaker or LED-strip return currents.

High return current can create:

- ground bounce;
- false interrupts;
- ADC error;
- UART corruption;
- display artifacts;
- ESP32 resets;
- connector heating.

### 12.3 Star-return approach

For larger external loads:

- power the load separately;
- connect grounds at a controlled point;
- route load current away from the board's signal-ground path;
- use local decoupling;
- isolate inductive loads appropriately.

## 13. CN1 3.3-V supply

### 13.1 Confirmed label

CN1 exposes a contact marked `3.3V`.

### 13.2 Unknown safe current

The available current is not established by the label. It depends on:

- which AMS1117 supplies the contact;
- regulator thermal state;
- ESP32 Wi-Fi current;
- TFT and backlight load;
- SD and audio activity;
- cable and connector resistance;
- ambient temperature;
- USB input quality.

### 13.3 Conservative initial loads

Suitable early tests:

- low-power sensor;
- logic buffer;
- EEPROM;
- small I2C module after removing 5-V pull-ups;
- low-current ADC or GPIO expander.

Unsuitable direct loads:

- motor;
- servo;
- relay coil;
- solenoid;
- high-power LED;
- heater;
- radio module with large burst current unless separately qualified.

### 13.4 Back-powering

An externally powered peripheral may drive GPIOs while the board is off. Current can then flow through protection structures into the 3.3-V rail.

Likewise, injecting 3.3 V into CN1 may power parts of the board through an unverified path.

Before using external power:

1. measure current into the unpowered board;
2. check regulator input/output voltages;
3. observe USB connector VBUS;
4. verify EN and ESP32 state;
5. ensure no host or adapter is being reverse-powered.

## 14. I2C options

### 14.1 Candidate pair

A practical candidate is:

```text
SCL -> GPIO22
SDA -> GPIO27
```

The assignment can be reversed in software, but documentation should select and retain one convention for a project.

### 14.2 Pull-ups

I2C requires pull-up resistors.

The effective pull-up is the parallel combination of all installed pull-ups. Multiple breakout boards can produce an excessively strong pull-up.

Record:

- resistor value on each module;
- bus supply voltage;
- total effective resistance;
- bus capacitance estimate;
- rise time at the chosen clock.

### 14.3 Initial clock

Start at 50 or 100 kHz. Increase only after waveform and error testing.

### 14.4 Address conflicts

Two devices with the same fixed address cannot share the bus directly without:

- address configuration;
- bus multiplexer;
- separate buses;
- power or reset gating.

### 14.5 Cable effects

Long I2C wiring increases capacitance and susceptibility to noise. For off-board cables:

- use ground interleaving where possible;
- reduce clock;
- strengthen pull-ups within device limits;
- use differential extenders for long distances;
- avoid routing beside audio, motor or switching-power wiring.

### 14.6 Recovery

Firmware should handle a slave holding SDA low:

1. release bus lines;
2. clock SCL manually several times if safe;
3. generate a STOP condition;
4. reinitialize the controller;
5. power-cycle only the peripheral where possible;
6. report the fault rather than hanging indefinitely.

## 15. UART options

### 15.1 Candidate mapping

GPIO22 and GPIO27 can be mapped as an additional UART pair in software.

Example project convention:

```text
GPIO22 -> external UART TX
GPIO27 -> external UART RX
```

This is a software choice, not a board-defined function.

### 15.2 Direction crossing

```text
board TX -> peripheral RX
board RX <- peripheral TX
GND      <-> GND
```

### 15.3 Voltage and protocol

The interface is 3.3-V asynchronous UART, not RS-232 or RS-485.

### 15.4 Baud qualification

Start at 9600 or 115200. Qualify higher baud rates with:

- sequence numbers;
- CRC;
- full-duplex traffic;
- intended cable length;
- Wi-Fi, TFT, SD and audio stress;
- long-duration testing.

### 15.5 GPIO35 as receive-only UART

GPIO35 may be considered for a receive-only serial input, but this requires:

- external idle bias if needed;
- compatible peripheral mapping;
- no transmit requirement;
- timing verification;
- noise testing.

## 16. SPI options

### 16.1 Limited exposed pins

P3 and CN1 do not expose a convenient complete four-wire SPI bus plus chip select without reusing GPIO21 or GPIO35.

A conventional SPI peripheral needs at least:

```text
SCLK
MOSI
MISO
CS
GND
```

GPIO35 can serve only as an input, so it could potentially be MISO but never SCLK, MOSI or CS.

### 16.2 Candidate experimental mapping

One possible mapping is:

```text
GPIO22 -> SCLK
GPIO27 -> MOSI
GPIO35 -> MISO
GPIO21 -> CS
```

This conflicts with TFT backlight control and is therefore not recommended without deliberate hardware and software redesign.

### 16.3 Safer alternatives

- use an I2C peripheral instead;
- use a UART peripheral;
- expose additional verified pads;
- add an I/O expander for chip-select lines;
- share an existing SPI bus only after its electrical and timing rules are fully understood;
- use an external microcontroller as a bus bridge.

### 16.4 Sharing an onboard SPI bus

The board already uses separate serial paths for TFT, touch and microSD in the current profile. Adding a device to any of those buses requires:

- physical access to the bus signals;
- independent chip select;
- all devices releasing MISO when deselected;
- compatible SPI modes;
- controlled bus speed;
- transaction-level arbitration;
- no boot-time contention.

Do not assume that sharing a clock and data line is sufficient.

## 17. PWM and timing signals

GPIO22 and GPIO27 can be candidates for PWM or pulse generation.

Characterize:

- frequency;
- duty resolution;
- jitter under Wi-Fi and display load;
- startup level;
- external load current;
- cable ringing;
- whether the peripheral requires a defined off state during reset.

Use a buffer or transistor when driving anything beyond a small logic input.

## 18. Interrupt inputs

GPIO22, GPIO27 and GPIO35 can serve as interrupt inputs subject to software and hardware constraints.

Good practice:

- define an external pull-up or pull-down where required;
- add RC filtering only after considering pulse width;
- debounce mechanical switches in software or hardware;
- avoid long unshielded leads;
- record active polarity;
- ensure the signal remains within 0–3.3 V;
- avoid performing heavy work in the interrupt handler.

## 19. Open-drain and one-wire buses

GPIO22 or GPIO27 can implement open-drain style signaling.

Before connecting a one-wire or similar bus:

- confirm pull-up voltage;
- calculate current at low level;
- verify cable capacitance;
- test timing margin;
- avoid parasitic powering until qualified;
- add transient protection for external cables.

GPIO35 cannot be the actively driven master line.

## 20. External buttons and switches

### 20.1 Preferred pins

GPIO22, GPIO27 or GPIO35 may be used as switch inputs.

### 20.2 Biasing

- GPIO22/GPIO27 may use internal bias for simple local tests;
- external resistors are preferable for long cables or defined safety behavior;
- GPIO35 normally requires an external bias.

### 20.3 Protection

For an external-panel switch:

- add a series resistor;
- consider ESD protection;
- use a defined pull resistor;
- filter or debounce;
- route ground with the signal;
- avoid sharing the return with a high-current load.

## 21. External LEDs, relays and actuators

Do not drive significant loads directly from a GPIO.

Use:

- transistor or MOSFET driver;
- gate/base resistor;
- flyback diode for inductive DC loads;
- separate power supply where required;
- common ground or isolation as designed;
- default-off bias during reset.

The GPIO should control the driver, not carry load current.

## 22. Protection for off-board wiring

The connector area does not have a confirmed protection network for every GPIO.

For cables leaving an enclosure, consider:

- series resistance;
- TVS diode with suitable capacitance;
- RC filter;
- Schmitt-trigger buffer;
- galvanic isolation;
- differential transceiver;
- shield and chassis strategy;
- connector pin sequencing.

Protection must be chosen for the signal type. A high-capacitance TVS can damage high-speed timing even while improving ESD robustness.

## 23. Cable design

### 23.1 Keep early wiring short

Start with cables under roughly 10–20 cm for logic tests.

### 23.2 Pair signals with ground

Where possible:

- place ground beside clock;
- use twisted signal-ground pairs for longer runs;
- avoid one shared thin ground for several fast signals and a load.

### 23.3 Avoid breadboard ambiguity

Solderless breadboards add:

- contact resistance;
- capacitance;
- long loops;
- accidental shorts;
- poor high-frequency behavior.

A failure at high speed should be repeated with a short soldered adapter before blaming the ESP32.

## 24. Connector contact and cable qualification

For each cable assembly record:

```text
Connector family candidate:
Housing positions:
Contact type:
Wire gauge:
Crimp tool:
Insertion orientation:
Pull test result:
Contact resistance:
Ground continuity:
Signal mapping:
Strain relief:
```

A cable that works while stationary may fail after repeated flexing if the crimp is poor.

## 25. Power-up sequencing

External devices can force GPIO levels before the ESP32 is powered.

Test these states:

1. board and peripheral power on together;
2. board first, peripheral second;
3. peripheral first, board second;
4. USB removed while peripheral remains powered;
5. peripheral removed while board remains powered;
6. reset while peripheral drives signals;
7. deep sleep and wake.

Measure unintended current in every asymmetric-power state.

## 26. Boot-time behavior

GPIO22 and GPIO27 may transition or float before application initialization. GPIO21 follows the board's backlight circuit and software startup. GPIO35 remains an input.

External hardware requiring a strict startup state should include local bias or enable control rather than relying solely on application code.

Capture from power application through firmware initialization.

## 27. Low-power behavior

During light sleep, deep sleep or reset:

- GPIO hold behavior may differ;
- pull resistors may dominate;
- peripherals may back-power the board;
- CN1 3.3 V may or may not remain present depending on the power architecture;
- external interrupts may wake the ESP32;
- the backlight path on GPIO21 may consume power.

Low-power use requires separate verification.

## 28. Continuity and resistance mapping

With power removed, record for each exposed contact:

- continuity to the labelled ESP32 GPIO;
- resistance to ground;
- resistance to 3.3 V;
- resistance to 5 V;
- continuity to onboard devices;
- diode-mode readings in both directions;
- presence of series resistors;
- connector-to-test-pad mapping.

Suggested table:

| Contact | Label | ESP32 net verified | R to GND | R to 3.3 V | Other connection | Status |
|---|---|---|---:|---:|---|---|
| P3-? | GND | ground | | | | |
| P3-? | IO35 | | | | | |
| P3-? | IO22 | | | | | |
| P3-? | IO21 | | | | backlight candidate | |
| CN1-? | GND | ground | | | | |
| CN1-? | NC | | | | | |
| CN1-? | IO27 | | | | | |
| CN1-? | 3.3V | rail | | | regulator path | |

Do not assign manufacturer pin numbers until connector orientation is established.

## 29. Basic GPIO verification firmware

A safe test application should:

1. leave GPIO21 under normal backlight control;
2. configure GPIO35 as input and print its state;
3. configure GPIO22 and GPIO27 as inputs first;
4. report idle levels;
5. apply weak internal pulls where supported and record changes;
6. drive GPIO22 and GPIO27 slowly through external series resistors;
7. read back the driven state;
8. detect shorts or unexpected loading;
9. stop on overcurrent or abnormal rail behavior.

Avoid testing all pins as outputs simultaneously.

## 30. I2C verification firmware

The test should report:

- selected SDA/SCL pins;
- bus clock;
- discovered addresses;
- NACK count;
- timeout count;
- recovery count;
- maximum observed transaction latency;
- results during display, Wi-Fi, SD and audio load.

Use a known, low-power 3.3-V peripheral for the first test.

## 31. UART verification firmware

Use framed packets containing:

```text
sync
sequence
length
payload
CRC
```

Test:

- transmit only;
- receive only;
- full duplex;
- several baud rates;
- intended cable length;
- peripheral-first and board-first power sequencing.

## 32. Waveform measurements

For GPIO22 and GPIO27 record:

- low voltage;
- high voltage;
- rise time;
- fall time;
- overshoot;
- ringing;
- loaded and unloaded behavior;
- current through the series resistor;
- waveform during Wi-Fi and TFT activity.

For GPIO35 record:

- threshold behavior;
- input leakage effect;
- noise amplitude;
- interrupt false-trigger rate.

## 33. Bus stress matrix

| Test | TFT | Touch | SD | Wi-Fi | Audio | Expansion function | Expected result |
|---|---|---|---|---|---|---|---|
| baseline GPIO | off | off | off | off | off | slow toggle/read | correct levels |
| normal UI load | on | on | off | off | off | GPIO/UART/I2C | no flicker or errors |
| storage load | on | on | active | off | off | I2C/UART | no corruption |
| radio load | on | on | off | active | off | analog/input/bus | acceptable error rate |
| audio load | on | off | off | off | active | input/interrupt | no false triggers |
| all subsystems | on | active | active | active | active | selected expansion bus | qualified operation |

## 34. Diagnostic flow

### 34.1 Peripheral does not power

Check:

- CN1 3.3-V contact orientation;
- ground continuity;
- board rail voltage;
- peripheral current demand;
- connector crimp;
- voltage drop under load;
- regulator temperature.

### 34.2 Board resets when peripheral is connected

Possible causes:

- 3.3-V overload;
- shorted cable;
- 5-V signal into GPIO;
- back-powering;
- ground transient;
- GPIO21/backlight conflict;
- peripheral holding a boot-sensitive or reset-related net indirectly.

### 34.3 GPIO35 always reads high or low

Check:

- missing external bias;
- source output type;
- cable short;
- incorrect connector orientation;
- voltage outside valid range;
- software pin number;
- source impedance and leakage.

### 34.4 GPIO22 or GPIO27 cannot drive a clean level

Check:

- accidental external driver contention;
- onboard loading;
- wrong pin mapping;
- excessive load current;
- 5-V pull-up;
- damaged GPIO;
- series resistance and cable capacitance.

### 34.5 Backlight changes when using expansion wiring

GPIO21 is being affected. Remove the external connection and restore its backlight role.

### 34.6 I2C devices are not found

Check:

- SDA/SCL mapping;
- pull-up voltage;
- total pull-up resistance;
- common ground;
- address;
- clock frequency;
- stuck-low bus;
- module powered from the wrong voltage.

### 34.7 I2C works only at low speed

Investigate:

- cable capacitance;
- weak pull-ups;
- excessive level-shifter delay;
- breadboard wiring;
- noise;
- incompatible device timing.

### 34.8 UART shows garbage

Check:

- TX/RX crossing;
- baud and framing;
- common ground;
- voltage level;
- two transmitters driving one line;
- long cable;
- line interpreted as text when carrying binary data.

### 34.9 Peripheral works from USB power but fails from another supply arrangement

Investigate ground reference, back-powering, rail sequencing, USB VBUS relationship, regulator direction, and peripheral pull-ups.

## 35. Measurement record template

```text
Board sample/revision:
Date:
Operator:

P3 visible order:
CN1 visible order:
Connector family/cable:

GPIO35:
continuity:
idle voltage:
pull resistor:
digital test:
ADC test:

GPIO22:
continuity:
R to rails:
high/low voltage:
maximum tested toggle rate:
assigned function:

GPIO21:
backlight relation:
active level:
external loading test:

GPIO27:
continuity:
R to rails:
high/low voltage:
maximum tested toggle rate:
assigned function:

CN1 3.3 V:
source regulator:
no-load voltage:
loaded voltage:
qualified current:
thermal condition:
back-power result:

NC contact findings:

I2C:
pins:
pull-ups:
clock:
devices:
error rate:

UART:
pins:
baud:
cable:
CRC result:

Open questions:
Evidence files:
Conclusion:
```

## 36. Evidence levels

- **Photo-confirmed** — visible connector label or construction;
- **Repository-configured** — current software assignment;
- **Continuity-verified** — connector-to-net path measured;
- **Voltage-verified** — rail or logic level measured;
- **Waveform-verified** — timing and edge behavior captured;
- **Load-qualified** — operation demonstrated under declared current and temperature;
- **Stress-qualified** — bus operation demonstrated with other subsystems active;
- **Family-typical** — generic ESP32 capability not yet verified on the sample;
- **Open** — unresolved.

## 37. Claims that must not be made yet

Until measurement is complete, do not claim:

- exact connector manufacturer or pin numbering;
- that GPIO22 and GPIO27 are electrically unloaded;
- that GPIO21 is available while backlight control is required;
- that CN1 can supply a specific current;
- that CN1 3.3 V may safely be driven externally;
- that P3 or CN1 include ESD protection;
- that the NC contact is connected to any GPIO;
- that `GPIO22/GPIO27` is an official I2C pair;
- a guaranteed I2C, UART or SPI maximum speed;
- that any exposed pin is 5-V tolerant;
- that long cables are safe without protection;
- that peripheral-first powering is harmless;
- that generic CYD pinouts apply to this sample.

## 38. Completion criteria

The expansion subsystem can be considered sample-verified when:

1. connector orientation and cable fit are documented;
2. every P3 and CN1 contact is continuity-mapped;
3. GPIO21 backlight connection is confirmed;
4. GPIO22, GPIO27 and GPIO35 loading is measured;
5. NC is characterized and remains safely documented;
6. CN1 3.3-V source rail is identified;
7. qualified current and thermal limits are recorded;
8. back-power and asymmetric-power tests are complete;
9. logic voltage and edge behavior are captured;
10. one I2C configuration is qualified;
11. one external UART configuration is qualified;
12. any SPI claim is demonstrated with a complete verified pin set;
13. cable-length limits are recorded;
14. subsystem stress tests pass;
15. all protection and external-driver assumptions are documented.

## 39. Open questions

- Are GPIO22 and GPIO27 directly connected to ESP32 module pads?
- Are series resistors or pull resistors fitted?
- Is GPIO21 connected directly to the backlight transistor or through another stage?
- What regulator supplies CN1 3.3 V?
- What continuous and burst current can CN1 safely provide?
- Does external 3.3-V injection back-power USB or either regulator?
- Is the NC position truly floating on this revision?
- What cable family is mechanically exact?
- What I2C clock is reliable with the intended cable and devices?
- What additional-UART baud is reliable?
- Can any existing onboard SPI bus be safely exposed and shared?
- What external ESD protection is needed for the intended enclosure?

## 40. Summary

P3 exposes `GND | IO35 | IO22 | IO21`; CN1 exposes `GND | NC | IO27 | 3.3V`. GPIO35 is input-only. GPIO21 is currently the TFT backlight-control signal and should not be treated as free. GPIO22 and GPIO27 are the most practical candidates for an added two-wire bus or additional UART, but their electrical freedom and maximum speed remain to be measured.

All exposed logic is 3.3 V. CN1's 3.3-V contact is suitable only for conservatively tested low-power loads until its regulator path, thermal margin, current limit, and back-power behavior are established.