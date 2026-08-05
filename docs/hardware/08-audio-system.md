# Audio System — ESP32-2432S028

> **Document status:** sample-specific audio reference for the author's `ESP32-2432S028` board.  
> **Evidence rule:** package markings, connector placement, and repository configuration are distinguished from unverified copper routing and measured performance.  
> **Related chapters:** [Hardware Overview](01-hardware-overview.md), [PCB Component Atlas](02-pcb-component-atlas.md), [PCB Walkthrough](03-pcb-walkthrough.md), [Connector Reference](04-connector-reference.md), and [Power System](05-power-system.md).

![8002A audio-amplifier area](../../hardware/images/ns8002a.jpg)

## 1. Scope

This chapter documents the visible audio amplifier, the current ESP32 audio-source assumption, the likely coupling and gain path, the P4 speaker output, safe measurement methods, speaker selection, distortion and output-power testing, thermal behavior, software-generation options, and the evidence required before publishing firm electrical specifications.

It is not a complete schematic and does not assign guaranteed output power from seller listings or from a generic 8002A-family datasheet.

## 2. Confirmed hardware

| Item | Evidence | Status |
|---|---|---|
| U5 | package marking readable as `8002A` / NS8002A-family | confirmed audio power-amplifier IC |
| P4 | two-position connector adjacent to U5 | confirmed speaker-output connector |
| ESP32 module | ESP-WROOM-32 marking | confirmed programmable audio source platform |
| Repository profile | `AUDIO_PIN = 26` | current software baseline, not yet continuity-verified |
| Output topology | two amplifier output conductors to P4 | strongly consistent with BTL output |

## 3. Working signal path

```text
ESP32 GPIO26 or another configured source
        |
  PWM / DAC-like waveform
        |
 input coupling and bias network
        |
      U5 8002A
      /       \
   OUT+       OUT-
      \       /
        P4
        |
   floating speaker
```

The exact resistor values, coupling-capacitor values, gain-setting topology, shutdown control, supply voltage, and route from GPIO26 to U5 remain open until traced.

## 4. Current repository audio pin

The board-support header currently defines:

```cpp
constexpr uint8_t AUDIO_PIN = 26;
```

This is the current implementation baseline. It should be treated as **software configuration**, not yet as an electrically proven net assignment.

Confirmation methods:

1. unpowered continuity from GPIO26 to the U5 input network;
2. oscilloscope observation of a generated test tone at the suspected input node;
3. temporary software reassignment showing that only GPIO26 drives the amplifier;
4. exact-revision schematic or verified board trace.

## 5. ESP32 audio-generation methods

Possible source methods include:

- hardware DAC output on a classic ESP32-capable pin;
- PWM followed by onboard or effective low-pass filtering;
- sigma-delta output;
- I2S-generated waveform routed through a suitable pin mode;
- timer-driven digital waveform for simple beeps.

The presence of `AUDIO_PIN = 26` does not by itself prove which peripheral mode is intended.

### Practical hierarchy

For bring-up:

1. start with a low-amplitude 1-kHz sine or conservative PWM tone;
2. confirm the input node and speaker output;
3. determine DC bias and coupling behavior;
4. measure distortion and clipping;
5. only then increase amplitude or implement streamed audio.

## 6. 8002A amplifier role

The 8002A family is commonly used as a small mono bridge-tied-load audio amplifier.

Expected board-level functions:

- voltage gain;
- current drive for a small speaker;
- differential output across P4;
- possible shutdown or enable control;
- operation from a low-voltage supply.

The exact clone, silicon revision, and datasheet matching the photographed marking have not yet been established.

## 7. Critical BTL rule

P4 must be treated as a bridge-tied-load output.

```text
P4 pin A = amplifier output A
P4 pin B = amplifier output B
speaker connects between A and B
```

Neither conductor is assumed to be ground.

### Never connect either P4 pin directly to

