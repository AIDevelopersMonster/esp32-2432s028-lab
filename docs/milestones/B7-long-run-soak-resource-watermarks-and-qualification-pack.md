# B7 — Long-Run Soak Harness, Resource-Watermark Telemetry and Reproducible Qualification Pack

## Status

- Repository implementation: complete.
- Fixture/parser validation: included in CI.
- Physical long-run soak: required.
- Environmental or field qualification: not claimed.

## Purpose

B7 turns the B6 service firmware into a repeatable long-run observation harness. It records resource watermarks and persistence activity without redefining the board pin allocation or weakening the SPI ownership rules established in B1–B6.

## Runtime telemetry

Every five seconds the firmware emits one `B7` record with:

- sequence and uptime;
- boot count and healthy-marker state;
- current free heap;
- minimum observed free heap reported by the ESP32 runtime;
- largest free 8-bit-capable heap block;
- loop-task stack high-water mark;
- resource-sample and watchdog-feed counters;
- microSD and bounded-archive state;
- chip-select and SPI ownership invariants.

The values are observations, not proof that all allocations, tasks or failure paths have been exercised.

## Host acceptance parser

`tools/parse_b7_soak.py` checks:

- required fields and declared board profile;
- monotonic sequence, uptime, sample and watchdog counters;
- released SPI ownership, zero ownership errors and software-visible idle chip selects;
- optional mandatory SD archive availability;
- nonzero largest block and stack watermark;
- a declared maximum heap drop from the first accepted sample;
- a declared minimum final uptime.

Example 24-hour laboratory gate:

```bash
python tools/parse_b7_soak.py capture.log \
  --min-records 17280 \
  --min-uptime-ms 86400000 \
  --require-sd \
  --max-heap-drop 32768
```

The threshold must be declared before the run. Changing it after seeing the result invalidates the qualification claim for that run.

## Reproducible evidence pack

`tools/build_b7_qualification_pack.py` creates a deterministic directory containing:

- the unmodified serial capture copied as `serial-capture.log`;
- `manifest.json` with board profile, operator-supplied board/card identities, firmware commit and SHA-256 of the copied capture;
- a short reproduction note.

Example:

```bash
python tools/build_b7_qualification_pack.py capture.log \
  --output qualification/B7-2026-08-06 \
  --board-id sample-a-01 \
  --firmware-commit <exact-git-sha> \
  --card-id <recorded-card-id>
```

The pack is inspectable and reproducible. It is not signed device attestation and is not tamper-proof.

## Required physical campaign

1. Use the documented ESP32-2432S028R sample and a recorded power source and microSD card.
2. Flash an exact commit and record the binary/build identity.
3. Start an uninterrupted serial capture before reset.
4. Run at least 24 hours for the initial laboratory gate.
5. Exercise touch, display refresh and archive rotation at declared intervals.
6. Record ambient conditions and any operator interaction.
7. Parse the untouched capture using thresholds declared before the run.
8. Build the qualification pack and retain the original capture separately.
9. Repeat on at least three cold starts before making a repeatability statement.

## Failure conditions

The run fails its declared gate if any of the following occurs:

- reset or uptime discontinuity not declared by the campaign;
- stalled watchdog or resource-sample counters;
- SPI ownership error or non-idle chip select observation;
- required archive unavailable;
- heap drop beyond the predeclared limit;
- zero or implausible resource watermark;
- missing records beyond the capture policy;
- archive corruption or inability to reproduce the pack.

## Evidence boundary

B7 does not claim:

- absence of every memory leak;
- bounded fragmentation under every workload;
- stack safety for every interrupt or task;
- thermal, EMC, ESD or vibration qualification;
- filesystem power-loss atomicity;
- microSD or flash endurance;
- authenticated evidence;
- universal compatibility across CYD variants;
- production or regulatory qualification.

## Claim ceiling

A successful campaign supports only this statement:

> The identified board, firmware commit, power source and card completed the declared soak interval while the captured B7 telemetry remained within the predeclared software-level resource and SPI-invariant limits.

## Next milestone

B8 — Release Candidate Freeze, Hardware Acceptance Matrix and Versioned Laboratory Baseline.
