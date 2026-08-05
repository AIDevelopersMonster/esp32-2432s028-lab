# On-board I/O System — ESP32-2432S028

> **Document status:** sample-specific reference for the author’s `ESP32-2432S028` board.  
> **Evidence rule:** visible hardware, repository code, and later electrical measurements are kept separate. A working GPIO definition is not treated as a traced net until it is continuity-tested or observed electrically.  
> **Related chapters:** [Hardware Overview](01-hardware-overview.md), [PCB Walkthrough](03-pcb-walkthrough.md), [Connector Reference](04-connector-reference.md), [Power System](05-power-system.md), and [Display System](06-display-system.md).

![ESP32-2432S028 board overview](../../hardware/images/board-overview.jpg)

## 1. Scope

This chapter documents the board-local user I/O that is neither part of the TFT, touch, storage, audio, nor external-connector subsystems:

- three-channel RGB status LED;
- photoresistor / light-dependent resistor input;
- BOOT button;
- RESET button and EN/reset behavior;
- active levels and startup states;
- ADC behavior and calibration limits;
- safe software interfaces;
- interaction with ESP32 boot strapping;
- diagnostics and verification procedures.

The current repository profile defines:

```cpp
LED red   -> GPIO4
LED green -> GPIO16
LED blue  -> GPIO17
LDR       -> GPIO34
BOOT      -> GPIO0
```

It also treats all three RGB channels as active LOW and the BOOT button as active LOW with `INPUT_PULLUP`.

These definitions are the current implementation baseline. They remain subject to sample-specific electrical verification.

## 2. Evidence status summary

| Function | Current repository profile | Evidence status |
|---|---:|---|
| RGB red channel | GPIO4 | software baseline; continuity confirmation pending |
| RGB green channel | GPIO16 | software baseline; continuity confirmation pending |
| RGB blue channel | GPIO17 | software baseline; continuity confirmation pending |
| RGB active level | LOW = on | software-tested assumption; electrical confirmation pending |
| LDR analog input | GPIO34 | software baseline; divider topology and polarity pending |
| BOOT button | GPIO0 | function strongly supported by ESP32 programming behavior; sample continuity still useful |
| BOOT active level | LOW = pressed | expected and used by current software |
| RESET button | ESP32 EN/reset path | function established; exact passive network and button routing open |

## 3. Functional block diagram

```text
                        ESP-WROOM-32
                             |
          +------------------+------------------+
          |                  |                  |
       GPIO4              GPIO16             GPIO17
          |                  |                  |
   red current path    green current path   blue current path
          \                  |                  /
           +------------- RGB LED --------------+

3.3 V or GND
    |
  LDR + fixed resistor divider
    |
  GPIO34 ADC input

BOOT switch
    |
  GPIO0 ---- pull-up / boot-strapping network

RESET switch / USB auto-reset network
    |
  EN pin ---- pull-up / capacitor / transistor control
```

The diagram is functional. Exact resistor values, common-anode/common-cathode LED construction, and LDR-divider polarity must be measured rather than inferred.

## 4. RGB LED subsystem

### 4.1 Purpose

The on-board RGB LED is suitable for:

- power-up state indication;
- firmware status;
- error codes;
- Wi-Fi connection state;
- storage or activity indication;
- visual diagnostics during board bring-up.

It is not a calibrated light source and should not be treated as a colorimetric device.

### 4.2 Current software mapping

```cpp
constexpr uint8_t LED_RED_PIN   = 4;
constexpr uint8_t LED_GREEN_PIN = 16;
constexpr uint8_t LED_BLUE_PIN  = 17;
```

The helper function currently applies inverted logic:

```cpp
digitalWrite(pin, enabled ? LOW : HIGH);
```

Therefore the working model is:

| GPIO output | Channel state |
|---:|---|
| LOW | on |
| HIGH | off |

### 4.3 Likely hardware topology

Active-LOW behavior is consistent with a common-anode LED or another arrangement in which each ESP32 pin sinks current through a channel resistor.

Possible implementation:

```text
3.3 V
  |
 common LED node
  |-- red die   -- resistor -- GPIO4
  |-- green die -- resistor -- GPIO16
  `-- blue die  -- resistor -- GPIO17
```

This topology is plausible but must not be presented as proven until continuity and voltage measurements confirm it.

### 4.4 What must be verified

For each channel:

1. continuity from the GPIO to the corresponding series resistor;
2. continuity from the resistor to one LED terminal;
3. common LED-terminal connection;
4. channel color;
5. active polarity;
6. resistor value;
7. current at 3.3-V operation;
8. startup state during reset and boot.

### 4.5 Safe first test

Use this sequence:

1. configure all three pins as outputs;
2. write HIGH to all channels before any animation;
3. enable one channel at a time by writing LOW;
4. observe color and brightness;
5. disable the channel before enabling the next one;
6. test combinations only after individual channels are identified.

This avoids an unintended full-white high-current state during startup.

### 4.6 GPIO4 boot-strapping caution

GPIO4 is an ESP32 strapping-related pin on classic ESP32 devices. External circuitry can influence its reset-time level.

The LED network may therefore affect or be affected by:

- power-up state;
- reset state;
- external probing;
- replacement LED or resistor values;
- additional circuitry connected to GPIO4.

Do not attach a low-impedance external driver to GPIO4 without checking the reset-time level and boot behavior.

### 4.7 GPIO16 and GPIO17 considerations

GPIO16 and GPIO17 are ordinary output-capable GPIOs on the classic ESP32 module configuration used here, but they may also be used by software peripherals.

Potential conflicts include:

- secondary UART assignments;
- custom serial protocols;
- timing-sensitive LED effects;
- external wiring added directly to the module pins.

The on-board LED load remains present even if firmware reassigns these GPIOs.

### 4.8 PWM brightness control

Brightness can be controlled using ESP32 LEDC PWM.

For an active-LOW channel:

```text
higher low-time duty cycle -> brighter LED
higher numeric duty may mean dimmer or brighter depending on software inversion
```

A robust API should expose logical brightness rather than raw inverted duty values.

Example conceptual conversion for an `N`-bit PWM range:

```text
hardwareDuty = maxDuty - requestedBrightness
```

### 4.9 Recommended PWM test

For each color:

1. hold the other channels off;
2. sweep logical brightness from 0 to maximum;
3. record the lowest visibly stable level;
4. check for flicker with a camera and by eye;
5. measure GPIO waveform;
6. confirm that duty-cycle inversion is correct;
7. verify that PWM operation does not disturb boot, UART, touch, or audio.

### 4.10 Mixed-color behavior

RGB mixing is not linear because:

- each die has different luminous efficiency;
- resistor values may differ;
- the eye has nonlinear sensitivity;
- PWM duty does not directly equal perceived brightness;
- supply voltage and temperature affect output.

A nominal `(255,255,255)` command may not appear neutral white.

For visually balanced colors, store empirical correction factors per channel.

### 4.11 LED current measurement

Measure channel current using one of these methods:

- voltage drop across the known series resistor;
- a small series ammeter during isolated testing;
- current-probe measurement;
- supply-current delta with only one channel changing.

If the resistor value is known:

```text
ILED = VRESISTOR / RSERIES
```

Do not infer LED current solely from a generic ESP32 GPIO limit.

### 4.12 Startup glitches

During reset, GPIOs may be inputs or follow strapping behavior before firmware configures them. Brief LED flashes can therefore occur.

Document:

- LED state at power application;
- LED state while EN is low;
- LED state during ROM boot;
- LED state during firmware upload;
- time until firmware forces all channels off.

A startup flash is not necessarily a firmware bug.

## 5. Light sensor / photoresistor subsystem

### 5.1 Current software mapping

The repository currently defines:

```cpp
constexpr uint8_t LDR_PIN = 34;
```

and reads it with:

```cpp
analogRead(LDR_PIN)
```

GPIO34 is an input-only ESP32 pin, which is appropriate for an analog sensor.

### 5.2 Likely electrical form

The photoresistor is expected to form a voltage divider with a fixed resistor:

```text
3.3 V
  |
 LDR or fixed resistor
  |
  +------> GPIO34 ADC
  |
 fixed resistor or LDR
  |
 GND