- board GND;
- oscilloscope protective earth;
- grounded audio analyzer input;
- common-ground headphones;
- a stereo amplifier input with shared return;
- the sleeve of a normal 3.5-mm audio jack.

Doing so can short one half of the bridge and damage U5, the measurement instrument, or the board.

## 8. Safe output measurement

Preferred methods:

### Method A — differential probe

Measure directly across the two P4 pins with a properly rated differential probe.

### Method B — two oscilloscope channels

- connect both probe grounds to board GND;
- probe P4 pin A with channel 1;
- probe P4 pin B with channel 2;
- display `CH1 - CH2` mathematically.

This is safe only when both channel inputs are ground-referenced in the normal way and neither probe ground is attached to P4.

### Method C — floating isolated instrument

An isolated battery-powered differential instrument may be used if its isolation rating and input range are known.

## 9. Speaker selection

A conservative starting point is a small dynamic speaker with:

- 8-ohm nominal impedance;
- power rating comfortably above the expected test level;
- short leads;
- no connection from either lead to chassis or ground.

A 4-ohm speaker may produce more output but also increases current, heating, distortion risk, and power-rail stress.

### Do not infer compatibility from diameter alone

Record:

- nominal impedance;
- rated power;
- DC resistance;
- enclosure type;
- test duration;
- measured output voltage;
- amplifier temperature.

## 10. Speaker impedance and power

For a sinusoidal differential output across a resistive approximation:

```text
PLOAD = VRMS^2 / RLOAD
```

For example, 1.0 V RMS across 8 ohms corresponds to 0.125 W.

Real speakers are reactive, frequency-dependent loads. This calculation is a first approximation, not a complete acoustic or thermal model.

## 11. Why seller power ratings are not board specifications

A quoted amplifier power may assume:

- a particular supply voltage;
- a particular load impedance;
- a defined distortion limit;
- ideal PCB heat spreading;
- a sine-wave test;
- short test duration;
- a genuine reference IC rather than a clone.

The assembled board may differ in supply voltage, gain, cooling, decoupling, and component quality. Therefore the repository should publish only measured board-level results.

## 12. Supply-voltage status

The U5 supply voltage has not yet been confirmed.

Possible cases include:

- direct 5-V-class supply;
- regulated 3.3-V supply;
- filtered branch of another rail.

Determine it by measuring U5 supply relative to board ground under idle and loaded conditions.

This matters because output swing, power, clipping, and dissipation depend strongly on supply voltage.

## 13. Input coupling and bias network

Small passives around U5 likely provide some combination of:

- AC coupling;
- input biasing;
- gain setting;
- RF filtering;
- supply decoupling;
- shutdown timing;
- pop-noise suppression.

Do not assign functions from placement alone. Trace each component before documenting its role.

## 14. Gain measurement

Measure gain using a low-distortion test tone.

1. apply or generate a known input waveform;
2. measure input amplitude at the actual U5 input node;
3. measure differential output across P4;
4. stay below clipping;
5. compute:

```text
AV = VOUT_RMS / VIN_RMS
```

Record frequency, load, supply voltage, and measurement bandwidth.

## 15. Clipping test

Use a 1-kHz sine wave and increase amplitude slowly.

Observe:

- differential output waveform;
- individual bridge outputs relative to ground;
- supply rail;
- input waveform;
- current consumption;
- U5 temperature.

Define clipping consistently, for example by visible waveform flattening or a measured THD threshold.

Do not use an audibly harsh sound as the only clipping criterion.

## 16. Distortion measurement

Useful metrics:

- THD;
- THD+N;
- signal-to-noise ratio;
- idle noise;
- channel output symmetry;
- residual PWM carrier;
- power-supply modulation.

At minimum, record output waveform and FFT at several amplitudes and frequencies.

Suggested test points:

- 100 Hz;
- 1 kHz;
- 5 kHz;
- low, medium, and near-clipping amplitude;
- 8-ohm and optionally 4-ohm loads.

## 17. Frequency response

