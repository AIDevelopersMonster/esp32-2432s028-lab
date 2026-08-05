# Storage System — ESP32-2432S028

> **Document status:** sample-specific storage reference for the author's `ESP32-2432S028` board.  
> **Evidence rule:** visible markings, repository configuration, and non-destructive tests are separated from unverified routing and contents claims.  
> **Related chapters:** [Hardware Overview](01-hardware-overview.md), [PCB Component Atlas](02-pcb-component-atlas.md), [PCB Walkthrough](03-pcb-walkthrough.md), [Power System](05-power-system.md), and [Display System](06-display-system.md).

## 1. Scope

This chapter documents the board's three storage domains:

1. program Flash inside the ESP-WROOM-32 module;
2. auxiliary Winbond W25Q32JV on the carrier PCB;
3. removable microSD card.

The three devices serve different roles and must not be treated as one interchangeable storage pool.

## 2. Evidence summary

| Storage domain | Evidence | Current status |
|---|---|---|
| ESP-WROOM-32 program Flash | module construction and normal ESP32 boot architecture | function confirmed, exact capacity open |
| U4 Winbond W25Q32JV | readable package marking | device identity and nominal capacity confirmed; routing and contents open |
| microSD socket | visible socket and repository example | hardware presence confirmed; repository pin profile is current software baseline |

## 3. Storage architecture

```text
ESP-WROOM-32 internal module Flash
  -> bootloader
  -> partition table
  -> application firmware
  -> optional NVS / filesystem / OTA partitions

Carrier-board W25Q32JV
  -> separate 32-Mbit serial NOR device
  -> possible resource, data, font, image or factory storage
  -> actual use not yet proven

microSD socket
  -> removable card
  -> files, images, logs, media and exchange with a PC
```

## 4. ESP-WROOM-32 program Flash

The ESP-WROOM-32 module contains the Flash used for normal ESP32 boot and firmware execution.

### 4.1 Typical contents

Depending on the partition table, module Flash may contain:

- second-stage bootloader;
- partition table;
- application image;
- OTA application slot;
- NVS key-value storage;
- SPIFFS, LittleFS or another embedded filesystem;
- crash data or application metadata.

These are architectural possibilities, not a claim about the exact partition table currently installed.

### 4.2 Exact capacity

The module photograph does not reveal the Flash capacity. Determine it through:

- boot log;
- `esptool.py flash_id`;
- build configuration;
- module-specific documentation;
- full readback size validation.

Do not infer the capacity solely from the generic `ESP-WROOM-32` label.

### 4.3 Safe backup

Before modifying partitions or experimenting with OTA layouts:

1. record chip and Flash IDs;
2. record detected Flash size and mode;
3. read the entire addressable Flash region;
4. calculate SHA-256 of the image;
5. preserve boot log and partition-table output;
6. verify that the backup can be read and parsed.

A backup is not complete until its size and hash are recorded.

### 4.4 Partition-table inspection

Useful checks include:

- start address and length of each partition;
- overlapping or truncated entries;
- application-slot size;
- NVS location;
- filesystem type and mount point;
- OTA metadata partition;
- whether the declared Flash size matches the physical device.

Never write a partition table copied from another CYD revision without first checking total Flash size and application layout.

## 5. Auxiliary W25Q32JV — U4

![Winbond W25Q32JV on the examined board](../../hardware/images/w25q32.jpg)

The populated U4 device is a separate Winbond W25Q32JV-class serial NOR Flash.

### 5.1 Confirmed properties

- visible Winbond `25Q32JV` marking;
- 32-Mbit nominal capacity;
- 4-MB byte capacity;
- 3.3-V-class serial NOR device;
- physically separate from the ESP-WROOM-32 program Flash.

### 5.2 Open questions

The photograph does not establish:

- the connected ESP32 GPIOs;
- SPI mode;
- clock limit on this PCB;
- whether single, dual or quad I/O is wired;
- write-protect and hold/reset wiring;
- whether the chip is used by factory firmware;
- whether it contains fonts, images, calibration data or a filesystem;
- whether it is blank.

### 5.3 First-access rule

The first operation on U4 should be **read-only identification and backup**.

Do not begin with:

- chip erase;
- sector erase;
- write-status-register commands;
- protection-bit changes;
- quad-enable changes;
- arbitrary program commands.

### 5.4 Non-destructive identification

Recommended sequence:

