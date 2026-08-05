# System Pin Map and Resource Conflicts — ESP32-2432S028

> **Document status:** consolidated, sample-specific GPIO and peripheral-allocation reference for the author's `ESP32-2432S028` board.  
> **Scope:** current repository pin ownership, ESP32 input-only and boot-strapping restrictions, SPI/UART/I2C allocation, startup states, shared resources, reassignment rules, conflict diagnosis, verification, and change control.  
> **Evidence rule:** repository mappings are treated as the active software baseline; hidden copper routing, passive loading, alternate-revision wiring, startup waveforms, and safe reassignment remain open until measured on the documented sample.

## 1. Purpose

The board exposes many ESP32 capabilities, but most useful GPIOs are already committed to on-board hardware. A numeric pin list alone is therefore unsafe.

For every GPIO, the system must distinguish:

- the function assigned by the current repository;
- the physical device connected on this sample;
- whether the pin is input-only;
- whether it affects boot strapping;
- whether it is shared with a connector;
- whether another peripheral controller can remap onto it;
- its state before firmware configures it;
- the effect of an external pull-up, pull-down or driver;
- whether changing it breaks display, touch, storage, audio, programming or boot.

The goal is a controlled resource-allocation map, not a claim that every listed route has already been continuity-traced.

## 2. Active repository baseline

The current `CYD_Board.h`, TFT_eSPI configuration and PlatformIO flags define the following baseline:

| GPIO | Current function | Direction in normal use | Main restriction |
|---:|---|---|---|
| 0 | BOOT button / ROM download strap | input | boot-strapping; LOW during reset enters downloader |
| 1 | UART0 TX / USB serial / P1 | output | shared programming and logging path |
| 2 | TFT D/C | output | boot-strapping-sensitive on classic ESP32 |
| 3 | UART0 RX / USB serial / P1 | input | shared programming and application serial |
| 4 | RGB red | output, active LOW | boot-strapping-sensitive; permanent LED load |
| 5 | microSD CS | output | boot-strapping-sensitive; card/socket loading |
| 12 | TFT MISO | input | boot-strapping-sensitive; flash-voltage strap risk on classic ESP32 |
| 13 | TFT MOSI | output | assigned to TFT bus |
| 14 | TFT SCLK | output | assigned to TFT bus |
| 15 | TFT CS | output | boot-strapping-sensitive |
| 16 | RGB green | output, active LOW | permanent LED load; common alternate UART pin |
| 17 | RGB blue | output, active LOW | permanent LED load; common alternate UART pin |
| 18 | microSD SCLK | output | assigned to SD bus |
| 19 | microSD MISO | input | assigned to SD bus |
| 21 | TFT backlight | output, active HIGH | also exposed on P3; not free |
| 22 | P3 expansion | bidirectional candidate | hidden loading not yet excluded |
| 23 | microSD MOSI | output | assigned to SD bus |
| 25 | touch SCLK | output | assigned to XPT2046 bus |
| 26 | audio source | output | connected to on-board audio path |
| 27 | CN1 expansion | bidirectional candidate | hidden loading not yet excluded; ADC2 caveat |
| 32 | touch MOSI | output | assigned to XPT2046 bus |
| 33 | touch CS | output | assigned to XPT2046 bus |
| 34 | LDR ADC | input only | no ordinary internal pull-up/down |
| 35 | P3 expansion input | input only | no output drive; external bias normally required |
| 36 | touch IRQ | input only | no ordinary output function |
| 39 | touch MISO | input only | no ordinary output function |

GPIO6 through GPIO11 are not application GPIOs on the usual ESP-WROOM-32 arrangement because they are used by the module's internal Flash interface. They must not be treated as free pins.

## 3. Consolidated subsystem ownership

### 3.1 TFT display

```text
MISO  GPIO12
MOSI  GPIO13
SCLK  GPIO14
CS    GPIO15
D/C   GPIO2
RST   not separately controlled in current profile
BL    GPIO21
```