The low-frequency response may be limited by coupling capacitors and speaker impedance. High-frequency response may be affected by source generation, filtering, amplifier bandwidth, wiring, and measurement setup.

Measure relative output level across a defined range, for example:

```text
100 Hz -> 200 Hz -> 500 Hz -> 1 kHz -> 2 kHz -> 5 kHz -> 10 kHz
```

Keep source amplitude low enough to avoid clipping at every frequency.

## 18. Pop and click behavior

Transient noises can occur during:

- power-up;
- reset;
- GPIO-mode changes;
- enabling PWM or DAC output;
- starting and stopping audio streams;
- amplifier shutdown transitions;
- brownout.

Mitigation may require:

- initializing the audio pin to a quiet midpoint before enabling output;
- ramping amplitude;
- using amplifier shutdown if available;
- avoiding abrupt DC changes;
- sequencing power and software initialization.

The exact quiet-start method must be validated on this board.

## 19. Idle behavior

With no intended audio, measure:

- DC voltage of each P4 pin relative to ground;
- differential DC voltage across P4;
- idle noise spectrum;
- idle current;
- response to Wi-Fi and display activity.

A BTL amplifier can show nonzero common-mode voltage on both outputs while the differential DC remains small.

## 20. Interaction with the power system

Audio load can expose power weaknesses through:

- voltage sag;
- USB-cable drop;
- regulator heating;
- ESP32 resets;
- display artifacts;
- microSD errors;
- touch noise;
- audible digital interference.

Test audio both alone and with:

- Wi-Fi transmit activity;
- full backlight;
- TFT updates;
- touch polling;
- SD writes;
- external loads.

## 21. Thermal testing

Measure temperature at:

- U5 package surface;
- nearby coupling and decoupling components;
- both AMS1117 regulators;
- ESP-WROOM-32;
- speaker connector and wires.

Test conditions should record:

- ambient temperature;
- enclosure state;
- speaker impedance;
- waveform type;
- output level;
- duration;
- supply voltage;
- total board current.

Stop if temperature rises rapidly, output becomes unstable, or the rail sags significantly.

## 22. Minimum safe bring-up procedure

1. visually inspect U5 and P4;
2. power from a current-limited 5-V source;
3. leave P4 unloaded initially;
4. measure U5 supply voltage;
5. generate a very low-level test tone on the configured source pin;
6. verify activity at the suspected U5 input;
7. attach an 8-ohm speaker;
8. monitor current and temperature;
9. measure differential P4 waveform;
10. increase amplitude gradually.

## 23. Recommended test firmware behavior

A useful test program should provide:

- selectable tone frequency;
- amplitude ramp;
- mute state;
- finite test duration;
- serial reporting of configuration;
- optional sweep mode;
- optional PWM/DAC mode selection;
- watchdog-safe operation;
- immediate return to mute after reset or command timeout.

A constant maximum-amplitude square wave is not a suitable first test.

## 24. PWM considerations

If PWM is used:

- carrier frequency should be well above the audible band where practical;
- duty-cycle centering must avoid DC steps;
- effective resolution decreases as carrier frequency rises;
- filtering and amplifier bandwidth determine residual carrier;
- ESP32 timer allocation may conflict with other peripherals.

Measure residual switching content rather than assuming it is inaudible or harmless.

## 25. DAC-like source considerations

For a classic ESP32 internal DAC-capable path:

- output is not precision hi-fi conversion;
- offset and nonlinearity may be significant;
- sample timing jitter and software scheduling matter;
- the amplifier input network may expect a biased signal;
- maximum code should not automatically be treated as maximum safe audio amplitude.

## 26. I2S considerations

I2S can improve timing regularity, but an external DAC is not visibly confirmed in this audio path.

If I2S is used to generate an internal or routed waveform, document:

- peripheral mode;
- selected output pin;
- sample rate;
- bit depth;
- DMA buffer size;
- conversion method from samples to the physical pin waveform.

## 27. Audio from files

For WAV or other stored audio:

- use a known sample format;
- avoid filesystem latency underruns;
- use buffering;
- scale samples to prevent clipping;
- define mono conversion explicitly;
- validate SD activity alongside amplifier operation.

Compressed formats add decoding load and should be tested separately from the basic analog path.

## 28. Common symptoms

### No sound

Check:

1. speaker continuity and impedance;
2. P4 connection;
3. U5 supply voltage;
4. source pin configuration;
5. activity at the amplifier input;
6. amplifier shutdown state;
7. differential output rather than output-to-ground only.

### Very quiet sound

Possible causes:

- low source amplitude;
- wrong source pin;
- incorrect bias;
- gain network issue;
- one output half inactive;
- high-impedance or damaged speaker;
- measuring only one bridge output.

### Harsh distortion

Possible causes:

- clipping;
- excessive source amplitude;
- 4-ohm load stress;
- power-supply sag;
- wrong waveform offset;
- residual PWM carrier;
- damaged speaker;
- overheating.

### Resets when sound starts

Investigate:

- USB cable drop;
- supply current limit;
- regulator temperature;
- speaker impedance;
- clipping and excessive duty cycle;
- combined Wi-Fi/display/SD load.

### Buzz linked to screen activity

Investigate:

- shared supply impedance;
- ground routing;
- PWM frequencies;
- TFT SPI coupling;
- insufficient decoupling;
- measurement grounding errors.

## 29. Continuity-mapping plan

With power removed:

1. identify U5 ground and supply pins;
2. map both output pins to P4;
3. trace the input pin through nearby passives;
4. test continuity toward GPIO26;
5. identify shutdown or enable pin routing;
6. record component designators and measured values where possible;
7. photograph probe points.

A low-resistance reading through a component is not the same as a direct copper connection.

## 30. Suggested measurement table

| Test | Supply | Load | Frequency | Input | Vout RMS | Power | Current | U5 temp | Result |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---|
| idle | TBD | none | — | muted | TBD | — | TBD | TBD | open |
| low level | TBD | 8 Ω | 1 kHz | TBD | TBD | TBD | TBD | TBD | open |
| medium | TBD | 8 Ω | 1 kHz | TBD | TBD | TBD | TBD | TBD | open |
| near clip | TBD | 8 Ω | 1 kHz | TBD | TBD | TBD | TBD | TBD | open |
| 4 Ω trial | TBD | 4 Ω | 1 kHz | TBD | TBD | TBD | TBD | TBD | optional |
| combined load | TBD | 8 Ω | program audio | TBD | TBD | TBD | TBD | TBD | open |

## 31. Evidence levels

### Level A — visible

- U5 marking;
- P4 presence;
- two-wire output connector;
- nearby passive network.

### Level B — configured

- repository constant `AUDIO_PIN = 26`;
- test firmware using the selected pin.

### Level C — electrically verified

- GPIO26-to-input continuity;
- U5 supply voltage;
- P4 output-pin mapping;
- shutdown topology;
- gain-network values.

### Level D — performance verified

- output power versus distortion;
- frequency response;
- noise;
- thermal stability;
- combined-load behavior;
- safe speaker range.

## 32. Completion criteria

The audio chapter can be marked fully verified when the repository contains:

- confirmed U5 pin and net map;
- confirmed source GPIO and source mode;
- measured supply voltage;
- confirmed P4 polarity labels or neutral A/B labels;
- verified BTL measurement procedure;
- measured gain;
- output-power and distortion results for at least one documented load;
- thermal data;
- tested startup/mute behavior;
- reproducible audio test firmware;
- photographs or diagrams of measurement points.

## 33. Current controlled conclusions

The photographed board contains an 8002A-family mono power amplifier and a two-wire speaker connector. The output should be handled as a floating BTL pair. The repository currently assigns GPIO26 as the audio source, but the route, generation mode, gain network, amplifier supply, output capability, and thermal limits remain to be verified by continuity tests and controlled measurements.