```

Two opposite polarities are possible:

- ADC reading increases with light;
- ADC reading decreases with light.

The direction must be determined experimentally.

### 5.3 The LDR is not a lux meter

Raw ADC values depend on:

- LDR resistance curve;
- fixed resistor value;
- supply voltage;
- ADC reference variation;
- ESP32 ADC nonlinearity;
- attenuation setting;
- board temperature;
- nearby LED and TFT illumination;
- enclosure geometry;
- angle of incoming light.

Without calibration against a reference instrument, use the sensor for relative brightness only.

### 5.4 GPIO34 electrical characteristics relevant here

For this board application:

- GPIO34 is input-only;
- internal pull-up and pull-down support should not be assumed for the sensor network;
- the board’s external divider provides the electrical bias;
- the node must remain within the 3.3-V input domain;
- external injection above the rail can damage or back-power the ESP32.

### 5.5 ADC resolution

The example firmware calls:

```cpp
analogReadResolution(12);
```

A nominal 12-bit result spans approximately:

```text
0 ... 4095
```

This does not imply 12-bit absolute accuracy.

Resolution, repeatability, linearity, and calibration are different properties.

### 5.6 ADC attenuation

The ESP32 ADC input range depends on the configured attenuation. The current example does not explicitly document an attenuation choice.

Therefore record:

- Arduino-ESP32 core version;
- default attenuation in that version;
- explicit attenuation if later configured;
- observed minimum and maximum raw values;
- measured sensor-node voltage at those values.

### 5.7 ADC1 and Wi-Fi interaction

GPIO34 belongs to the ADC1-capable input group on the classic ESP32, making it preferable to ADC2 pins when Wi-Fi is active.

Even so, Wi-Fi can still indirectly affect readings through:

- supply noise;
- ground bounce;
- regulator heating;
- task scheduling;
- RF coupling;
- TFT and backlight current changes.

The sensor should be tested both with radio disabled and under active Wi-Fi traffic.

### 5.8 Basic polarity test

1. cover the sensor completely;
2. record at least 100 samples;
3. illuminate it with a stable lamp;
4. record another 100 samples;
5. compare medians;
6. repeat at several distances;
7. note whether the raw value rises or falls with illumination.

### 5.9 Noise characterization

For each lighting condition, calculate:

- minimum;
- maximum;
- mean;
- median;
- standard deviation;
- peak-to-peak noise;
- occasional outliers.

Test with:

- TFT static;
- TFT full-screen updates;
- RGB LED off;
- each RGB channel on;
- Wi-Fi off and transmitting;
- microSD idle and writing;
- audio silent and active.

### 5.10 Filtering

Useful filters include:

- moving average;
- median filter;
- exponential moving average;
- hysteresis thresholds;
- rate limiting.

For simple automatic backlight control, a filtered value plus hysteresis is generally better than reacting to every raw sample.

Example state logic:

```text
if filtered < darkThreshold:
    enter DARK state
else if filtered > brightThreshold:
    enter BRIGHT state
else:
    retain previous state
```

Use two thresholds to prevent rapid toggling near a boundary.

### 5.11 Sampling rate

Ambient light changes slowly in most applications. Very high sampling rates provide little benefit and may increase noise sensitivity.

A practical starting point is:

- raw samples at tens of samples per second;
- filtered user-visible updates several times per second;
- slower updates for automatic brightness control.

The final rate should be selected from application behavior rather than copied from a generic example.

### 5.12 Self-illumination effects

The board’s own display backlight and RGB LED can illuminate the LDR.

Test for feedback by changing:

- TFT backlight level;
- screen content;
- RGB LED color and brightness;
- enclosure state.

An automatic backlight algorithm may otherwise create an oscillation:

```text
backlight increases
 -> sensor sees more board-generated light
 -> firmware interprets environment as brighter
 -> backlight changes again
