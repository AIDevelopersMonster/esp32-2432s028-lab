# Evidence directory structure

Create the following directories before the campaign. Empty evidence directories are not committed; create them locally on the operator workstation.

```text
qualification/boards/cyd-r-sample-01/
├── campaign-manifest.json
├── hardware-acceptance-matrix.json
├── OPERATOR-PROTOCOL.md
├── EVIDENCE-STRUCTURE.md
└── evidence/
    ├── HW-01/
    │   ├── board-front.jpg
    │   ├── board-back.jpg
    │   ├── power-supply.jpg
    │   ├── microsd.jpg
    │   ├── build-command.txt
    │   └── firmware-sha256.txt
    ├── HW-02/
    │   ├── display-photo.jpg
    │   └── serial.log
    ├── HW-03/
    │   ├── b3-calibration.log
    │   └── calibration-photo.jpg
    ├── HW-04/
    │   ├── b5-storage.log
    │   ├── b5-current.log
    │   ├── b5-previous.log
    │   └── parser-report.txt
    ├── HW-05/
    │   └── spi-summary.txt
    ├── HW-06/
    │   ├── power-cut-matrix.csv
    │   └── b4-captures/
    ├── HW-07/
    │   ├── reset-campaign.csv
    │   └── b5-reset-captures/
    ├── HW-08/
    │   ├── pre-stall.log
    │   ├── post-reset.log
    │   └── result.txt
    ├── HW-09/
    │   ├── b6-export.log
    │   └── service/
    ├── HW-10/
    │   ├── capture.log
    │   ├── parser-report.txt
    │   └── qualification-pack/
    ├── HW-11/
    │   ├── cold-start-01/
    │   ├── cold-start-02/
    │   ├── cold-start-03/
    │   └── summary.json
    └── HW-12/
        ├── validator-report.txt
        └── release-bundle/
```

## Evidence rules

- Raw serial captures are append-only evidence. Do not normalize line endings, delete noise or reorder records.
- Keep photographs in their original files with metadata where possible.
- Every CSV or summary must point to the associated raw evidence paths.
- Use UTC timestamps in manifests and trial tables.
- Record every failed and interrupted trial; do not retain only successful repetitions.
- Never overwrite a completed run directory. Add a new run or trial directory.
- Keep the exact accepted `firmware.bin` inside the final release bundle.
- Back up the complete campaign directory before changing matrix statuses.

## Local directory creation

From the repository root:

```bash
base=qualification/boards/cyd-r-sample-01/evidence
mkdir -p \
  "$base/HW-01" "$base/HW-02" "$base/HW-03" "$base/HW-04" \
  "$base/HW-05" "$base/HW-06/b4-captures" \
  "$base/HW-07/b5-reset-captures" "$base/HW-08" \
  "$base/HW-09/service" "$base/HW-10/qualification-pack" \
  "$base/HW-11/cold-start-01" "$base/HW-11/cold-start-02" \
  "$base/HW-11/cold-start-03" "$base/HW-12/release-bundle"
```

Evidence files can be large and may contain local identifiers. Decide deliberately which artifacts belong in Git and which should remain in an external archived qualification package.