The display consumes five signal GPIOs plus the backlight-control GPIO.

GPIO2, GPIO12 and GPIO15 require particular care because they are associated with classic ESP32 boot-strapping behavior.

### 3.2 Resistive touch controller

```text
IRQ   GPIO36
MOSI  GPIO32
MISO  GPIO39
CLK   GPIO25
CS    GPIO33
```

GPIO36 and GPIO39 are input-only, which is compatible with their assigned IRQ and MISO roles.

The touch bus is separate from the TFT and microSD pin groups in the repository baseline.

### 3.3 microSD

```text
MISO  GPIO19
MOSI  GPIO23
SCLK  GPIO18
CS    GPIO5
```

GPIO5 is boot-strapping-sensitive. A damaged card, strong external pull or unusual socket state can therefore affect startup.

### 3.4 RGB LED

```text
red    GPIO4
 green  GPIO16
blue   GPIO17
logic  active LOW
```

These pins are not free merely because the LED is not used by an application. The on-board LED and series network remain connected.

### 3.5 Analog light input

```text
LDR -> GPIO34
```

GPIO34 is input-only and normally lacks general internal pull resistors.

### 3.6 Audio

```text
audio source -> GPIO26 -> on-board 8002A path
```

Reassigning GPIO26 can inject unintended signals into the amplifier input.

### 3.7 Programming and serial

```text
UART0 TX GPIO1
UART0 RX GPIO3
BOOT     GPIO0
RESET    EN
```

UART0 is shared by the ESP32 ROM bootloader, boot messages, CH340G and P1 service connector.

### 3.8 Expansion

```text
P3  GND | GPIO35 | GPIO22 | GPIO21
CN1 GND | NC     | GPIO27 | 3.3V
```

GPIO21 is already the TFT backlight control. GPIO35 is input-only. GPIO22 and GPIO27 are candidates for new functions, not automatically verified free pins.

## 4. Pin status classes

Use these status labels in design reviews:

- **OWNED** — required by an on-board subsystem;
- **SHARED** — used by an on-board subsystem and externally exposed;
- **CANDIDATE** — no current repository owner, but electrical freedom not yet proven;
- **INPUT_ONLY** — cannot source normal digital output;
- **STRAP_SENSITIVE** — reset-time level can affect boot behavior;
- **PROGRAMMING_CRITICAL** — required for upload, boot log or reset control;
- **MODULE_INTERNAL** — unavailable because used inside ESP-WROOM-32;
- **OPEN** — routing or load unresolved.

Current high-level classification:

| GPIO group | Classification |
|---|---|
| 6–11 | MODULE_INTERNAL |
| 0, 1, 3 | PROGRAMMING_CRITICAL |
| 2, 4, 5, 12, 15 | OWNED + STRAP_SENSITIVE |
| 13, 14, 18, 19, 23, 25, 26, 32, 33 | OWNED |
| 16, 17 | OWNED, possible software-peripheral conflict |
| 21 | SHARED + OWNED |
| 22, 27 | CANDIDATE |
| 34, 36, 39 | OWNED + INPUT_ONLY |
| 35 | CANDIDATE + INPUT_ONLY |

## 5. Boot-strapping pins

Classic ESP32 startup samples selected GPIO levels during reset. The exact strapping interpretation must be checked against the matching Espressif documentation and module configuration, but these board pins deserve special caution:

```text
GPIO0
GPIO2
GPIO4
GPIO5
GPIO12
GPIO15
```

### 5.1 GPIO0

- connected to BOOT control;
- LOW during reset requests ROM serial download mode;
- external circuitry must not unintentionally hold it LOW;
- normal application use is possible only with strict startup discipline, but this board already dedicates it to BOOT.

### 5.2 GPIO2

- current TFT D/C signal;
- external loading or an alternate display circuit may disturb reset-time state;
- reassignment risks both boot and display operation.

### 5.3 GPIO4