```

Mechanical shielding or software compensation may be required.

### 5.13 Approximate voltage conversion

A simple uncalibrated estimate is:

```text
VADC ≈ raw / fullScale × Vreference
```

However, the ESP32 ADC should not be treated as an ideal linear converter. Prefer calibrated conversion APIs or direct multimeter correlation when absolute voltage matters.

### 5.14 Divider-resistance measurement

With power removed:

1. measure resistance from the ADC node to ground in darkness;
2. repeat under strong illumination;
3. measure toward the 3.3-V side where accessible;
4. identify the fixed-resistor value from marking or measurement;
5. infer divider polarity;
6. verify under powered voltage measurements.

Avoid using an ohmmeter on a powered board.

## 6. BOOT button

### 6.1 Function

The BOOT button is associated with ESP32 GPIO0.

Holding GPIO0 low while the ESP32 leaves reset selects the ROM serial bootloader rather than normal Flash boot.

### 6.2 Current software behavior

The repository configures:

```cpp
pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
```

and treats:

```cpp
digitalRead(GPIO0) == LOW
```

as pressed.

### 6.3 BOOT is not an ordinary unrestricted button

GPIO0 is a boot-strapping pin. Application firmware may read it as a user input after startup, but its reset-time state affects boot mode.

Consequences:

- holding BOOT during reset can prevent normal application boot;
- external circuitry on GPIO0 can force download mode;
- a stuck button can appear as a firmware or Flash failure;
- aggressive debounce capacitors or low-value pull resistors can interfere with programming.

### 6.4 Manual programming sequence

When automatic programming fails:

1. hold BOOT;
2. start the upload or pulse RESET;
3. release BOOT after the ROM bootloader starts;
4. complete upload;
5. reset normally.

Exact timing can vary with the upload tool and auto-reset circuit.

### 6.5 Using BOOT as an application input

Safe uses include:

- entering a settings screen after normal boot;
- requesting a factory-reset prompt;
- acknowledging an error;
- selecting a runtime mode.

Avoid actions that immediately destroy data merely because BOOT is held at power-up. The same condition is used for firmware recovery.

### 6.6 Debouncing

Mechanical button bounce can produce multiple transitions.

Possible software strategies:

- require a stable level for 20–50 ms;
- sample periodically and use a state machine;
- detect press and release separately;
- require a long press for destructive actions.

### 6.7 Long-press handling

A robust interface distinguishes:

- short press;
- long press;
- held during startup;
- repeated press;
- release after long press.

Log the duration rather than relying only on edge interrupts.

### 6.8 Interrupt use

GPIO0 can be used as an interrupt source after boot, but the ISR should remain minimal.

Recommended ISR behavior:

- capture a timestamp or set a flag;
- avoid display drawing;
- avoid serial printing;
- avoid filesystem access;
- process debounce in the main task.

### 6.9 Electrical verification

Measure:

- released voltage;
- pressed voltage;
- resistance to ground when pressed;
- pull-up resistance or effective bias;
- waveform during manual programming;
- waveform during normal reset;
- interaction with the CH340 auto-boot transistors.

## 7. RESET button and EN path

### 7.1 Function

The RESET control acts through the ESP32 `EN` pin.

Pulling EN low resets and disables the ESP32 core. Releasing it allows the module to start after the EN network charges to a valid high level.

### 7.2 RESET versus BOOT

| Control | Principal signal | Function |
|---|---|---|
| RESET | EN | restarts the ESP32 |
| BOOT | GPIO0 | selects boot mode when sampled during reset |

Pressing BOOT alone during normal operation does not reset the board. Pressing RESET alone normally boots from Flash. Holding BOOT while resetting selects the ROM loader.

### 7.3 Auto-reset interaction

The CH340G area likely drives EN and GPIO0 through a transistor network for automatic upload.

The exact network remains to be documented, including:

- transistor types;
- resistor values;
- which CH340 control outputs are used;
- inversion;
- pulse timing;
- EN capacitor value;
- behavior when the serial port opens.

### 7.4 Reset-source distinction

A board restart can originate from:

- physical RESET button;
- upload-tool control;
- software restart;
- watchdog;
- brownout;
- power interruption;
- EN noise;
- external circuitry.

Firmware should print or log the reset reason during diagnostics.

### 7.5 Reset waveform test

Observe EN and GPIO0 simultaneously during:

1. power-up;
2. RESET-button press and release;
3. BOOT held plus RESET;
4. automatic upload initiation;
5. serial-port open and close;
6. brownout or intentional supply reduction.

Record:

- low pulse duration;
- rise time;
- overshoot;
- ringing;
- final high voltage;
- GPIO0 level at the EN rising threshold.

### 7.6 External reset connection

Do not connect an external push-pull output directly to EN without understanding the onboard network.

Safer options include:

- open-drain/open-collector pull-down;
- isolated transistor;
- series resistance;
- explicit shared-ground design.

The external circuit must release EN cleanly and must not drive above 3.3 V.

## 8. GPIO startup and active-level discipline

### 8.1 Configure safe states early

At application startup:

1. define RGB pins;
2. write their inactive level;
3. configure outputs;
4. configure BOOT input;
5. configure LDR input;
6. only then start animations or sensor-driven behavior.

On some frameworks it is preferable to write the intended output latch before changing pin direction to minimize glitches.

### 8.2 Logical APIs instead of raw levels

Application code should use functions such as:

```cpp
setRgb(red, green, blue);
readLightRaw();
isBootPressed();
```

rather than scattering raw `digitalWrite()` and `digitalRead()` calls throughout the program.

This keeps polarity and pin mapping in one place.

### 8.3 Resource ownership

Assign one software owner to each GPIO.

Potential failures from multiple owners include:

- LED PWM overridden by a diagnostic task;
- GPIO16/17 reconfigured for UART;
- BOOT pin accidentally configured as output;
- ADC configuration changed by another library;
- low-power code disabling expected peripherals.

## 9. Suggested board-support API

A minimal interface may provide:

```cpp
void beginBasicHardware();
void setRgb(bool red, bool green, bool blue);
void setRgbBrightness(uint8_t red, uint8_t green, uint8_t blue);
void rgbOff();
uint16_t readLightRaw();
float readLightFiltered();
bool isBootPressed();
uint32_t bootPressDurationMs();
```

The API should document:

- active levels;
- valid call context;
- PWM channel allocation;
- ADC settings;
- filtering state;
- thread/task safety.

## 10. Minimal verification firmware

A focused test should avoid unrelated peripherals where possible.

Suggested sequence:

1. start serial output;
2. force RGB off;
3. print reset reason;
4. cycle red, green, blue, white, and off;
5. print GPIO output states;
6. sample LDR continuously;
7. show raw, mean, median, and range;
8. print BOOT press/release events and duration;
9. repeat while RESET is manually exercised;
10. store no persistent data during the initial test.

## 11. Combined on-screen self-test

The existing project self-test already demonstrates a useful pattern:

- TFT status display;
- RGB color cycling;
- LDR raw reading;
- BOOT-button state;
- serial logging.

For formal verification, extend it to show:

- current GPIO map;
- active-level interpretation;
- ADC attenuation and resolution;
- filtered light value;
- minimum/maximum samples;
- button press duration;
- reset reason;
- test firmware commit hash.

## 12. Continuity-mapping procedure

With power removed:

### 12.1 RGB channels

1. locate the RGB LED package;
2. locate nearby series resistors;
3. map each resistor to GPIO4, GPIO16, or GPIO17;
4. map resistor outputs to LED terminals;
5. identify the common terminal;
6. record resistor values.

### 12.2 LDR

1. identify both LDR terminals;
2. identify the divider resistor;
3. map the divider midpoint to GPIO34;
4. determine whether the LDR is above or below the midpoint;
5. map supply and ground;
6. inspect for a filter capacitor.

### 12.3 Buttons

1. map BOOT switch terminals to GPIO0 and ground;
2. map RESET switch to EN and ground;
3. identify pull-up resistors;
4. identify any capacitors;
5. map the auto-reset transistor connections.

## 13. Powered voltage measurements

Record at least:

| Node | Released / off | Active / pressed | Notes |
|---|---:|---:|---|
| GPIO4 |  |  | red channel |
| GPIO16 |  |  | green channel |
| GPIO17 |  |  | blue channel |
| RGB common |  |  | topology |
| GPIO34 dark |  | n/a | raw ADC also recorded |
| GPIO34 bright |  | n/a | raw ADC also recorded |
| GPIO0 released |  |  | BOOT |
| GPIO0 pressed |  |  | BOOT |
| EN normal |  |  | reset line |
| EN pressed |  |  | RESET |

## 14. Logic-analyzer and oscilloscope captures

Useful captures include:

- RGB PWM at several brightness levels;
- GPIO4 during power-up;
- GPIO0 and EN during manual bootloader entry;
- EN and GPIO0 during automatic upload;
- GPIO34 noise during TFT refresh;
- GPIO34 noise during Wi-Fi transmit;
- supply rail during full-white RGB output.

Use short probe grounds and avoid loading strapping nodes with low-impedance instruments.

## 15. Cross-subsystem interaction tests

### 15.1 RGB plus TFT

Verify that:

- color cycling does not disturb display output;
- TFT updates do not modulate LED brightness unexpectedly;
- shared power remains stable;
- GPIO mappings do not overlap.

### 15.2 LDR plus backlight

Measure sensor values for several backlight levels and screen contents.

### 15.3 LDR plus RGB

Measure how each LED channel affects the sensor in the intended enclosure.

### 15.4 BOOT plus microSD

GPIO0 and the microSD GPIO5 chip-select are both boot-sensitive board resources. Verify startup with:

- no card;
- several cards;
- BOOT released;
- BOOT held;
- USB upload;
- external connector loads attached.

### 15.5 I/O plus Wi-Fi and audio

Run RGB PWM, LDR acquisition, and button handling while:

- Wi-Fi transmits continuously;
- microSD writes;
- audio plays;
- TFT refreshes.

Check for missed button events, ADC shifts, LED flicker, and resets.

## 16. Low-power considerations

Before sleep:

- set RGB to the intended state;
- decide whether the LDR divider may remain powered;
- ensure BOOT and other wake sources are configured correctly;
- account for backlight and peripheral leakage;
- measure actual sleep current.

GPIO34 can be considered as a sensing input, but wake-source support and thresholding depend on the chosen ESP32 sleep mode and firmware architecture.

Do not assume that turning the RGB LED off makes the board low power; TFT backlight, regulators, CH340G, storage, and other circuits may dominate.

## 17. Fault diagnosis

### 17.1 One LED color never lights

Check:

1. GPIO mapping;
2. active polarity;
3. series resistor;
4. LED orientation and soldering;
5. pin direction;
6. PWM configuration;
7. alternate peripheral assignment;
8. continuity from ESP32 to LED.

### 17.2 LED color is wrong

Likely causes:

- channel-order mismatch;
- swapped software definitions;
- incorrect physical color identification;
- one failed die;
- mixed active-level assumptions.

### 17.3 All LED channels remain on

Check whether software assumes active HIGH when hardware is active LOW. Also inspect startup configuration and damaged GPIOs.

### 17.4 Board fails to boot after LED modification

Inspect GPIO4 reset-time loading and restore the original resistor/LED network before deeper firmware debugging.

### 17.5 LDR reading is fixed at zero

Check:

- GPIO selection;
- short to ground;
- divider continuity;
- broken LDR;
- ADC configuration;
- accidental digital-output configuration.

### 17.6 LDR reading is fixed near full scale

Check:

- short to 3.3 V;
- missing lower divider leg;
- wrong attenuation assumption;
- open sensor;
- incorrect pin mapping.

### 17.7 LDR value changes when RGB LED changes

This may be optical coupling rather than electrical failure. Test with opaque shielding before modifying the circuit.

### 17.8 BOOT always appears pressed

Check:

- stuck switch;
- GPIO0 short to ground;
- auto-boot transistor fault;
- external UART adapter control lines;
- incorrect pin mode;
- damaged pull-up.

### 17.9 Board enters download mode unexpectedly

Observe GPIO0 during reset and disconnect external circuits. Check the BOOT switch and CH340 control network.

### 17.10 RESET causes repeated boot loops

Check EN rise time, pull-up, capacitor, supply stability, and whether GPIO0 is low when EN rises.

### 17.11 Button events are duplicated

Implement debounce and avoid performing slow work directly in an interrupt handler.

## 18. Test matrix

| Test | RGB | LDR | BOOT | RESET | Other load |
|---|---|---|---|---|---|
| basic idle | off | sampled | sampled | manual | none |
| RGB channel ID | one at a time | logged | idle | none | none |
| RGB PWM | swept | logged | idle | none | none |
| dark/bright sensor | off | dark to bright | idle | none | none |
| self-light coupling | swept | logged | idle | none | TFT varied |
| button debounce | off | sampled | repeated press | none | none |
| manual bootloader | off | off | held | pressed | USB serial |
| upload automation | off | off | observed | observed | CH340 active |
| combined stress | animated | sampled | pressed | occasional | TFT/Wi-Fi/SD/audio |
| thermal enclosure | selected state | sampled | idle | none | full application |

## 19. Measurement record template

```text
Board marking / revision:
Firmware commit:
Arduino-ESP32 version:
Power source:
Ambient temperature:
Enclosure:

