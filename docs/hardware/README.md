# ESP32-2432S028 Hardware Reference

This directory contains a sample-specific hardware reference built from macro photographs, PCB markings, public component documentation, software tests, and later electrical measurements.

## Documents

1. [Hardware Overview](01-hardware-overview.md) — board identity, subsystem map, visible components, revision observations, cautions, and open verification tasks.
2. [PCB Component Atlas](02-pcb-component-atlas.md) — functional-zone component catalogue, package-marking evidence, board-level roles, electrical cautions, and verification tasks.
3. [PCB Walkthrough](03-pcb-walkthrough.md) — guided signal-flow tour from USB and power through ESP32, display, touch, storage, audio, controls, and expansion connectors.
4. [Connector Reference](04-connector-reference.md) — P1, P3, CN1 and P4 signal labels, electrical cautions, orientation rules, mechanical-family candidates, cable procurement, crimping, and fit-test protocol.
5. [Power System](05-power-system.md) — USB 5-V entry, dual AMS1117-3.3 regulators, provisional rail map, load domains, thermal loss, back-powering hazards, brownout analysis, and measurement plan.
6. [Display System](06-display-system.md) — TFT hardware path, current ILI9341-compatible profile, SPI/control signals, reset and backlight status, color and rotation tests, throughput, DMA, diagnostics, and controller-identification plan.
7. [Touch System](07-touch-system.md) — photo-confirmed XPT2046, current GPIO profile, dedicated SPI path, IRQ behavior, raw X/Y/Z acquisition, calibration, filtering, rotation mapping, stress testing, and verification plan.

## Evidence policy

Each claim should be distinguishable as one of the following:

- verified directly from the photographed board;
- verified by measurement or software test;
- taken from manufacturer documentation;
- typical for the CYD family but still pending verification on this sample.

Conflicting third-party pinouts must not silently override observations from the documented board sample.

## Photo policy

Photographs in `hardware/images/` are author-supplied images of the documented sample unless stated otherwise. Do not replace them with seller photographs without recording the source and license.
