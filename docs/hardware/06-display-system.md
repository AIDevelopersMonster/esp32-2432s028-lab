# Display System — ESP32-2432S028

> **Document status:** sample-specific display-system reference for the author's `ESP32-2432S028` board.  
> **Evidence rule:** visible construction, repository configuration, and successful software behavior are kept separate from unverified controller identity and hidden routing.  
> **Related chapters:** [Hardware Overview](01-hardware-overview.md), [PCB Component Atlas](02-pcb-component-atlas.md), [PCB Walkthrough](03-pcb-walkthrough.md), [Connector Reference](04-connector-reference.md), and [Power System](05-power-system.md).

![ESP32-2432S028 board overview](../../hardware/images/board-overview.jpg)

## 1. Scope

This chapter documents the TFT display subsystem as it is currently understood from the photographed board and the repository's working configuration.

It covers:

- physical display assembly;
- host interface and control signals;
- current software assumptions;
- reset and backlight behavior;
- orientation and color-order handling;
- SPI frequency, readback, and throughput;
- DMA and bus-sharing considerations;
- power and signal-integrity interactions;
- controller-identification strategy;
- a repeatable display verification plan.

It does **not** claim that the controller is physically proven to be ILI9341. The repository currently uses an ILI9341 configuration because that is the established working model for this board family, but the exact controller on the photographed sample remains open until stronger evidence is recorded.

## 2. Confirmed physical facts

| Item | Evidence | Status |
|---|---|---|
| TFT assembly | visible on the display side | Confirmed |
| Nominal size class | board model and physical format | 2.8-inch class |
| Resolution class | `2432` model designation and repository configuration | 240 × 320 working configuration |
| Touch overlay | visible resistive layer and XPT2046 on PCB | Confirmed |
| Backlight | illuminated TFT module and configured control signal | Function confirmed; circuit topology open |
| Host controller | ESP-WROOM-32 | Confirmed |
| Exact TFT controller IC | hidden in display assembly | Open |

The display and touch panel are mechanically integrated but electrically distinct subsystems. The TFT image path is not handled by XPT2046.

## 3. Current repository configuration

The repository currently configures TFT_eSPI with the following working assumptions:

```c
#define ILI9341_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 320

#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST  -1

#define TFT_BL 21
#define TFT_BACKLIGHT_ON HIGH

#define SPI_FREQUENCY       40000000
#define SPI_READ_FREQUENCY  20000000
```

The same definitions are mirrored in `platformio.ini` through build flags.

### Documentation meaning

These values should be interpreted as:

- the project's **current tested software profile**;
- a strong practical indication of board-family routing;
- not yet a continuity-verified schematic for every signal;
- not yet physical proof of the display-controller model.

## 4. Functional signal path

```text
ESP32
  |-- GPIO14 --> TFT SCLK
  |-- GPIO13 --> TFT MOSI
  |-- GPIO12 <-- TFT MISO / readback path
  |-- GPIO15 --> TFT CS
  |-- GPIO2  --> TFT D/C
  |-- reset handling: TFT_RST = -1 in current configuration
  `-- GPIO21 --> backlight control in current configuration