1. continuity-map VCC and GND;
2. identify CS, CLK and data lines;
3. confirm idle CS level;
4. read JEDEC ID at a conservative SPI clock;
5. read status registers without changing them;
6. sample several address ranges;
7. read the full 4-MB image;
8. calculate and record a hash;
9. repeat the read and compare hashes.

Two identical full reads provide much stronger evidence than a single successful JEDEC-ID command.

### 5.5 Backup record

Record:

| Field | Value |
|---|---|
| board sample ID | pending |
| package marking | W25Q32JV |
| JEDEC ID | pending |
| detected capacity | pending |
| SPI pins | pending |
| SPI mode | pending |
| read clock | pending |
| image size | pending |
| SHA-256 | pending |
| all-`FF` percentage | pending |
| all-`00` percentage | pending |
| notable strings or headers | pending |

### 5.6 Interpreting contents

A non-blank image does not prove its purpose. Possible structures include:

- raw binary resources;
- compressed image data;
- font tables;
- fixed-offset records;
- filesystem image;
- factory test data;
- calibration values;
- encrypted or obfuscated data.

Identify structure by reproducible parsing evidence, not by visual pattern alone.

## 6. microSD hardware path

The board contains a removable microSD/TF socket.

The repository's current software profile defines:

```text
SD_MISO = GPIO19
SD_MOSI = GPIO23
SD_SCLK = GPIO18
SD_CS   = GPIO5
```

The existing example creates a dedicated `SPIClass`, initializes those pins, mounts the card with `SD.begin()`, reports card size, and lists directories.

This profile is a working repository baseline. Electrical continuity and revision identity should still be verified on the author's sample.

## 7. GPIO5 caution

GPIO5 is an ESP32 strapping-related pin on classic ESP32 designs.

Potential problems include:

- external pull state affecting boot;
- SD card or socket circuitry driving the line during reset;
- a damaged or partially inserted card disturbing startup;
- overly strong external pull resistors;
- firmware selecting CS too late or with the wrong idle state.

Tests should include booting with:

- no card;
- a known-good card;
- an unformatted card;
- card inserted after boot;
- card removed while powered only when the software explicitly supports it.

Do not assume hot-plug safety from the mechanical socket alone.

## 8. microSD electrical considerations

### 8.1 Logic voltage

ESP32 GPIO is 3.3-V logic. The on-board socket path should be treated as a 3.3-V interface unless measurement proves additional translation circuitry.

### 8.2 Power transients

Cards can draw short current peaks during:

- initialization;
- writes;
- erase-block management;
- filesystem metadata updates;
- internal garbage collection.

A multimeter may miss the transient that causes a mount failure or reset.

### 8.3 Signal integrity

Symptoms of excessive SPI speed or poor signal quality include:

- intermittent mount failure;
- corrupted file names;
- CRC or read errors;
- write failures at large block sizes;
- card-dependent reliability;
- failures only with Wi-Fi or TFT activity.

Start at a conservative clock, then qualify higher rates by repeated read/write verification.

## 9. Card compatibility

Test more than one card type and capacity.

Record:

- manufacturer and model;
- nominal capacity;
- card type reported by software;
- filesystem;
- allocation-unit size;
- mount result;
- sustained read speed;
- sustained write speed;
- error count;
- operating temperature;
- observed current or rail droop.

Seller claims that “all cards are supported” are not a substitute for a compatibility matrix.

## 10. Filesystem choices

Common removable-card filesystems include FAT16, FAT32 and exFAT.

Compatibility depends on:

- card capacity;
- Arduino/ESP32 core version;
- enabled filesystem support;
- formatting parameters;
- long-file-name support;
- host operating system behavior.

For maximum compatibility with simple embedded examples, FAT32 is often the first test choice. This is a practical starting point, not a universal requirement.

## 11. Formatting discipline

Before blaming the board:

1. back up important card contents;
2. verify the card on a PC;
3. run a capacity/fake-card test where appropriate;
4. format with a known tool;
5. use a simple volume label;
6. avoid unusual partition layouts during first tests;
7. retest at a conservative SPI clock.

A counterfeit or failing card can mimic board, library or power problems.

## 12. Safe write test

Use a disposable test card or a dedicated test directory.

Recommended sequence:

1. mount card;
2. create a small text file;
3. flush and close;
4. reopen and compare exact contents;
5. create a larger deterministic binary file;
6. calculate a hash while writing;
7. remount the card;
8. read back and recalculate the hash;
9. compare on a PC;
10. repeat through several power cycles.

