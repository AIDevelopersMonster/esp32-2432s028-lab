/*
 * ESP32-2432S028R / Cheap Yellow Display (CYD)
 * ------------------------------------------------------------
 * Example: 03_sd_rw_lab
 * Purpose: educational microSD filesystem read/write laboratory.
 *
 * Project:
 *   https://github.com/AIDevelopersMonster/esp32-2432s028-lab
 *
 * Documentation:
 *   examples/03_sd_rw_lab/README.md
 *
 * Status:
 *   VERIFIED WORKING on 2026-08-07
 *
 * Verified on real hardware:
 *   - microSD mount succeeds;
 *   - reported card size: 30250 MB;
 *   - /cyd_lab and nested directories are created;
 *   - files are created at several hierarchy levels;
 *   - written byte counts match expectations;
 *   - files are closed and reopened;
 *   - readback matches the exact written text;
 *   - append mode works and is verified by full readback;
 *   - the resulting directory tree is enumerated successfully.
 *
 * Verified resulting tree:
 *   /cyd_lab
 *   |-- readme.txt                    23 bytes
 *   |-- config/
 *   |   `-- device.txt                37 bytes
 *   |-- logs/
 *   |   `-- session.txt               34 bytes after append
 *   `-- archive/
 *       `-- 2026/
 *           `-- result.txt            18 bytes
 *
 * Verified final status markers:
 *   SD_RW_LAB_PASS
 *   DIRECTORY_CREATE_CONFIRMED
 *   NESTED_DIRECTORY_CONFIRMED
 *   FILE_WRITE_CONFIRMED
 *   FILE_READBACK_CONFIRMED
 *   FILE_APPEND_CONFIRMED
 *
 * Relationship to 03_sd_test:
 *   - 03_sd_test is the safe read-only hardware/mount/listing test.
 *   - 03_sd_rw_lab is the companion write/readback laboratory.
 *
 * IMPORTANT SAFETY RULE:
 *   The sketch works only inside /cyd_lab and does not format the card.
 *   Existing files elsewhere on the microSD are not intentionally modified.
 *   Files with the same names inside /cyd_lab are overwritten by this test.
 *
 * microSD pin map from CYD_Board:
 *   MISO : GPIO 19
 *   MOSI : GPIO 23
 *   SCLK : GPIO 18
 *   CS   : GPIO 5
 *
 * Required libraries:
 *   - SPI from the ESP32 Arduino core
 *   - SD from the ESP32 Arduino core
 *   - CYD_Board from this repository
 *
 * Serial Monitor:
 *   115200 baud
 */

#include <SPI.h>       // ESP32 SPI bus support.
#include <SD.h>        // ESP32 SD filesystem library.
#include <CYD_Board.h> // Project-specific CYD pin definitions.

// The microSD connector uses its own SPI pin group on this CYD revision.
// VSPI is used as the hardware SPI peripheral and remapped below to the
// board-specific pins from CYD_Board.
SPIClass sdSpi(VSPI);

// Keep every destructive test operation inside one dedicated directory.
// This makes the laboratory repeatable and avoids touching unrelated files.
static const char *LAB_ROOT = "/cyd_lab";

// Create a directory if it does not already exist.
// Returning true for an existing directory makes the test repeatable after
// RESET without requiring the user to remove /cyd_lab manually.
bool ensureDirectory(const char *path) {
  if (SD.exists(path)) {
    Serial.printf("DIR EXISTS : %s\n", path);
    return true;
  }

  if (SD.mkdir(path)) {
    Serial.printf("DIR CREATED: %s\n", path);
    return true;
  }

  Serial.printf("DIR FAILED : %s\n", path);
  return false;
}

// Write the complete expected text to a file, close it, reopen it for reading
// and compare the full readback with the original string.
// This checks much more than successful File.open(): it verifies that bytes
// actually reach the filesystem and can be read back unchanged.
bool writeAndVerify(const char *path, const char *expected) {
  Serial.printf("\nWRITE      : %s\n", path);

  // FILE_WRITE creates the file when needed and writes the new test content.
  File file = SD.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("  FAIL: cannot open for writing");
    return false;
  }

  const size_t expectedLength = strlen(expected);
  const size_t written = file.print(expected);

  // Closing before verification is deliberate: it flushes/finishes the write
  // and forces the next step to reopen the file through the filesystem.
  file.close();

  if (written != expectedLength) {
    Serial.printf("  FAIL: wrote %u of %u bytes\n",
                  static_cast<unsigned>(written),
                  static_cast<unsigned>(expectedLength));
    return false;
  }

  // Reopen as a separate read operation.
  File verifyFile = SD.open(path, FILE_READ);
  if (!verifyFile) {
    Serial.println("  FAIL: cannot reopen for reading");
    return false;
  }

  String actual = verifyFile.readString();
  verifyFile.close();

  // Exact comparison verifies both file length and file contents.
  if (actual != expected) {
    Serial.println("  FAIL: readback differs from written data");
    Serial.printf("  READ: %s\n", actual.c_str());
    return false;
  }

  Serial.printf("  PASS: %u bytes written and verified\n",
                static_cast<unsigned>(expectedLength));
  return true;
}

