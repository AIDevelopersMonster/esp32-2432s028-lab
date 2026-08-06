# B8 hardware acceptance operator protocol

Campaign: `b8-v1.0.0-rc1-cyd-r-sample-01`

## 0. Freeze and identity gate

1. Confirm that the tested source tree is release candidate `v1.0.0-rc1`.
2. Record the exact Git commit before building.
3. Build without modifying source, pin definitions, persistent schemas, telemetry fields or acceptance criteria.
4. Record SHA-256 of the resulting `firmware.bin`.
5. Label and photograph the physical board, power supply, USB-UART adapter and microSD card.
6. Complete all `TO_BE_RECORDED` fields in `campaign-manifest.json`.
7. Create evidence directories using the structure in `EVIDENCE-STRUCTURE.md`.
8. If firmware or acceptance criteria change after evidence collection begins, stop the campaign. Do not reuse the evidence for the changed binary; create a new RC or campaign.

Pass condition for HW-01: all identities and exact firmware hash are recorded and internally consistent.

## 1. Build and initial capture

```bash
pio run
sha256sum .pio/build/esp32-2432s028r/firmware.bin
pio run --target upload
pio device monitor --baud 115200 | tee qualification/boards/cyd-r-sample-01/evidence/HW-02/serial.log
```

Start capture before reset whenever a reset reason or boot transition is under test. Preserve raw logs; never edit them in place.

## 2. HW-02 display acceptance

1. Cold-start the board normally.
2. Verify 320x240 landscape orientation.
3. Verify the full status screen is readable, stable and not mirrored.
4. Observe for at least five minutes for corruption, random resets or persistent flicker.
5. Save one clear photograph and the corresponding serial capture.

PASS requires correct orientation and stable rendering on the identified board.

## 3. HW-03 touch calibration

1. Run the B3 calibration path and complete all four points.
2. Save the raw B3 serial capture.
3. Reset without erasing NVS.
4. Verify calibration is loaded rather than silently recreated.
5. Touch each screen quadrant and edges; verify mapped coordinates remain bounded and correspond to physical location.
6. Save a photograph or short operator note showing mapped-point confirmation.

PASS requires a persisted valid calibration and bounded mapping after reset.

## 4. HW-04 storage and bounded archive

1. Insert the identified microSD card and start a B5 capture.
2. Verify `sd_present=1` and `archive_writable=1`.
3. Run long enough to append records.
4. Continue until rotation is observed, or use a controlled accelerated laboratory run without changing the acceptance criterion.
5. Copy `/diag/b5-current.log` and `/diag/b5-previous.log` unchanged.
6. Run:

```bash
python tools/parse_b5_archive.py evidence/HW-04/b5-storage.log --min-records 30 --require-sd
```

7. Save parser output in `evidence/HW-04/parser-report.txt`.

PASS requires successful probe, append, bounded two-file behavior and parser acceptance.

## 5. HW-05 SPI invariants

Review accepted B2-B7 logs and confirm throughout the campaign:

- `owner=none` at record boundaries;
- `owner_errors=0`;
- `cs_idle=1`.

Create `evidence/HW-05/spi-summary.txt` listing each reviewed log and its result. Any ownership violation is FAIL until explained, corrected in a new RC and rerun.

## 6. HW-06 configuration transaction campaign

Use a controllable power switch. For every declared interruption phase, record at least three repetitions.

Required phases:

1. before inactive-slot write;
2. during inactive-slot write;
3. after inactive-slot write but before verification;
4. after verification but before active-marker update;
5. during marker update;
6. immediately after marker update;
7. one-slot invalidation or corruption test.

For each trial record:

- trial ID;
- prior generation;
- intended new generation;
- interruption phase;
- selected generation after restart;
- config state;
- recovery count;
- raw-log path;
- result.

Write results to `evidence/HW-06/power-cut-matrix.csv` and retain every raw B4 capture. PASS requires selection of only a checksum-valid previous or new generation; no mixed or unexplained state is permitted.

## 7. HW-07 boot ledger and containment

1. Start from a healthy normal boot and wait for the healthy marker.
2. Record the boot counter and healthy state.
3. Perform three separate early resets before the healthy window.
4. Confirm containment appears at the declared threshold.
5. Perform a normal uninterrupted recovery run and confirm the healthy marker clears the consecutive-unhealthy count as designed.
6. Hold BOOT during reset and verify explicit recovery mode remains distinguishable from automatic containment.