Do not test write reliability using the only copy of valuable data.

## 13. Throughput test

Measure separately:

- sequential read;
- sequential write;
- many small-file operations;
- metadata-heavy directory creation;
- random block access if relevant;
- simultaneous TFT rendering;
- simultaneous Wi-Fi transfer;
- audio playback from card.

Record block size and total transferred bytes. A single short transfer is usually dominated by setup and cache effects.

## 14. Data-integrity test

Use deterministic content such as:

- incrementing byte pattern;
- pseudorandom sequence with fixed seed;
- block number plus CRC;
- SHA-256 of complete files.

For long tests, record:

- bytes written;
- bytes read;
- mismatched blocks;
- retries;
- mount failures;
- resets;
- rail voltage minima;
- card and regulator temperature.

## 15. Power-loss and corruption risks

Removing power during a write may damage:

- the current file;
- directory entries;
- free-space metadata;
- filesystem allocation tables;
- unrelated data being relocated internally by the card.

Risk reduction methods include:

- explicit flush and close;
- journaling at the application level;
- append-only records;
- checksums;
- duplicate metadata;
- atomic rename patterns;
- power-fail detection;
- hold-up capacitance where justified;
- avoiding frequent metadata updates.

FAT filesystems are not inherently power-fail safe.

## 16. Logging architecture

For reliable embedded logging:

- preallocate files where possible;
- buffer records in RAM;
- write aligned blocks;
- flush at controlled intervals;
- include sequence number and checksum;
- mark clean shutdown;
- recover incomplete final records on boot;
- limit wear from excessive open/close cycles.

The correct flush interval balances data-loss window, latency, wear and power consumption.

## 17. Interaction with TFT and touch

The current repository uses distinct pin groups for TFT, touch and microSD.

Even with separate SPI peripherals, interactions can occur through:

- shared ESP32 DMA resources;
- interrupt latency;
- task scheduling;
- power rail transients;
- memory pressure;
- incorrect global SPI assumptions in libraries.

Test storage while continuously updating the display and reading touch input.

## 18. Interaction with audio

Audio playback from microSD stresses multiple subsystems simultaneously:

```text
microSD read
  -> filesystem
  -> decode / buffering
  -> GPIO26 or another audio source path
  -> 8002A amplifier
  -> speaker load
```

A stable isolated SD test does not prove stable audio playback. Buffer underruns, rail sag and filesystem latency must be tested together.

## 19. Memory and buffering

Large buffers can improve throughput but compete with:

- TFT sprites and frame buffers;
- network stacks;
- audio decode buffers;
- touch and UI state;
- filesystem objects.

Measure free heap and largest allocatable block before and during storage operations. Fragmentation can cause failures even when total free memory appears sufficient.

## 20. Card-detect status

No card-detect signal is currently documented in the repository profile.

Possible cases:

- socket has no detect switch;
- switch exists but is not routed;
- detect line is routed to an undocumented GPIO;
- software relies only on mount attempts.

Confirm mechanically and by continuity test before adding a card-detect GPIO claim.

## 21. Write-protect status

microSD cards do not provide the full-size SD card's mechanical write-protect slider. Adapters may have one, but the microSD socket itself normally does not expose that user switch.

Software write protection must therefore be implemented explicitly when needed.

## 22. First microSD bring-up

1. use a known-good, backed-up card;
2. format with a simple supported filesystem;
3. inspect socket contacts;
4. power from a stable source;
5. initialize at a conservative SPI clock;
6. print card type and size;
7. list the root directory;
8. read a known file;
9. perform a disposable write/readback test;
10. repeat across ten cold boots.

## 23. microSD troubleshooting

### Mount fails consistently

Check:

- pin profile;
- CS idle state;
- card formatting;
- card seating;
- 3.3-V rail;
- boot interaction with GPIO5;
- SPI instance passed to `SD.begin()`;
- socket soldering.

### Mount is intermittent

Check:

- USB cable and rail droop;
- SPI clock;
- card quality;
- connector contamination;
- long test leads;
- concurrent subsystem activity;
- insufficient startup delay.

### Reads work but writes fail

Check:

- filesystem free space;
- card health;
- power transients;
- file close/flush behavior;
- counterfeit capacity;
- write-protection logic in software;
- allocation-unit and filesystem integrity.

### Data corrupts after reset

Check:

- unclosed files;
- reset during metadata update;
- inadequate power-fail handling;
- application buffer ownership;
- task synchronization;
- card removal while mounted.

## 24. Auxiliary-Flash troubleshooting

### JEDEC ID is all `FF`

Possible causes:

- CS never asserted;
- MISO floating high;
- wrong pin map;
- device unpowered;
- incorrect SPI mode;
- HOLD/RESET state;
- bus contention.

### JEDEC ID is all `00`

Possible causes:

- shorted or held-low data line;
- wrong wiring;
- device held in reset;
- excessive clock;
- damaged device;
- reading the wrong bus.

### ID is valid but data varies between reads

Check:

- clock rate;
- power integrity;
- SPI mode;
- CS timing;
- shared-bus contention;
- buffer or transfer-length bugs.

## 25. Continuity-mapping plan

With the board unpowered:

1. identify U4 pin 1 orientation;
2. confirm U4 VCC and GND;
3. map CLK, CS, DI and DO;
4. check WP and HOLD/RESET biasing;
5. trace microSD CLK, MOSI, MISO and CS to ESP32 pins;
6. look for series resistors or pull networks;
7. check whether any lines are shared with another device;
8. record resistance, not only buzzer continuity.

## 26. Logic-analyzer plan

Capture:

- power-up and first mount;
- card identification sequence;
- single-block read;
- multi-block read;
- file write and close;
- U4 JEDEC-ID command;
- U4 full-read start;
- failures at qualified and overclocked SPI rates.

Annotate CS, CLK, MOSI and MISO and record the exact firmware version.

## 27. Storage test matrix

| Test | Module Flash | U4 Flash | microSD |
|---|---:|---:|---:|
| identify device | required | required | required |
| full backup | required before partition changes | required before writes | card-dependent |
| repeated read/hash | recommended | required | required for integrity test |
| write test | controlled | only after backup | disposable data |
| power-loss test | advanced | advanced | required for logging products |
| thermal test | system-level | system-level | recommended |
| combined TFT/Wi-Fi/audio test | recommended | recommended | required for media applications |

## 28. Measurement record

| Item | Result |
|---|---|
| ESP32 module Flash ID | pending |
| module Flash capacity | pending |
| module Flash mode/frequency | pending |
| partition-table hash | pending |
| full module backup hash | pending |
| U4 JEDEC ID | pending |
| U4 full-image hash | pending |
| U4 GPIO map | pending |
| microSD pin continuity | pending |
| stable microSD SPI clock | pending |
| tested card list | pending |
| max verified read rate | pending |
| max verified write rate | pending |
| power-loss behavior | pending |

## 29. Evidence levels

### Level A — visual

- microSD socket present;
- W25Q32JV marking readable;
- ESP-WROOM-32 module present.

### Level B — software baseline

- repository pin profile exists;
- microSD example mounts and lists files when run successfully;
- build and library versions recorded.

### Level C — electrical

- GPIO continuity mapped;
- supply and signal levels measured;
- SPI captures recorded;
- stable frequency range established.

### Level D — data integrity

- full backups with hashes;
- repeated identical reads;
- write/readback hash tests;
- power-cycle and stress-test results.

## 30. Completion criteria

The storage system can be marked verified when:

- module Flash identity, capacity and partition layout are recorded;
- a complete module-Flash backup exists with hash;
- U4 JEDEC ID and full-image backup are recorded;
- U4 pin routing is mapped;
- microSD pin routing is continuity-confirmed;
- at least three representative cards are tested;
- conservative and maximum stable SPI rates are recorded;
- read/write integrity tests pass;
- combined TFT, touch, Wi-Fi and audio tests pass where applicable;
- corruption behavior under reset or power loss is documented;
- all firmware, library and card details are reproducible.

## 31. Current controlled claims

The following claims are currently safe:

- the board has three distinct storage domains;
- U4 is a separate W25Q32JV-class 4-MB serial NOR device;
- the microSD socket is physically present;
- the repository currently uses GPIO19/23/18/5 for microSD;
- the existing example performs a basic mount and directory listing;
- destructive writes to U4 should not precede a complete verified backup.

The following remain open:

- exact ESP-WROOM-32 Flash capacity;
- exact U4 GPIO routing and use;
- U4 contents and filesystem status;
- electrical confirmation of the microSD pin profile;
- card-detect implementation;
- maximum reliable SPI rates;
- production-safe power-loss behavior.