- current red RGB channel;
- LED network contributes a permanent electrical load;
- external low-impedance circuitry can alter boot state.

### 5.4 GPIO5

- current microSD chip select;
- card/socket behavior and pull networks matter during reset;
- test boot with no card, known-good card and failed-card scenarios.

### 5.5 GPIO12

- current TFT MISO;
- is a particularly sensitive classic ESP32 strapping pin associated with internal Flash supply configuration on relevant devices;
- do not add a strong pull-up without a confirmed design review;
- an external display or analyzer connection must not control its reset-time level.

### 5.6 GPIO15

- current TFT chip select;
- external loading must preserve valid reset behavior;
- reassignment affects both display selection and boot assumptions.

## 6. Input-only GPIOs

On this classic ESP32 profile:

```text
GPIO34
GPIO35
GPIO36
GPIO39
```

are input-only.

They cannot be used for:

- UART TX;
- SPI MOSI or clock;
- I2C master SDA/SCL where active pull-down is required;
- PWM output;
- LED or relay drive;
- chip-select output;
- one-wire master output.

They can be useful for:

- ADC;
- digital input;
- interrupt input;
- pulse counting;
- receive-only serial;
- external comparator output;
- switches with external bias.

Do not assume ordinary internal pull-up or pull-down availability on these pins.

## 7. ADC ownership and Wi-Fi interaction

### 7.1 ADC1-oriented inputs

GPIO34 and GPIO35 belong to the ADC1-capable group on classic ESP32 and are generally the preferred external analog candidates when Wi-Fi must remain active.

Board-level noise, source impedance and calibration still require characterization.

### 7.2 ADC2 caveat

GPIO27 is ADC2-capable on the classic ESP32. ADC2 use is constrained by Wi-Fi operation in common ESP32 software stacks.

A sensor that works with Wi-Fi disabled may fail, block or return unusable readings when Wi-Fi is active.

Therefore GPIO27 should not be selected for a critical analog measurement until the exact framework behavior is tested under radio load.

### 7.3 GPIO34 already has an on-board source

GPIO34 is connected to the LDR divider in the repository model. It is not a free external ADC input.

### 7.4 GPIO35 external ADC candidate

GPIO35 is the cleaner expansion candidate, but still requires:

- external voltage-range control;
- external bias where needed;
- source-impedance analysis;
- noise and calibration tests;
- proof that no hidden board network is connected.

## 8. SPI resource map

The repository uses three distinct signal groups:

| Logical bus | Pins | Device |
|---|---|---|
| TFT bus | 12, 13, 14, 15 plus D/C 2 | ILI9341-compatible profile |
| touch bus | 39, 32, 25, 33 plus IRQ 36 | XPT2046 |
| SD bus | 19, 23, 18, 5 | microSD |

### 8.1 Hardware-controller versus pin group

The ESP32 GPIO matrix allows peripheral signals to be routed flexibly, but the number of hardware SPI controllers and framework/library assumptions remain finite.

Three separate pin groups do not automatically imply three independent unrestricted hardware SPI controllers.

### 8.2 Common conflict mechanisms

- two libraries both initialize the same SPI controller with different pins;
- one library calls global `SPI.begin()` and overwrites another configuration;
- chip-select is not driven inactive before bus initialization;
- a transaction changes mode or frequency and does not restore it;
- DMA channel or mutex ownership is not coordinated;
- display transfer blocks touch or SD servicing;
- independent buses increase GPIO use and software complexity.

### 8.3 Required SPI ownership record

For a combined application, record:

```text
controller object
pin assignment
owner module
SPI mode
maximum qualified clock
chip-select idle state
transaction API
mutex/locking policy
DMA use
initialization order
recovery behavior
```

### 8.4 Safe integration sequence

