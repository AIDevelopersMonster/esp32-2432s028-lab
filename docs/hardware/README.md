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
8. [Audio System](08-audio-system.md) — 8002A amplifier path, current GPIO26 source profile, BTL-output safety, speaker selection, differential measurement, gain, distortion, power, thermal testing, and verification plan.
9. [Storage System](09-storage-system.md) — ESP-WROOM-32 program Flash, auxiliary W25Q32JV, microSD pin profile, safe backups, filesystem and card compatibility, throughput, corruption risks, and verification plan.
10. [On-board I/O System](10-onboard-io-system.md) — RGB LED, LDR input, BOOT and RESET controls, active levels, ADC behavior, boot-strapping cautions, software interfaces, diagnostics, and verification plan.
11. [USB Programming and Serial System](11-usb-programming-and-serial-system.md) — USB connector, CH340G, UART0, P1 service UART, automatic and manual bootloader entry, upload-speed qualification, serial monitoring, diagnostics, and verification plan.
12. [Expansion GPIO and Bus System](12-expansion-gpio-and-bus-system.md) — P3 and CN1, GPIO35/22/21/27 capabilities, 3.3-V compatibility, I2C/UART/SPI options, external powering, protection, cable effects, diagnostics, and verification plan.
13. [Wireless and Antenna System](13-wireless-and-antenna-system.md) — ESP-WROOM-32 Wi-Fi and Bluetooth capabilities, PCB antenna placement, enclosure and cable effects, power integrity, coexistence, security, range and throughput tests, diagnostics, and verification plan.
14. [Mechanical Integration and Enclosure System](14-mechanical-integration-and-enclosure-system.md) — controlled dimensional survey, mounting, display and touch opening, connector and button access, antenna clearance, cable and speaker placement, thermal paths, environmental protection, tolerance stack-up, diagnostics, and verification plan.
15. [System Pin Map and Resource Conflicts](15-system-pin-map-and-resource-conflicts.md) — consolidated GPIO ownership, input-only and boot-strapping restrictions, SPI/UART/I2C allocation, startup states, shared resources, safe reassignment gates, diagnostics, and verification plan.
16. [System Bring-Up and Acceptance Test](16-system-bring-up-and-acceptance-test.md) — receiving inspection, controlled first power-up, staged subsystem activation, golden firmware, automated self-test, power and thermal measurements, regression, fault isolation, release records, and acceptance criteria.
17. [Reliability, Maintenance and Lifecycle System](17-reliability-maintenance-and-lifecycle-system.md) — failure mechanisms, aging, preventive inspection, firmware and data recovery, connector and storage wear, spares, requalification triggers, maintenance records, retirement, and secure disposal.
18. [Diagnostics, Fault Recovery and Service Toolkit](18-diagnostics-fault-recovery-and-service-toolkit.md) — safe-mode entry, diagnostic firmware, serial and on-screen service consoles, staged subsystem isolation, logs, recovery images, test fixtures, guided troubleshooting, repair records, and return-to-service criteria.

## Evidence policy

Each claim should be distinguishable as one of the following:

- verified directly from the photographed board;
- verified by measurement or software test;
- taken from manufacturer documentation;
- typical for the CYD family but still pending verification on this sample.

Conflicting third-party pinouts must not silently override observations from the documented board sample.

## Photo policy

Photographs in `hardware/images/` are author-supplied images of the documented sample unless stated otherwise. Do not replace them with seller photographs without recording the source and license.
