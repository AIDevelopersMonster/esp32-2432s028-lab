# B8 — Release Candidate Freeze, Hardware Acceptance Matrix and Versioned Laboratory Baseline

## Status

- Repository implementation: complete.
- Release candidate identifier: `v1.0.0-rc1`.
- Hardware acceptance: pending.
- 24-hour soak and repeatability: pending.
- Production or regulatory qualification: not claimed.

## Purpose

B8 freezes an inspectable laboratory release candidate from the B1–B7 architecture. It does not convert repository implementation into hardware qualification. The release remains evidence-gated until the acceptance matrix is completed from retained board-specific evidence.

## Frozen identity

The firmware exposes:

- release name;
- release version;
- milestone;
- board profile;
- release date;
- baseline status.

The version is intentionally an RC identifier rather than a final release because required hardware campaigns remain incomplete.

## Versioned baseline

`release/baseline-v1.0.0-rc1.json` declares:

- included milestones B1–B8;
- hardware scope;
- source baseline;
- required acceptance matrix;
- claim ceiling and explicit non-claims.

A later change to firmware behavior, pin allocation, persistent schema, telemetry contract, acceptance criteria or release tooling requires a new versioned baseline. The RC file must not be silently rewritten after release evidence is collected.

## Hardware acceptance matrix

`release/hardware-acceptance-matrix-v1.0.0-rc1.json` contains twelve declared gates covering identity, TFT, touch, microSD, SPI invariants, configuration recovery, boot containment, watchdog reset, service export, soak, repeatability and release bundle integrity.

Allowed states are:

- `PASS` — retained evidence satisfies the declared requirement;
- `FAIL` — evidence contradicts the requirement;
- `NOT_RUN` — no qualifying run exists;
- `BLOCKED` — the run could not be completed for a recorded external reason.

`NOT_RUN` and `BLOCKED` must never be represented as acceptance.

## Validation

Repository structure check:

```bash
python tools/validate_b8_release.py
```

Final all-pass gate, only after editing a copied evidence matrix from actual results:

```bash
python tools/validate_b8_release.py \
  --matrix qualification/hardware-acceptance-matrix.json \
  --require-all-pass
```

The repository matrix remains `NOT_RUN` by default so that source control does not claim tests that have not been performed.

## Release bundle

After building an exact firmware binary:

```bash
python tools/build_b8_release_bundle.py \
  --firmware .pio/build/esp32dev/firmware.bin \
  --output qualification/B8-v1.0.0-rc1 \
  --commit <exact-git-sha> \
  --board-id <physical-board-id> \
  --matrix qualification/hardware-acceptance-matrix.json \
  --qualification-pack qualification/B7-run-01
```

The builder copies the firmware, baseline and selected matrix, computes SHA-256 values and records the operator-supplied commit and board identity. The bundle is inspectable but unsigned and not tamper-proof.

## Acceptance campaign

A release-candidate acceptance run must retain:

1. exact firmware commit and binary hash;
2. board, power-supply and microSD identities;
3. B3 calibration evidence;
4. B4 transaction and power-interruption evidence;
5. B5 reset, containment and archive evidence;
6. B6 watchdog and service-export evidence;
7. B7 24-hour soak pack;
8. three cold-start repeatability runs;
9. completed matrix with links or paths to retained evidence;
10. generated B8 release bundle.

## Change-control rule

After RC freeze, fixes are allowed only through a new commit and a new RC version. Evidence collected against one firmware hash cannot be transferred to another binary without a declared impact analysis and rerun decision.

## Claim ceiling

The strongest claim available before all matrix items pass is:

> The repository defines versioned release-candidate artifacts and an explicit hardware-acceptance process for the documented ESP32-2432S028R laboratory baseline.

After every matrix item passes for an identified sample, the claim may be raised only to:

> The identified sample and exact firmware binary passed the declared laboratory acceptance matrix under the recorded conditions.

This is not universal compatibility, production qualification, regulatory approval or field reliability certification.

## Next step

Do not create B9 automatically. Complete the B3–B8 hardware evidence campaign, resolve failures, and decide whether the result becomes `v1.0.0` or a new release candidate such as `v1.0.0-rc2`.