RGB
  red GPIO / active level / resistor / current:
  green GPIO / active level / resistor / current:
  blue GPIO / active level / resistor / current:
  common-anode or common-cathode:
  startup state:
  PWM frequency:

LDR
  GPIO:
  divider topology:
  fixed resistor:
  ADC resolution:
  attenuation:
  dark raw median:
  bright raw median:
  sensor-node voltages:
  noise and filtering:

BOOT
  GPIO:
  released voltage:
  pressed voltage:
  debounce interval:
  bootloader behavior:

RESET / EN
  released voltage:
  low pulse duration:
  rise time:
  auto-reset behavior:

Cross-system observations:
Artifacts / failures:
Evidence files:
```

## 20. Evidence levels

Use these labels in future updates:

- **Photo-confirmed** — component or control is visible;
- **Repository-defined** — current code assigns the pin or polarity;
- **Functionally tested** — firmware behavior observed on the author’s sample;
- **Continuity-confirmed** — unpowered electrical path measured;
- **Waveform-confirmed** — active behavior captured electrically;
- **Characterized** — current, ADC response, timing, and environmental behavior recorded.

Do not collapse these categories into a single “confirmed” label.

## 21. Current controlled claims

The repository currently uses:

- GPIO4, GPIO16, and GPIO17 for RGB channels;
- active-LOW channel control;
- GPIO34 for raw LDR acquisition;
- GPIO0 for the active-LOW BOOT button;
- the ESP32 EN path for reset;
- a combined board test that cycles RGB, reads the LDR, and reports BOOT state.

The board visibly contains user controls and indicator/sensor hardware consistent with those roles.

## 22. Current non-claims

This chapter does not yet claim:

- continuity-confirmed GPIO routing for every RGB channel;
- measured LED series-resistor values;
- common-anode or common-cathode construction;
- per-channel current or safe PWM limit;
- exact LDR-divider polarity;
- fixed-resistor value;
- calibrated lux conversion;
- ADC absolute accuracy;
- exact BOOT pull-up and debounce network;
- exact RESET/EN RC values;
- complete CH340 auto-reset transistor topology;
- glitch-free startup states;
- validated low-power behavior.

## 23. Completion criteria

The on-board I/O reference can be marked electrically complete when:

1. all RGB GPIO paths are continuity-mapped;
2. color order and active polarity are measured;
3. series resistors and channel currents are recorded;
4. startup and PWM waveforms are captured;
5. LDR-divider topology and resistor values are identified;
6. raw ADC response is characterized across useful lighting levels;
7. ADC settings and Wi-Fi interaction are documented;
8. BOOT and RESET nets are continuity-mapped;
9. EN/GPIO0 manual and automatic programming waveforms are captured;
10. debounce and long-press behavior are tested;
11. cross-subsystem stress tests complete without unexplained resets or corruption;
12. photographs, logs, source revision, and measurement conditions are archived.

Until then, the present chapter is a controlled implementation and verification guide rather than a substitute for a complete schematic.
