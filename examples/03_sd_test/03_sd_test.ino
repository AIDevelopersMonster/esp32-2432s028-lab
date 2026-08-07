/*
 * ESP32-2432S028R / Cheap Yellow Display (CYD)
 * ------------------------------------------------------------
 * Example: 03_sd_test
 * Purpose: safe read-only microSD hardware and filesystem test.
 *
 * Project:
 *   https://github.com/AIDevelopersMonster/esp32-2432s028-lab
 *
 * Documentation:
 *   examples/03_sd_test/README.md
 *
 * Status:
 *   VERIFIED WORKING on 2026-08-07
 *
 * Verified on real hardware:
 *   - microSD mount succeeds;
 *   - card is detected by the ESP32 SD library;
 *   - reported card size: 30250 MB;
 *   - root directory enumeration works;
 *   - directories and files created manually on the card are visible;
 *   - Windows service files are also enumerated when present.
 *
 * Observed example output included:
 *   Card size: 30250 MB
 *   DIR : System Volume Information
 *   FILE: WPSettings.dat
 *   FILE: IndexerVolumeGuid
 *   DIR : test
 *   DIR : test2
 *   FILE: test1.txt
 *
 * This sketch is intentionally READ-ONLY.
 * It does NOT create, modify, delete or format files on the card.
 * Use 03_sd_rw_lab for the educational write/readback laboratory.
 *
 * microSD pin map from CYD_Board:
 *   MISO : GPIO 19
 *   MOSI : GPIO 23
 *   SCLK : GPIO 18
 *   CS   : GPIO 5
 *
 * The microSD slot uses its own SPI pin group on this CYD revision.
 * Do not assume that it shares the TFT or XPT2046 SPI wiring.
 *
 * Required libraries:
 *   - SPI from the ESP32 Arduino core
 *   - SD from the ESP32 Arduino core
 *   - CYD_Board from this repository
 *
 * Serial Monitor:
 *   115200 baud
 *
 * What this sketch checks:
 *   - board-specific microSD SPI pin assignment;
 *   - SD.begin() mount operation;
 *   - physical card detection through SD.cardType();
 *   - card capacity reporting;
 *   - root directory opening;
 *   - recursive directory and file enumeration up to two levels deep.
 */

#include <SPI.h>       // ESP32 SPI bus support.
#include <SD.h>        // ESP32 SD filesystem library.
#include <CYD_Board.h> // Project-specific CYD pin definitions.

// The microSD connector on this board uses a dedicated SPI pin group.
// VSPI here is only the ESP32 SPI peripheral object; begin() below remaps
// it to the actual CYD microSD pins declared in CYD_Board.
SPIClass sdSpi(VSPI);

// Recursively list files and directories starting at 'path'.
// 'levels' limits recursion depth so the test cannot endlessly traverse
// an unexpectedly deep directory tree.
void listDirectory(fs::FS &fs, const char *path, uint8_t levels) {
  // Open the requested path and make sure it is really a directory.
  File root = fs.open(path);
  if (!root || !root.isDirectory()) {
    Serial.println("Failed to open directory");
    return;
  }

  // Iterate through every entry in the directory.
  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      // Print directory names as they are reported by the filesystem.
      Serial.printf("DIR : %s\n", file.name());

      // Enter the directory only while the requested recursion depth remains.
      if (levels > 0) {
        listDirectory(fs, file.path(), levels - 1);
      }
    } else {
      // For normal files print both the name and file size in bytes.
      Serial.printf("FILE: %-24s SIZE: %u\n",
                    file.name(),
                    static_cast<unsigned>(file.size()));
    }

    // Move to the next entry in the current directory.
    file = root.openNextFile();
  }
}

void setup() {
  // All test results are printed to Serial Monitor.
  Serial.begin(115200);

  // Give the USB/UART monitor a short moment to become ready after reset.
  delay(500);

  // Start the SPI peripheral on the board-specific microSD pins:
  // SCLK=18, MISO=19, MOSI=23, CS=5.
  sdSpi.begin(cyd::SD_SCLK_PIN,
              cyd::SD_MISO_PIN,
              cyd::SD_MOSI_PIN,
              cyd::SD_CS_PIN);

  // Mount the card filesystem through the dedicated SPI object.
  // Failure here normally means wiring, card insertion, filesystem or
  // compatibility problems rather than a directory-listing problem.
  if (!SD.begin(cyd::SD_CS_PIN, sdSpi)) {
    Serial.println("microSD mount failed");
    return;
  }

  // SD.begin() can succeed far enough to initialize the bus while no usable
  // card is actually reported, so cardType() is checked separately.
  const uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No microSD card detected");
    return;
  }

  // Report physical card capacity in binary megabytes as returned by the
  // ESP32 SD library. The verified test card reports 30250 MB here.
  Serial.printf("Card size: %llu MB\n",
                SD.cardSize() / (1024ULL * 1024ULL));

  // List the root directory and recurse through at most two further levels.
  // The sketch only opens entries for reading; it does not create or alter
  // anything on the card.
  listDirectory(SD, "/", 2);
}

void loop() {
  // One-shot read-only test. Press RESET/EN to run the scan again.
}