```

The display logic and the backlight are separate functions:

```text
ESP32 SPI/control signals --> TFT controller --> pixel matrix
ESP32 GPIO21 / switching path --> LED backlight
```

A visible image requires both a correctly initialized controller and a powered backlight. A lit white screen does not prove that SPI communication is working.

## 5. Signal reference

### 5.1 TFT_SCLK — GPIO14

Serial clock for display transactions.

Important considerations:

- clock quality becomes more critical at higher frequencies;
- long external wiring or probe capacitance can distort edges;
- this pin must not be assumed free for expansion while the display is active;
- observed stable clock should be recorded on the actual board rather than copied from another revision.

### 5.2 TFT_MOSI — GPIO13

Carries commands, parameters, and pixel data from ESP32 to the display controller.

Most display traffic is write-heavy, so MOSI performance strongly influences rendering speed.

### 5.3 TFT_MISO — GPIO12

Configured as the display readback line.

Potential uses include:

- controller register reads;
- display ID reads;
- pixel or status reads where supported;
- diagnostics.

However, a configured MISO pin does not prove that the physical display module drives it correctly. Some low-cost modules have incomplete, tri-stated, or revision-dependent readback paths.

GPIO12 is also a classic ESP32 strapping-related pin, so external loading during reset deserves caution.

### 5.4 TFT_CS — GPIO15

Chip-select for the display controller.

The line should remain inactive when another device uses the same SPI host or shared data lines. Incorrect CS handling can produce:

- corrupted pixels;
- bus contention;
- failed SD-card access;
- invalid controller reads;
- sporadic initialization failures.

### 5.5 TFT_DC — GPIO2

Selects command versus data mode.

Typical meaning:

- one level indicates command bytes;
- the other level indicates command parameters or pixel data.

GPIO2 also participates in ESP32 boot behavior on classic modules. The board's built-in loading is evidently intended to allow normal boot, but external circuitry must not force an incompatible state during reset.

### 5.6 TFT_RST — configured as `-1`

The current software configuration declares no independently controlled TFT reset pin.

Possible physical interpretations include:

- display reset tied to board reset;
- display reset held by an RC network;
- display reset connected to a fixed level;
- reset controlled indirectly by another circuit;
- controller tolerant of software-only initialization after power-up.

The exact implementation remains open until the net is traced.

### 5.7 TFT_BL — GPIO21

The repository treats GPIO21 as the backlight-control signal and uses `HIGH` as the illuminated state.

This has two important consequences:

1. GPIO21 is not automatically free for external I2C or GPIO use.
2. external use of P3's `IO21` contact can affect the display backlight or be affected by the onboard backlight circuit.

The exact switching topology remains open. GPIO21 may drive the LED path directly, control a transistor, or feed another intermediate stage.

## 6. Display-controller identity

### 6.1 Current working assumption

The repository enables `ILI9341_DRIVER`.

This is reasonable because:

- the board family is widely associated with ILI9341-compatible 240 × 320 TFT modules;
- the current project structure was built around that profile;
- the signal set and dimensions are consistent with that class of controller.

### 6.2 Why this is not yet physical proof

The controller package is not visible in the available board photographs. It may be:

- mounted on the display flex;
- hidden under the panel;
- chip-on-glass;
- chip-on-flex;
- replaced by a compatible or partially compatible controller in another production batch.

A successful image is strong functional evidence but may not distinguish between register-compatible controller variants.

### 6.3 Evidence levels

| Evidence | Strength |
|---|---|
| seller listing says ILI9341 | weak, revision-dependent |
| generic CYD example works | moderate family evidence |
| board displays correctly with ILI9341 init | strong functional compatibility evidence |
| stable controller-ID readback | stronger, if read path is valid |
| visible flex/controller marking | strong physical evidence |
| exact manufacturer BOM or schematic | strong documentary evidence |

### 6.4 Safe wording

Until identification is complete, documentation should say:

> The board is currently operated using an ILI9341-compatible 240 × 320 configuration. The exact controller on this sample has not yet been physically identified.

## 7. Controller-identification procedure

### Phase A — non-invasive software tests

1. initialize using the current ILI9341 profile;
2. verify full-screen color fills;
3. verify address-window writes across all edges;
4. test all four rotations;
5. test inversion on and off;
6. test RGB/BGR color order;
7. attempt documented register reads at a conservative SPI frequency;
8. repeat reads several times and compare consistency.

### Phase B — readback validation

A controller-ID result should not be trusted merely because a byte sequence was returned.

Validate that:

- MISO is not permanently high or low;
- values change appropriately across different registers;
- repeated reads are stable;
- CS and D/C timing match the controller protocol;
- slower clock produces the same result;
- another known device is not driving the bus;
- dummy cycles are handled correctly.

### Phase C — physical evidence

Where safe and non-destructive:

- inspect visible flex markings;
- photograph labels at high resolution;
- inspect exposed test pads;
- compare flex geometry with manufacturer drawings;
- do not remove the display solely to satisfy curiosity if doing so risks the touch panel or adhesive.

## 8. Initialization sequence

A typical TFT initialization process includes:

```text
power stable
  -> optional hardware reset
  -> software reset
  -> controller-specific power and timing registers
  -> pixel format
  -> memory-access control
  -> sleep-out
  -> required delay
  -> display-on
  -> clear framebuffer area