Record each reset in `evidence/HW-07/reset-campaign.csv`, with a raw log per boot. PASS requires declared ledger increments, healthy transition and containment behavior.

## 8. HW-08 watchdog fault injection

This destructive reset test must be performed only in recovery mode.

1. Start capture before reset and boot while holding BOOT.
2. Confirm recovery mode.
3. Save the pre-fault stream as `pre-stall.log`.
4. Issue exactly:

```text
B6 ARM STALL
```

5. Do not interact until the watchdog resets the board.
6. Start or continue capture through the next boot as `post-reset.log`.
7. Verify reset reason, boot-ledger change and absence of an unrelated manual reset.
8. Record the result in `result.txt`.

PASS requires expected watchdog-reset evidence tied to the armed fault.

## 9. HW-09 service export

1. Boot in recovery mode with the identified microSD inserted.
2. Issue:

```text
B6 EXPORT
```

3. Verify successful export telemetry.
4. Power down cleanly and copy the complete `/service` directory to `evidence/HW-09/service/`.
5. Inspect `manifest.txt` and copied diagnostic logs.
6. Retain the raw B6 export capture.

PASS requires an inspectable bounded service bundle matching the identified board and run.

## 10. HW-10 24-hour soak

Before the run, record workload, ambient conditions and the heap-drop limit in `campaign-manifest.json`. Default prepared limit: 32768 bytes.

1. Flash the exact frozen binary.
2. Start untouched serial capture before reset.
3. Run for at least 24 hours with declared display, touch and archive activity.
4. Do not restart the serial logger or edit the capture.
5. At completion run:

```bash
python tools/parse_b7_soak.py evidence/HW-10/capture.log \
  --min-records 17280 \
  --min-uptime-ms 86400000 \
  --require-sd \
  --max-heap-drop 32768
```

6. Save output as `parser-report.txt`.
7. Build the qualification pack:

```bash
python tools/build_b7_qualification_pack.py evidence/HW-10/capture.log \
  --output evidence/HW-10/qualification-pack \
  --board-id cyd-r-sample-01 \
  --firmware-commit <exact-commit> \
  --card-id <recorded-card-id>
```

PASS requires uninterrupted duration, no unexpected reset, monotonic counters, zero SPI violations and resource values within the predeclared gate.

## 11. HW-11 cold-start repeatability

Perform three independent cold-start runs after removing power long enough for a genuine cold start. Each run gets its own directory.

For every run repeat at minimum:

- identity confirmation;
- display startup;
- persisted touch calibration;
- microSD/archive availability;
- SPI invariants;
- normal healthy-marker transition.

Create `summary.json` listing run directories, firmware hash and PASS/FAIL. Do not combine logs from different cold starts into one apparent run.

PASS requires all three runs to satisfy the declared subset without changing firmware or hardware identities.

## 12. HW-12 final validation and release bundle

1. Update each working matrix item only after reviewing its retained evidence.
2. Set `PASS`, `FAIL` or `BLOCKED`; never mark PASS from memory or fixture output.
3. Run the final gate:

```bash
python tools/validate_b8_release.py \
  --matrix qualification/boards/cyd-r-sample-01/hardware-acceptance-matrix.json \
  --require-all-pass
```

4. Save terminal output to `evidence/HW-12/validator-report.txt`.
5. Build the exact binary-bound release bundle:

```bash
python tools/build_b8_release_bundle.py \
  --firmware .pio/build/esp32-2432s028r/firmware.bin \
  --output evidence/HW-12/release-bundle \
  --commit <exact-commit> \
  --board-id cyd-r-sample-01 \
  --matrix qualification/boards/cyd-r-sample-01/hardware-acceptance-matrix.json \
  --qualification-pack qualification/boards/cyd-r-sample-01/evidence/HW-10/qualification-pack
```

6. Recompute SHA-256 of the retained firmware and compare it with the campaign manifest and release bundle.

PASS requires all matrix items PASS, successful validation and matching exact binary hashes.

## Stop rules

Immediately stop and mark the affected item FAIL or BLOCKED when:

- the firmware hash differs from the frozen build;
- the board, card or power source is substituted without starting a new campaign record;
- an unexpected reset occurs during an acceptance run;
- raw evidence is lost, edited or cannot be associated with a trial;
- acceptance criteria need modification;
- a code fix is required.

A required code or criterion change means `v1.0.0-rc1` is not promotable from this campaign. Create `v1.0.0-rc2` and repeat the affected gates with a new evidence tree.
