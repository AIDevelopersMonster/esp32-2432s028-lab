# Touch System — ESP32-2432S028

> **Document status:** sample-specific touch-system reference for the author's `ESP32-2432S028` board.  
> **Evidence rule:** the XPT2046 package marking and repository pin profile are recorded separately from measurements that still need to be performed.  
> **Related chapters:** [Hardware Overview](01-hardware-overview.md), [PCB Component Atlas](02-pcb-component-atlas.md), [PCB Walkthrough](03-pcb-walkthrough.md), and [Display System](06-display-system.md).

![XPT2046 on the examined board](../../hardware/images/xpt2046.jpg)

## 1. Scope

This chapter documents the resistive-touch subsystem, the photo-confirmed XPT2046 controller, the current GPIO profile, the raw-data path, IRQ behavior, calibration, rotation mapping, pressure-related values, filtering, bus interaction, and a repeatable verification plan.

It does not treat the current software constants as a substitute for continuity measurements or sample-specific calibration.

## 2. Confirmed hardware

| Item | Evidence | Status |
|---|---|---|
| Touch panel | resistive overlay visible on the display assembly | Confirmed |
| Touch controller | package marked `XPT2046` at U3 | Confirmed |
| Interface type | serial controller with separate chip-select and interrupt | Function confirmed |
| Touch and TFT separation | separate controller and separate current GPIO profile | Confirmed at architecture level |

The XPT2046 handles only the resistive touch layer. It does not generate the TFT image.

## 3. Current repository pin profile

The current board support header defines:

| Signal | GPIO | Direction at ESP32 | Current status |
|---|---:|---|---|
| `TOUCH_IRQ` | 36 | input only | software profile; needs sample verification |
| `TOUCH_MOSI` | 32 | output | software profile; needs sample verification |
| `TOUCH_MISO` | 39 | input only | software profile; needs sample verification |
| `TOUCH_CLK` | 25 | output | software profile; needs sample verification |
| `TOUCH_CS` | 33 | output | software profile; needs sample verification |

The repository example creates a dedicated SPI instance and starts it with this exact pin set.

## 4. Functional signal path

```text
Finger or stylus pressure
        |
resistive overlay changes contact resistance
        |
X+, X-, Y+, Y- panel electrodes
        |
      XPT2046
        |
  CLK / MOSI / MISO / CS
        |
      ESP32
        |
 raw X, raw Y, pressure-related Z
        |
 filtering and calibration
        |
 screen coordinates
```

The IRQ line can indicate that the panel is being pressed, allowing the ESP32 to avoid continuous polling.

## 5. Four-wire resistive touch principle

A four-wire resistive panel contains two transparent resistive layers. One layer represents one axis and the other represents the perpendicular axis.

When untouched, the layers are separated. When pressed, they make electrical contact. The controller drives one axis and measures the other, then reverses the arrangement for the second coordinate.

Consequences:

- the panel responds to a finger, passive stylus, glove, or other non-sharp object;
- only one touch point is measured reliably;
- raw coordinates depend on panel resistance, pressure, temperature, and mounting;
- calibration belongs to the physical panel, not merely to the controller model;
- edge accuracy is usually worse than center accuracy.

## 6. XPT2046 role

The XPT2046 is an ADC-based resistive-touch controller. Its tasks include:

- driving the panel electrodes in the required sequence;
- sampling X and Y channels;
- providing pressure-related Z measurements;
- reporting conversion data over a serial interface;
- asserting a pen-interrupt output when configured and wired appropriately.

The controller normally returns raw ADC-scale values. Those values are not screen pixels.

## 7. Dedicated SPI bus

The current board profile assigns the touch controller to GPIO25, GPIO32, GPIO39, and GPIO33 rather than the TFT bus pins.

This means the current software design treats touch as a separate SPI bus:

```text
TFT bus:
  CLK  GPIO14
  MOSI GPIO13
  MISO GPIO12
  CS   GPIO15

Touch bus:
  CLK  GPIO25
  MOSI GPIO32
  MISO GPIO39
  CS   GPIO33
```

Advantages of a dedicated bus can include:

- simpler chip-select handling;
- reduced contention with large TFT transfers;
- independent clock selection;
- easier isolation during diagnosis.

The actual PCB routing should still be checked by continuity or logic-analyzer observation.

## 8. Input-only GPIOs

GPIO36 and GPIO39 are input-only on the classic ESP32.

That is consistent with their current assignments:

- GPIO36 as touch IRQ input;
- GPIO39 as touch MISO input.

Do not enable output mode on these pins. Internal pull-ups or pull-downs are also limited on classic ESP32 input-only pins, so the external circuit and library assumptions must be verified.

## 9. Chip-select discipline