```

Failure at different stages can produce similar symptoms.

### Common visible outcomes

| Symptom | Possible cause |
|---|---|
| backlight on, screen completely white | controller not initialized, CS/DC issue, wrong driver, missing power rail |
| backlight off, no visible image | GPIO21 state, backlight power path, firmware not reaching setup |
| random pixels | unstable SPI, incorrect initialization, power transient |
| shifted or wrapped image | wrong width/height, address window, rotation or controller mismatch |
| red and blue swapped | RGB/BGR order mismatch |
| mirrored image | memory-access-control orientation setting |
| image appears only after manual reset | reset or startup-timing problem |

## 9. Pixel format and memory size

A common operating mode is RGB565:

- 16 bits per pixel;
- 2 bytes per pixel;
- 65,536 encoded colors.

A complete 240 × 320 frame contains:

```text
240 × 320 = 76,800 pixels
76,800 × 2 = 153,600 bytes
```

This matters because a full framebuffer is substantial relative to the internal RAM available to a classic ESP32 application.

### Practical rendering models

- direct drawing commands;
- small line or tile buffers;
- partial sprites;
- full sprites only when memory budget permits;
- compressed assets decoded in strips;
- direct transfers from Flash or SD with bounded buffers.

## 10. Color order and test pattern

A robust color test should display:

- black;
- white;
- red;
- green;
- blue;
- cyan;
- magenta;
- yellow;
- grayscale ramp;
- color gradients;
- one-pixel border.

The one-pixel border reveals clipping, offset, and rotation errors. Separate red and blue fields reveal RGB/BGR swaps immediately.

Do not declare color configuration correct using only text or a single photograph.

## 11. Rotation and coordinate systems

The TFT controller commonly supports four logical rotations.

For each rotation, document:

- reported width and height;
- physical top edge;
- origin location;
- X direction;
- Y direction;
- color order;
- border correctness;
- touch-coordinate mapping.

Display rotation and touch calibration are related at application level but controlled by different hardware. Rotating the TFT does not automatically rotate XPT2046 coordinates.

## 12. Backlight subsystem

### 12.1 Current behavior

The repository uses:

```c
#define TFT_BL 21
#define TFT_BACKLIGHT_ON HIGH
```

This defines software behavior, not the full electrical circuit.

### 12.2 Open hardware questions

- Does GPIO21 drive the LED load directly or through a transistor?
- Is the backlight powered from 3.3 V or 5 V?
- Is PWM dimming electrically supported?
- What PWM frequency avoids visible flicker and audible interference?
- Is the control active-high on every board revision?
- What current does the backlight draw at full brightness?
- Which AMS1117 rail, if any, supplies it?

### 12.3 PWM dimming test

Use a staged test:

1. confirm fully off and fully on states;
2. use a low-risk moderate PWM frequency;
3. sweep duty cycle slowly;
4. observe flicker, noise, color shift, and regulator heating;
5. check whether audio noise changes with PWM;
6. confirm that GPIO21 external loading on P3 does not interfere.

Do not connect an external load to GPIO21 until the onboard circuit is understood.

## 13. SPI frequency

The current project uses:

- write frequency: 40 MHz;
- read frequency: 20 MHz.

These are configuration values, not guaranteed electrical limits.

### Why writes and reads differ

Readback often requires a lower clock because of:

- controller output timing;
- board trace delay;
- MISO signal quality;
- level-shifting or series components;
- dummy cycles;
- library implementation.

### Frequency qualification procedure

Test a sequence such as:

```text
10 MHz -> 20 MHz -> 27 MHz -> 40 MHz -> higher experimental values
```

At each frequency:

- repeat full-screen fills;
- draw fine checkerboards;
- render text and diagonal lines;
- perform long-duration updates;
- activate Wi-Fi;
- operate SD and touch where applicable;
- observe corruption and resets;
- record ambient temperature and supply voltage.

The highest frequency that works once is not the engineering limit. Use a sustained, repeatable value with margin.

## 14. Theoretical transfer time

For RGB565, a full frame contains 1,228,800 data bits before protocol overhead.

At a nominal 40-MHz serial bit rate, the ideal raw transfer time is approximately:

```text
1,228,800 / 40,000,000 ≈ 30.7 ms
```

This ideal value excludes:

- command bytes;
- address-window setup;
- CS/DC transitions;
- software overhead;
- memory copies;
- task scheduling;
- bus arbitration;
- controller wait states;
- partial inefficiencies.

Therefore, measured full-screen frame rate will be lower than the raw theoretical maximum.

## 15. Throughput optimization

Useful optimization principles include:

- update only changed regions;
- combine adjacent drawing operations;
- keep CS asserted during large transfers where supported;
- use block writes instead of per-pixel calls;
- use line buffers or sprites;
- avoid repeated color conversions;
- decode images directly into transfer-sized blocks;
- limit unnecessary readback;
- measure real application workload rather than only a synthetic fill test.

Optimization should not compromise controller portability or hide timing assumptions.

## 16. DMA considerations

DMA can reduce CPU involvement during large SPI transfers, but it introduces constraints.

### Potential benefits

- faster block transfer;
- lower CPU occupancy;
- overlap of rendering and transfer;
- smoother animation.

### Potential constraints

- source buffer must reside in DMA-capable memory;
- alignment and length requirements may apply;
- buffer lifetime must exceed transfer completion;
- bus ownership must be coordinated;
- simultaneous SD, touch, or other SPI operations require serialization;
- library support depends on framework and configuration.

### Safe DMA pattern

```text
prepare buffer A
start DMA transfer A
prepare buffer B while A is active
wait or synchronize
start DMA transfer B
repeat
```

Do not modify or release a buffer while the peripheral still owns it.

## 17. Bus sharing and isolation

The current pin configuration assigns a dedicated signal set to the TFT, but the complete board netlist is not yet reconstructed.

When multiple peripherals share an SPI controller or physical lines, each inactive device must:

- have CS deasserted;
- release MISO;
- tolerate clock activity intended for another device;
- not be reset into a bus-driving state.

Potential symptoms of poor sharing include:

- display corruption during SD access;
- touch failures during drawing;
- invalid SD initialization after display startup;
- MISO contention;
- speed sensitivity.

A software architecture should explicitly own and serialize each bus transaction.

## 18. ESP32 strapping-pin implications

The working display profile uses GPIO2, GPIO12, and GPIO15, which have startup significance on classic ESP32 designs.

The carrier board is designed to boot with its onboard display circuitry attached, but external modifications can disturb this balance.

Avoid adding:

- strong pull-ups or pull-downs;
- powered peripherals that drive these pins during reset;
- large capacitors that alter startup timing;
- level shifters with incompatible idle states.

If boot failures appear only after an external modification, inspect these lines first.

## 19. Power interactions

Display faults may originate in the power system rather than the SPI software.

### Possible power-related symptoms

- white screen during Wi-Fi startup;
- display reset during SD writes;
- backlight flicker during audio peaks;
- color corruption at high brightness;
- successful operation from one USB cable but not another;
- failure only in a warm enclosure.

Measure:

- USB 5 V at the board;
- both AMS1117 outputs;
- display logic rail;
- backlight rail;
- GPIO21 waveform;
- voltage during combined display, Wi-Fi, SD, and audio load.

## 20. Signal-integrity measurement

At higher SPI frequencies, use short probing methods.

Observe:

- SCLK rise and fall quality;
- overshoot and undershoot;
- ringing;
- MOSI setup and hold relative to SCLK;
- CS stability;
- D/C transitions near command boundaries;
- MISO level and timing during reads;
- ground bounce during backlight or radio current changes.

A long oscilloscope ground lead can create misleading ringing. Use a ground spring or very short reference connection.

## 21. Minimal display test sequence

A good initial test should:

1. configure GPIO21 and enable the backlight;
2. initialize the TFT using the current profile;
3. display the controller profile name;
4. fill black, white, red, green, and blue;
5. draw a one-pixel border;
6. draw horizontal and vertical gradients;
7. render small and large text;
8. test rotations 0–3;
9. measure each full-screen fill time;
10. keep the pattern visible for photographic verification.

Serial output should log:

- build date;
- configured controller profile;
- pin assignment;
- SPI frequency;
- rotation;
- measured fill times;
- optional readback values;
- any detected errors.

## 22. Extended stress test

Run a longer test that combines:

- continuous region updates;
- periodic full-screen fills;
- Wi-Fi traffic;
- touch polling;
- SD-card reads;
- backlight PWM;
- optional audio output.

Record:

- corruption count;
- resets;
- minimum observed rail voltage;
- regulator temperature;
- average and worst update time;
- test duration;
- firmware commit.

## 23. Diagnostic flow

### Backlight is off

1. confirm board power;
2. set GPIO21 to the configured on level;
3. measure GPIO21;
4. inspect backlight supply;
5. check switching components;
6. verify no external P3 device is loading IO21.

### Backlight is on but screen is white

1. verify CS, SCLK, MOSI, and D/C activity;
2. reduce SPI frequency;
3. confirm driver selection;
4. check reset timing;
5. verify display logic rail;
6. try a known minimal initialization sequence.

### Wrong colors

1. display pure red, green, and blue;
2. toggle RGB/BGR order;
3. verify pixel format;
4. confirm byte order in image data;
5. inspect color-conversion code.

### Image shifted or clipped

1. draw the one-pixel border;
2. confirm width and height;
3. verify rotation;
4. verify address-window coordinates;
5. test controller offsets if using a non-ILI9341-compatible variant.

### Random corruption at high speed

1. lower SPI frequency;
2. use a better USB cable or bench supply;
3. shorten probes and external wiring;
4. serialize other bus users;
5. inspect SCLK ringing;
6. test without Wi-Fi, SD, touch, and audio, then re-enable one at a time.

### Board fails to boot after display modification

1. remove external additions from GPIO2, GPIO12, and GPIO15;
2. check strapping levels during reset;
3. inspect powered external devices;
4. restore original pull network;
5. test manual BOOT/RESET sequence.

## 24. Display verification record

For each tested board revision, record:

```text
PCB marking:
Board photographs:
Display flex marking:
Configured controller:
Controller ID result:
ID-read method:
Width × height:
TFT SCLK:
TFT MOSI:
TFT MISO:
TFT CS:
TFT D/C:
TFT reset behavior:
Backlight pin:
Backlight active level:
Backlight rail:
Stable write frequency:
Stable read frequency:
RGB/BGR setting:
Rotation results:
DMA tested:
Power source:
Ambient temperature:
Firmware commit:
Notes:
```

## 25. Confirmed versus open items

### Confirmed or currently established

- physical 2.8-inch-class TFT assembly;
- 240 × 320 working project profile;
- resistive touch overlay with separate XPT2046 controller;
- repository pin profile: GPIO12, 13, 14, 15, 2, and 21;
- current ILI9341-compatible software configuration;
- current 40-MHz write and 20-MHz read settings;
- GPIO21 is treated as active-high backlight control in the project.

### Still open

- exact physical TFT controller identity;
- controller revision and manufacturer;
- validity of the MISO/readback path;
- hardware reset topology;
- backlight switching circuit and supply rail;
- maximum stable write/read frequency with engineering margin;
- exact power consumption of logic and backlight;
- behavior across other ESP32-2432S028/R production batches;
- full bus-sharing topology;
- verified DMA performance and constraints on this repository version.

## 26. Completion criteria

This chapter can be promoted from working reference to measurement-verified reference when:

- the display controller is identified or explicitly certified only as register-compatible;
- pin routing is continuity-tested or otherwise strongly verified;
- reset behavior is traced;
- backlight power and switching topology are mapped;
- stable read and write frequencies are measured;
- all four rotations and color order are documented;
- full-screen transfer timing is recorded;
- combined-load stress testing passes;
- results include board revision, firmware commit, instruments, and test conditions.

## 27. Current engineering conclusion

The photographed ESP32-2432S028 sample is presently supported by a coherent ILI9341-compatible 240 × 320 TFT_eSPI profile using GPIO14/13/12 for clock and data, GPIO15 for chip-select, GPIO2 for command/data selection, and GPIO21 for active-high backlight control.

That profile is sufficiently concrete for software development and repeatable testing. It is not yet sufficient to claim a manufacturer-confirmed ILI9341 BOM, an independently controlled reset line, a guaranteed 40-MHz limit, or a known backlight power topology. Those claims remain reserved for direct readback, continuity tracing, electrical measurement, or physical display-marking evidence.