1. qualify TFT alone;
2. qualify touch alone;
3. qualify SD alone;
4. combine TFT and touch with explicit bus objects;
5. add SD with explicit ownership;
6. test simultaneous display refresh, touch polling and SD transfers;
7. capture bus waveforms and check chip-select exclusivity;
8. stress with Wi-Fi and audio active.

## 9. UART resource map

### 9.1 UART0

GPIO1 and GPIO3 form the normal programming and logging UART.

They are shared by:

- ESP32 ROM download protocol;
- boot messages;
- Arduino `Serial`;
- CH340G;
- P1 external connector.

### 9.2 External UART conflict

Connecting an external transmitter to P1 RX while CH340G is also able to drive the same logical path can create contention depending on exact wiring.

Use one active transmitter at a time until series isolation is confirmed.

### 9.3 Alternate UARTs

The ESP32 peripheral matrix can map additional UART signals to suitable GPIOs. Candidate pins must still pass the board-resource audit.

Potential candidate pair:

```text
GPIO22 + GPIO27
```

but direction assignment and hidden loading must be verified.

GPIO35 can be receive-only, never TX.

GPIO16/17 are common generic UART2 defaults in examples, but on this board they drive the green and blue LED channels and therefore are not free.

## 10. I2C resource map

Generic ESP32 examples often use:

```text
SDA GPIO21
SCL GPIO22
```

That convention is unsafe here because GPIO21 controls TFT backlight.

### 10.1 Candidate external pair

A practical candidate is:

```text
GPIO22 + GPIO27
```

This requires explicit software pin selection and electrical verification.

### 10.2 I2C requirements

- both lines must be output-capable open-drain GPIOs;
- pull-ups must go to 3.3 V, not 5 V;
- combined pull-up strength must be calculated;
- bus capacitance and rise time must be measured for long cables;
- stuck-low recovery must be defined;
- duplicate addresses require a multiplexer or other solution;
- GPIO35 cannot serve as a normal I2C line.

### 10.3 Backlight conflict on GPIO21

Using GPIO21 as SDA can:

- keep the backlight node pulled high;
- produce flicker during traffic;
- conflict with the backlight transistor or driver;
- prevent independent dimming or off control.

Do not use the generic 21/22 pair without first redesigning and verifying the backlight path.

## 11. PWM, LEDC and DAC resources

### 11.1 Backlight PWM

GPIO21 may be PWM-controlled for brightness, but external use then shares the same waveform.

### 11.2 RGB PWM

GPIO4, GPIO16 and GPIO17 can use LEDC channels for LED brightness, subject to active-LOW inversion.

### 11.3 Audio

GPIO26 can be used by the current audio path. On classic ESP32 it may support DAC-related use, but the on-board amplifier and filtering define the actual electrical behavior.

### 11.4 Timer/channel conflicts

Libraries may allocate LEDC channels or timers independently. Conflicts can produce:

- changed PWM frequency;
- disabled backlight;
- audio artifacts;
- RGB flicker;
- one module overwriting another channel configuration.

Maintain a software resource registry for LEDC channels and timers, not only GPIO numbers.

## 12. Interrupt ownership

Potential interrupt sources include:

- touch IRQ on GPIO36;
- external input on GPIO35;
- BOOT button on GPIO0;
- external GPIO22 or GPIO27;
- UART receive;
- SD and SPI completion paths;
- timers and wireless stack events.

ISR design rules:

- keep handlers short;
- do not perform blocking display, SD or network operations in an ISR;
- use volatile flags or queues;
- debounce mechanical inputs outside the ISR;
- record interrupt rate and overflow;
- avoid attaching conflicting handlers to a shared logical source.

## 13. Startup-state ownership

Firmware configuration happens after reset. Before that, pins can be:

- sampled as straps;
- high impedance;
- controlled by ROM code;
- driven by external pull networks;
- affected by on-board peripherals.

For each output used in a safety-relevant role, measure:

```text
power-off level
power-rise level
reset-held level
strap-sampling interval
ROM boot level
application-init transition
steady-state level
brownout behavior
```