`TOUCH_CS` isolates the XPT2046 from the bus.

Safe software behavior:

1. drive CS inactive before configuring the SPI peripheral;
2. assert CS only for one complete touch transaction;
3. release CS before another device uses the bus;
4. avoid leaving CS floating during reset;
5. verify idle polarity on the actual board.

Symptoms of incorrect CS handling include constant maximum readings, all-zero readings, random coordinates, or corruption when other SPI devices are active.

## 10. IRQ behavior

The current profile maps touch IRQ to GPIO36.

Potential behaviors that require measurement:

- active-low versus active-high assertion;
- whether the line remains asserted for the whole press;
- whether an external pull-up is fitted;
- whether the line chatters during light contact;
- whether IRQ is usable during all controller power modes;
- release delay after the finger is removed.

### Recommended IRQ test

Record the GPIO state in four conditions:

1. untouched;
2. light touch;
3. firm touch;
4. release.

Repeat while the TFT is idle and while it is being updated continuously.

## 11. Raw coordinate acquisition

The repository includes a raw touch example that:

- initializes the TFT;
- creates a dedicated `SPIClass` instance;
- initializes the XPT2046 library with CS and IRQ;
- calls `touch.touched()`;
- reads `TS_Point` with X, Y, and Z;
- prints raw values over serial.

This is an appropriate first diagnostic because it avoids hiding calibration errors behind a graphical UI.

## 12. Raw-value expectations

Do not assume that raw coordinates span exactly 0 to 4095 or that the panel reaches the converter endpoints.

Typical real behavior may include:

- a restricted usable range;
- reversed axes;
- swapped axes;
- non-zero offsets;
- compressed edge regions;
- unit-to-unit variation;
- pressure-dependent jitter.

The correct ranges must be measured on the author's sample.

## 13. Minimum raw-data test

Display or print at least:

```text
raw_x
raw_y
raw_z
irq_state
timestamp
```

Test points:

- four display corners;
- center;
- midpoint of each edge;
- slow horizontal sweep;
- slow vertical sweep;
- repeated taps at one location;
- light and firm pressure.

Store the results before applying filters.

## 14. Pressure-related Z values

Libraries often expose a `z` value derived from the controller's pressure-related channels.

Important limitations:

- it is not a calibrated force measurement;
- it depends on panel resistance and contact geometry;
- it varies with stylus shape and finger area;
- the useful threshold differs by board and library;
- a larger value does not automatically mean a linearly larger force.

Use Z primarily for contact qualification and noise rejection unless a dedicated calibration experiment is performed.

## 15. Touch detection strategy

A robust application can combine:

- IRQ state;
- minimum Z threshold;
- coordinate plausibility;
- temporal stability;
- release debounce.

Example decision chain:

```text
IRQ indicates contact?
  no  -> no touch
  yes -> read several samples
          |
          +-> Z below threshold -> reject
          +-> coordinates unstable -> retry or reject
          `-> stable sample -> calibrate and report
```

Do not rely on a single unfiltered ADC conversion for UI actions.

## 16. Filtering methods

Useful filters include:

### Median filter

Collect three to seven samples and choose the median for X and Y. This rejects isolated spikes without heavily delaying response.

### Trimmed mean

Sort a small sample set, discard the highest and lowest readings, and average the remainder.

### Stability window

Accept a touch only when successive samples remain within a defined raw-coordinate window.

### Exponential smoothing

Useful for drag motion, but may add lag and can smear fast taps.

### State-dependent filtering

Use stronger filtering on initial contact and lighter filtering during a confirmed drag.

## 17. Contact state machine

A practical touch driver should distinguish:

```text
IDLE
  -> PRESS_CANDIDATE
  -> PRESSED
  -> DRAGGING
  -> RELEASE_CANDIDATE
  -> IDLE