// Append new text to an existing file, close it, reopen it and verify the
// complete final contents. This demonstrates that append is different from
// replacing the original file contents.
bool appendAndVerify(const char *path,
                     const char *appendText,
                     const char *expectedFinal) {
  Serial.printf("\nAPPEND     : %s\n", path);

  File file = SD.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("  FAIL: cannot open for append");
    return false;
  }

  const size_t expectedAppendLength = strlen(appendText);
  const size_t written = file.print(appendText);
  file.close();

  if (written != expectedAppendLength) {
    Serial.println("  FAIL: append length mismatch");
    return false;
  }

  File verifyFile = SD.open(path, FILE_READ);
  if (!verifyFile) {
    Serial.println("  FAIL: cannot reopen after append");
    return false;
  }

  String actual = verifyFile.readString();
  verifyFile.close();

  if (actual != expectedFinal) {
    Serial.println("  FAIL: final contents do not match");
    Serial.printf("  READ: %s\n", actual.c_str());
    return false;
  }

  Serial.println("  PASS: append verified");
  return true;
}

// Recursively print the resulting directory tree so the user can confirm
// where files were created and what sizes the filesystem reports.
void listDirectory(fs::FS &fs, const char *path, uint8_t levels) {
  File root = fs.open(path);
  if (!root || !root.isDirectory()) {
    Serial.printf("Cannot open directory: %s\n", path);
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.printf("DIR : %s\n", file.path());

      // Recursion depth is explicitly bounded even though this laboratory
      // creates only a small known hierarchy.
      if (levels > 0) {
        listDirectory(fs, file.path(), levels - 1);
      }
    } else {
      Serial.printf("FILE: %-32s SIZE: %u\n",
                    file.path(),
                    static_cast<unsigned>(file.size()));
    }

    file = root.openNextFile();
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("ESP32-2432S028R microSD read/write laboratory");
  Serial.println("Working directory: /cyd_lab");
  Serial.println("No formatting is performed.");
  Serial.println();

  // Start the microSD SPI bus on the board-specific pins:
  // SCLK=18, MISO=19, MOSI=23, CS=5.
  sdSpi.begin(cyd::SD_SCLK_PIN,
              cyd::SD_MISO_PIN,
              cyd::SD_MOSI_PIN,
              cyd::SD_CS_PIN);

  // Mount the card through the dedicated SPI object.
  if (!SD.begin(cyd::SD_CS_PIN, sdSpi)) {
    Serial.println("microSD mount failed");
    return;
  }

  // Make sure a physical/usable card was detected after mounting.
  if (SD.cardType() == CARD_NONE) {
    Serial.println("No microSD card detected");
    return;
  }

  Serial.printf("Card size: %llu MB\n",
                SD.cardSize() / (1024ULL * 1024ULL));

  // Accumulate the result of every independent filesystem operation.
  // Using &= lets the later checks still run and produce useful diagnostics.
  bool ok = true;

  // Create a small hierarchy to demonstrate directories and subdirectories.
  ok &= ensureDirectory(LAB_ROOT);
  ok &= ensureDirectory("/cyd_lab/config");
  ok &= ensureDirectory("/cyd_lab/logs");
  ok &= ensureDirectory("/cyd_lab/archive");
  ok &= ensureDirectory("/cyd_lab/archive/2026");

  if (!ok) {
    Serial.println("\nDirectory creation failed. Test stopped.");
    return;
  }

  // Root-level file.
  ok &= writeAndVerify(
      "/cyd_lab/readme.txt",
      "CYD microSD laboratory\n");

  // File inside a first-level configuration directory.
  ok &= writeAndVerify(
      "/cyd_lab/config/device.txt",
      "board=ESP32-2432S028R\nmode=sd-rw-lab\n");

  // File used later for the append demonstration.
  ok &= writeAndVerify(
      "/cyd_lab/logs/session.txt",
      "session-start\n");

  // File placed two directory levels below /cyd_lab.
  ok &= writeAndVerify(
      "/cyd_lab/archive/2026/result.txt",
      "nested-write-pass\n");

  // Append another line to session.txt and verify that both the original and
  // appended data are present in the correct order.
  ok &= appendAndVerify(
      "/cyd_lab/logs/session.txt",
      "write-read-verified\n",
      "session-start\nwrite-read-verified\n");

  // Print the final hierarchy and sizes exactly as the filesystem reports it.
  Serial.println();
  Serial.println("--- /cyd_lab directory tree ---");
  listDirectory(SD, LAB_ROOT, 4);

  Serial.println();
  if (ok) {
    // Machine-readable PASS markers make the laboratory result easy to copy
    // into documentation, logs or later automated checks.
    Serial.println("SD_RW_LAB_PASS");
    Serial.println("DIRECTORY_CREATE_CONFIRMED");
    Serial.println("NESTED_DIRECTORY_CONFIRMED");
    Serial.println("FILE_WRITE_CONFIRMED");
    Serial.println("FILE_READBACK_CONFIRMED");
    Serial.println("FILE_APPEND_CONFIRMED");
  } else {
    Serial.println("SD_RW_LAB_FAIL");
  }

  // Leave all generated files on the card deliberately. The user can remove
  // the microSD later and inspect /cyd_lab from a desktop computer.
  Serial.println();
  Serial.println("Files are intentionally left on the card in /cyd_lab");
  Serial.println("so they can also be inspected later on a computer.");
}

void loop() {
  // One-shot laboratory test. Press RESET/EN to run it again.
}
