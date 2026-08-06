# B8 Hardware Acceptance — Start Here

This is the operator entry point for the board-specific acceptance campaign.

## What is the program?

The interactive HW-01 program is:

```text
tools/run_hw01_identity.py
```

It asks for the physical identities, builds the frozen firmware, calculates its SHA-256, copies the exact binary into the evidence directory, updates the campaign manifest, and creates:

```text
qualification/boards/cyd-r-sample-01/evidence/HW-01/hw-01-protocol.json
```

It does **not** automatically mark HW-01 as PASS. Board photographs and an operator review are still required.

## Start from the repository root

```bash
git pull
python tools/run_hw01_identity.py
```

The program will ask for:

- permanent board ID;
- visible board markings;
- power supply identity and rating;
- USB-UART identity;
- microSD identity and capacity;
- operator name;
- ambient conditions.

It then runs:

```bash
pio run
```

and uses:

```text
.pio/build/esp32-2432s028r/firmware.bin
```

## Files produced by HW-01

```text
qualification/boards/cyd-r-sample-01/
├── campaign-manifest.json                 # updated campaign identity
└── evidence/HW-01/
    ├── hw-01-protocol.json                # generated HW-01 protocol
    ├── firmware.bin                       # exact tested binary
    ├── firmware.sha256                    # binary checksum
    ├── build.log                          # PlatformIO build output
    ├── board-front.jpg                    # add manually
    └── board-back.jpg                     # add manually
```

## After the wizard

1. Add clear front and back photographs of the same physical board.
2. Open `hw-01-protocol.json` and verify every entered field.
3. Confirm that the board label in the photographs matches `board_id`.
4. Confirm the firmware hash with an independent command if desired.
5. Only then change HW-01 in the working acceptance matrix from `NOT_RUN` to `PASS` and retain a reviewer/operator note.

Working matrix:

```text
qualification/boards/cyd-r-sample-01/hardware-acceptance-matrix.json
```

## Existing detailed documents

- [Full operator protocol](../qualification/boards/cyd-r-sample-01/OPERATOR-PROTOCOL.md)
- [Evidence directory structure](../qualification/boards/cyd-r-sample-01/EVIDENCE-STRUCTURE.md)
- [Campaign manifest](../qualification/boards/cyd-r-sample-01/campaign-manifest.json)
- [Working acceptance matrix](../qualification/boards/cyd-r-sample-01/hardware-acceptance-matrix.json)

## Important boundary

The wizard records evidence. It cannot visually verify that the entered markings, power supply, microSD, and photographs truly belong to the same physical sample. That final verification remains an operator responsibility.