Do not assume that `pinMode()` in `setup()` controls the earliest startup state.

## 14. Recommended initialization order

A conservative application sequence is:

1. establish safe inactive chip-select states;
2. initialize backlight to the intended safe state;
3. turn RGB channels off;
4. configure audio output inactive;
5. configure input-only pins;
6. initialize serial logging without blocking indefinitely;
7. initialize display bus and controller;
8. initialize touch bus;
9. initialize SD bus and card;
10. initialize expansion buses;
11. start Wi-Fi/Bluetooth;
12. enable high-load or high-rate application behavior.

The exact order should be justified and regression-tested.

## 15. Resource-conflict matrix

| Requested new use | Direct conflict | Secondary risk | Default decision |
|---|---|---|---|
| GPIO21 as I2C SDA | TFT backlight | pull-up and flicker | reject unless backlight redesigned |
| GPIO16/17 as UART | RGB LED | LED loading and visible data flicker | reject for clean UART |
| GPIO26 as general PWM | audio path | amplifier noise | reject unless audio disabled and traced |
| GPIO35 as output | hardware limitation | none | impossible |
| GPIO27 as analog with Wi-Fi | ADC2/Wi-Fi | framework-dependent failure | avoid for critical ADC |
| GPIO1/3 as application UART | programming/logging | CH340/P1 contention | use only with explicit ownership |
| GPIO5 external device CS | microSD + strap | startup failure | reject |
| GPIO12 external pull-up | TFT MISO + strap | invalid Flash voltage configuration | reject without formal analysis |
| GPIO2 external signal | TFT D/C + strap | boot/display failure | reject |
| GPIO22/27 as I2C | no current repository owner | hidden loading, cable effects | candidate after verification |
| GPIO35 as switch input | none known | no internal pull | acceptable with external bias |

## 16. Safe reassignment gate

A pin may be reassigned only after all of the following are answered:

1. What current board function owns it?
2. Is it physically connected to a device or connector?
3. Is it input-only?
4. Is it boot-strapping-sensitive?
5. Is it required for programming or recovery?
6. What is its measured startup waveform?
7. What pull resistors or active drivers are present?
8. What happens if the external device is unpowered?
9. What happens if the board is unpowered?
10. Does the new function need output, interrupt, ADC, open-drain or high speed?
11. Does Wi-Fi constrain the selected ADC?
12. Does a library assume a default pin or peripheral controller?
13. Is there a recovery path if firmware misconfigures it?
14. Have all affected subsystems been regression-tested?

Failure to answer any critical item keeps the reassignment status **OPEN**.

## 17. Software ownership model

Centralize board pins in one authoritative header and avoid scattered numeric literals.

Recommended pattern:

```cpp
namespace board {
constexpr uint8_t kTftCs = 15;
constexpr uint8_t kTouchCs = 33;
constexpr uint8_t kSdCs = 5;
constexpr uint8_t kBacklight = 21;
}
```

Add compile-time checks for accidental duplicates where duplication is not intentional.

Document intentional sharing explicitly.

## 18. Resource manifest

A machine-readable or table-based manifest should include:

```text
resource ID
GPIO or peripheral controller
owner
shared users
active level
direction
startup requirement
strap sensitivity
external connector
library/config source
verification status
```

The manifest should cover:

- GPIOs;
- SPI controllers;
- UARTs;
- I2C controllers;
- LEDC channels;
- timers;
- DMA use;
- interrupts;
- ADC units/channels;
- power rails.

## 19. Configuration consistency audit

Pin mappings currently appear in several places:

- `include/CYD_Board.h`;
- `config/tft_espi/User_Setup.h`;
- `platformio.ini` build flags;
- examples;
- `docs/pinout.md`;
- hardware-reference chapters.

Every mapping change must update all authoritative locations or replace duplication with generated configuration.

### 19.1 Current known naming mismatch risk