```

Benefits:

- prevents duplicate button presses;
- separates tap from drag;
- supports long-press timing;
- rejects IRQ chatter;
- allows different filters for acquisition and motion.

## 18. Calibration model

A basic affine calibration maps raw coordinates to display coordinates.

For an axis without rotation or skew:

```text
screen_x = (raw_x - raw_x_min) * width  / (raw_x_max - raw_x_min)
screen_y = (raw_y - raw_y_min) * height / (raw_y_max - raw_y_min)
```

However, the real panel may require:

- axis swap;
- axis inversion;
- separate coefficients for each display rotation;
- two-dimensional affine correction;
- edge clipping;
- nonlinear correction for high-accuracy work.

## 19. Five-point calibration

A practical minimum is five points:

1. near top-left;
2. near top-right;
3. center;
4. near bottom-left;
5. near bottom-right.

Do not place targets at the absolute glass edge. Leave a margin so that the user can press them accurately.

Record multiple samples per point and use the median.

## 20. Extended calibration grid

For higher confidence, use a 3 x 3 or 5 x 5 grid.

This reveals:

- axis nonlinearity;
- edge compression;
- local dead regions;
- systematic rotation error;
- panel mounting stress;
- whether a simple affine map is sufficient.

Plot residual error after calibration rather than judging only by eye.

## 21. Rotation mapping

Display rotation and touch rotation must be treated separately.

A correct display image does not prove correct touch mapping.

For every supported TFT rotation:

- draw labeled corner targets;
- touch each target;
- verify axis order;
- verify inversion;
- verify width and height limits;
- check center alignment;
- record the transformation used.

The current example calls `setRotation(1)` for both TFT and touch, but that is a software assumption to be tested on the physical sample.

## 22. Coordinate clipping

After calibration:

- reject implausible raw readings;
- clamp valid screen coordinates to `[0, width - 1]` and `[0, height - 1]`;
- do not convert a missing touch into coordinate `(0,0)`;
- preserve a separate valid/contact flag.

This prevents false presses in the top-left corner.

## 23. UI target sizing

Resistive touch is less precise than a mouse pointer.

For practical interfaces:

- use large buttons;
- separate neighboring targets;
- avoid critical controls at the extreme edge;
- provide visual press feedback;
- require confirmation for destructive actions;
- consider stylus use for dense interfaces.

The final target size should be chosen from measured touch error, not from display resolution alone.

## 24. Noise sources

Touch instability can be caused by:

- long SPI wiring or poor routing;
- insufficient grounding;
- power-rail noise;
- TFT update activity;
- Wi-Fi transmit bursts;
- SD-card writes;
- audio amplifier load changes;
- weak contact pressure;
- damaged overlay or flex;
- excessive SPI clock;
- incorrect library transaction settings.

## 25. Cross-subsystem stress testing

Touch must be tested during:

| Concurrent activity | Purpose |
|---|---|
| continuous TFT redraw | detect digital coupling and CPU starvation |
| Wi-Fi transmit | detect power and RF-related disturbance |
| microSD read/write | detect scheduling and supply interaction |
| active audio | detect rail modulation and interrupt load |
| PWM backlight changes | detect electrical or software coupling |

A touch driver that works only when all other peripherals are idle is not yet validated.

## 26. SPI clock qualification

The touch controller does not need the same clock rate as the TFT.

Qualification procedure:

1. begin with a conservative touch SPI clock;
2. log raw stability at fixed points;
3. increase the clock in controlled steps;
4. repeat under combined load;
5. select a value with margin below the first unstable setting.

Do not copy the TFT's 40-MHz value into the touch configuration.

## 27. Timing and sample rate

Higher sample rate can improve drag smoothness but increases:

- CPU load;
- SPI traffic;
- sensitivity to noise;
- duplicate event generation;
- power consumption.

For user interfaces, a moderate stable sample rate is usually more useful than maximum possible throughput.

Record:

- acquisition time per point;
- filtered report rate;
- IRQ-to-coordinate latency;
- missed-touch rate;
- CPU use during combined load.

## 28. Polling versus interrupt-driven operation

### Polling

Advantages:

- simple control flow;
- easy debugging;
- predictable periodic acquisition.

Disadvantages:

- wastes CPU when untouched;
- can interfere with rendering or networking;
- may miss short taps at a low poll rate.

### IRQ-assisted polling

IRQ wakes or schedules the touch acquisition, while coordinate conversion remains in normal task context.

This is often safer than performing SPI transactions directly inside the interrupt service routine.

## 29. Interrupt-service guidance

The ISR should remain short.

Recommended ISR action:

- capture a timestamp or set a volatile flag;
- notify a task or queue;
- return immediately.

Avoid inside the ISR:

- SPI transactions;
- serial printing;
- TFT drawing;
- dynamic allocation;
- long debounce delays.

## 30. Library behavior to verify

For the selected XPT2046 library, confirm:

- SPI mode;
- maximum configured clock;
- whether `begin(SPIClass&)` uses the provided bus;
- meaning and scaling of `z`;
- behavior of `touched()` with and without IRQ;
- rotation transformation;
- transaction locking;
- whether samples are filtered internally.

Documentation of the library version should be retained with test results.

## 31. Minimal verification sketch

A minimum sample should:

1. initialize Serial;
2. initialize the touch SPI bus explicitly;
3. initialize XPT2046 with CS and IRQ;
4. report IRQ state;
5. print raw X, Y, and Z;
6. avoid calibration initially;
7. avoid drawing large TFT regions during first diagnosis.

Once raw values are stable, add graphical markers and calibration.

## 32. Graphical verification sketch

A second-stage test should:

- draw crosshair targets;
- display raw and calibrated values;
- mark the reported point;
- clear only a small region rather than the whole screen;
- support all four rotations;
- log rejected samples and pressure thresholds;
- show current contact state.

## 33. Repeated-point precision test

Press the same target at least 20 times.

Record:

- mean screen coordinate;
- X and Y standard deviation;
- maximum error;
- rejected sample count;
- pressure distribution;
- stylus or finger used.

Repeat at center and near corners.

## 34. Drag-line test

Draw slow horizontal, vertical, and diagonal lines.

Look for:

- discontinuities;
- reversed segments;
- stair-step noise;
- axis coupling;
- edge clipping;
- lag from excessive smoothing;
- missed release events.

## 35. Long-duration test

Run touch acquisition for at least one hour while:

- updating the TFT;
- periodically using Wi-Fi;
- reading or writing microSD;
- varying backlight;
- optionally driving audio.

Record communication errors, false touches, stuck IRQ, reset events, and coordinate drift.

## 36. Failure diagnosis

### No touch detected

Check:

- XPT2046 supply;
- CS idle and activity;
- touch SPI clock;
- MOSI/MISO routing;
- IRQ level;
- library bus selection;
- panel flex connection.

### Constant X/Y values

Possible causes:

- wrong SPI mode;
- MISO not connected or wrong GPIO;
- CS not asserted;
- panel electrode failure;
- controller not powered.

### Coordinates move but are reversed

Likely a mapping issue: swap or invert axes, then verify all rotations.

### Random coordinates while untouched

Check IRQ logic, Z threshold, floating inputs, SPI noise, and whether the application is reading without confirmed contact.

### Touch fails during TFT updates

Check bus assignment, transaction locking, task scheduling, interrupt latency, and supply integrity.

### False corner presses

Ensure invalid samples are not converted to `(0,0)`, and maintain a separate validity flag.

## 37. Electrical probing plan

Useful observations:

- XPT2046 supply voltage;
- CS waveform;
- touch clock frequency and mode;
- MOSI command frames;
- MISO response;
- IRQ idle and active levels;
- rail behavior during touch plus TFT/Wi-Fi load.

Use short ground connections. Do not probe the fine-pitch panel flex carelessly.

## 38. Continuity-verification plan

With power removed:

1. confirm GPIO33 to XPT2046 CS;
2. confirm GPIO25 to clock;
3. confirm GPIO32 to controller data input;
4. confirm GPIO39 to controller data output;
5. confirm GPIO36 to IRQ;
6. identify supply and ground pins;
7. record series resistors or test points, if present.

Only after this should the pin mapping be promoted from software profile to electrically confirmed routing.

## 39. Calibration record template

```text
Board marking:
Board photo revision:
Display rotation:
Touch library and version:
SPI clock:
IRQ polarity:
Raw X minimum / maximum:
Raw Y minimum / maximum:
Z threshold:
Axis swap:
X inversion:
Y inversion:
Calibration coefficients:
Center residual error:
Maximum grid residual error:
Test stylus/finger:
Ambient temperature:
Date:
Firmware commit:
```

## 40. Evidence levels

### Level 1 — photo confirmed

- XPT2046 package marking;
- resistive overlay present.

### Level 2 — software profile confirmed

- current repository constants and example compile;
- library initializes with the selected API.

### Level 3 — functional sample confirmed

- stable raw X/Y/Z values from the author's board;
- IRQ behavior recorded;
- all four rotations mapped.

### Level 4 — electrically verified

- continuity confirms all GPIO routes;
- SPI timing measured;
- supply and IRQ electrical behavior recorded.

### Level 5 — application qualified

- calibration error quantified;
- long-duration combined-load test passed;
- UI thresholds and filtering frozen for the target enclosure.

## 41. Completion criteria

This chapter can be upgraded from provisional to verified when:

- all five current GPIO assignments are continuity-confirmed;
- touch SPI mode and qualified clock are recorded;
- IRQ polarity and pull arrangement are measured;
- raw ranges are recorded on the author's sample;
- pressure threshold is justified experimentally;
- rotation mapping is verified for all supported orientations;
- calibration residual error is quantified;
- combined TFT, Wi-Fi, SD, audio, and touch stress tests pass;
- firmware commit and library versions are recorded.

## 42. Current conclusions

The photographed board definitely includes a resistive touch panel controlled by a package marked XPT2046. The repository currently uses a dedicated touch SPI profile on GPIO25, GPIO32, GPIO39, GPIO33, and IRQ on GPIO36, with a raw-coordinate example already present.

Those facts provide a strong implementation baseline, but the final hardware reference still requires continuity confirmation, measured IRQ behavior, raw-range capture, calibration data, clock qualification, and combined-load testing on the author's actual board.