Some repository text uses the common `ESP32-2432S028R` profile while the photographed sample is marked `ESP32-2432S028` and contains CH340G. This is acceptable only when the distinction is stated and the actual sample mapping remains verified.

## 20. Static conflict checks

Recommended automated checks:

- detect duplicate GPIO constants;
- permit only declared intentional sharing;
- ensure input-only pins are never configured as outputs;
- flag GPIO6–11 use;
- flag strap-sensitive pins in external expansion profiles;
- compare TFT pins between header, TFT_eSPI config and PlatformIO flags;
- compare documentation tables with code constants;
- verify all examples include the common board header where practical.

## 21. Runtime self-test

A resource-aware self-test can report:

```text
firmware commit
board profile name
pin assignments
input-only validation
chip-select idle levels
BOOT state
LDR raw value
backlight state
RGB state
UART status
SD mount result
touch IRQ idle state
expansion pin idle voltages
reset reason
```

It must not drive candidate expansion pins until a safe test fixture is connected.

## 22. Electrical verification plan

For each GPIO:

1. power off and measure resistance to ground and 3.3 V;
2. continuity-map to known device pins or connectors;
3. capture power-up and reset waveform;
4. observe application initialization;
5. drive through a protective series resistor where output-capable;
6. measure high/low voltage and edge shape;
7. test with related peripheral active and inactive;
8. test with external device powered and unpowered;
9. log any current or contention;
10. update evidence status.

## 23. Bus verification plan

### 23.1 TFT

Capture GPIO2, 12, 13, 14 and 15 during initialization and full-screen transfer.

### 23.2 Touch

Capture GPIO25, 32, 33, 39 and IRQ36 during idle and touch.

### 23.3 microSD

Capture GPIO5, 18, 19 and 23 during mount, read and write.

### 23.4 Combined load

Capture chip-select lines simultaneously and verify that only the intended device is selected.

## 24. Boot regression matrix

Test boot with:

- no external cables;
- USB only;
- P1 adapter receive-only;
- P3 disconnected;
- candidate device on GPIO22/27;
- SD absent;
- SD present;
- BOOT held;
- RESET pressed;
- weak and nominal supply;
- Wi-Fi starting immediately;
- display and backlight enabled.

Record boot mode, reset reason and first application log.

## 25. Diagnostic cases

### 25.1 Board enters bootloader unexpectedly

Check GPIO0 and auto-programming controls for unintended LOW during reset.

### 25.2 Boot fails only when an external device is connected

Disconnect strap-sensitive pins first: GPIO0, 2, 4, 5, 12 and 15.

Then inspect external pull networks, power sequencing and back-powering.

### 25.3 Display is dark after adding I2C

Check whether GPIO21 was used as SDA or pulled high/low by the bus.

### 25.4 RGB LED flickers with serial traffic

Check whether GPIO16 or GPIO17 was reused as UART.

### 25.5 Audio noise appears after PWM changes

Check GPIO26 ownership and LEDC timer/channel reconfiguration.

### 25.6 SD fails after display or touch initialization

Check SPI-controller ownership, global `SPI.begin()` calls, chip-select idle states and transaction restoration.

### 25.7 Touch freezes during screen updates

Check blocking TFT transfers, shared controller misuse, task priority and missing bus locking.

### 25.8 Analog input fails only with Wi-Fi

Check whether the signal is on ADC2, especially GPIO27.

### 25.9 External input on GPIO35 floats

Add and verify an external pull-up or pull-down; do not rely on an ordinary internal pull.

### 25.10 Upload fails while P1 adapter is attached

Remove the external transmitter and check UART0 contention.

## 26. Change-control procedure

Any pin-map change should include:

1. issue or design note describing the need;
2. current owner and affected subsystems;
3. proposed new map;
4. boot-strapping review;
5. input/output capability review;
6. electrical continuity evidence;
7. startup waveform evidence;
8. software-library compatibility;
9. updated central header and build config;
10. updated examples and documentation;
11. regression-test results;
12. rollback/recovery procedure.

## 27. Evidence levels

Use these labels:

- **Repository-configured** — present in current source or build flags;
- **Photo-supported** — device/connector identity visible;
- **Continuity-verified** — net traced electrically;
- **Waveform-verified** — startup and operation captured;
- **Function-verified** — subsystem works on the sample;
- **Conflict-verified** — intentional sharing tested under combined load;
- **Reassignment-qualified** — new use passed boot and subsystem regression;
- **Revision-qualified** — confirmed on a specified PCB revision;
- **Open** — unresolved.

## 28. Claims that must not be made yet

Do not yet claim:

- that GPIO22 and GPIO27 are electrically unconnected except at their connectors;
- that every repository mapping is continuity-proven;
- that generic CYD pinouts apply to every board revision;
- that TFT, touch and SD use three fully independent unrestricted hardware SPI controllers;
- that GPIO21 can safely serve I2C while retaining backlight control;
- that GPIO16/17 are free UART pins;
- that GPIO26 is a free DAC/PWM output;
- that 921600-baud upload is universally reliable;
- that GPIO27 ADC works concurrently with Wi-Fi;
- that any strap-sensitive pin tolerates arbitrary external pulls;
- that internal pull resistors exist on GPIO34–39;
- that a pin is safe merely because firmware does not currently reference it.

## 29. Completion criteria

The system pin map can be considered sample-qualified when:

1. all active repository GPIO constants are inventoried;
2. duplicate configuration sources are reconciled;
3. every on-board GPIO route is continuity-checked;
4. all connector pins are continuity-checked;
5. input-only restrictions are enforced in code review or CI;
6. GPIO6–11 are excluded from application use;
7. startup waveforms are captured for all strap-sensitive pins;
8. BOOT, RESET and UART0 paths are documented;
9. TFT, touch and SD controller ownership is explicit;
10. chip-select exclusivity is verified under combined load;
11. LEDC/timer allocation is documented;
12. ADC1/ADC2 use is documented and tested with Wi-Fi;
13. candidate GPIO22/27 use is electrically qualified;
14. GPIO35 external-bias requirements are verified;
15. expansion power-off/back-power behavior is tested;
16. boot regression matrix passes;
17. combined TFT/touch/SD/Wi-Fi/audio stress passes;
18. resource manifest is version-controlled;
19. automated static conflict checks are added;
20. revision-specific differences remain explicit.

## 30. Open questions

- Are all `CYD_Board.h` routes continuity-correct on the photographed sample?
- What hidden loading, if any, exists on GPIO22 and GPIO27?
- What is the exact backlight-control topology on GPIO21?
- Which hardware SPI controller does each library actually claim at runtime?
- Can TFT, touch and SD operate concurrently without software SPI?
- What startup levels appear on GPIO2, 4, 5, 12 and 15?
- What pull resistors are fitted on strap-sensitive nets?
- Are UART0 lines isolated between CH340G and P1?
- What LEDC timers/channels are used by backlight, RGB and audio code?
- What ADC calibration is required for GPIO34 and GPIO35?
- How does GPIO27 ADC behave with the chosen Wi-Fi framework version?
- Can GPIO22/27 form a reliable I2C bus in the intended enclosure and cable length?
- Which board revisions differ from this map?

## 31. Summary

The current profile consumes nearly every convenient GPIO. TFT, touch, microSD, RGB LED, LDR, audio, UART0, BOOT and backlight all have established repository owners. GPIO21 is shared with P3, GPIO35 is input-only, and GPIO22/GPIO27 are only candidate expansion resources.

The most dangerous conflicts involve boot-strapping pins, UART0 programming, GPIO21 backlight reuse, GPIO16/17 LED loading, GPIO26 audio loading, ADC2 use with Wi-Fi, and implicit SPI-controller ownership. Any reassignment must pass an electrical, startup, software and regression-test gate rather than relying on a generic ESP32 pinout.