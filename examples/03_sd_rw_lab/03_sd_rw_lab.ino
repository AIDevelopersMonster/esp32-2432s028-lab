/*
 * ESP32-2432S028R / Cheap Yellow Display (CYD)
 * ------------------------------------------------------------
 * Example: 03_sd_rw_lab
 * Purpose: educational microSD filesystem read/write laboratory.
 *
 * Project:
 *   https://github.com/AIDevelopersMonster/esp32-2432s028-lab
 *
 * This companion example follows 03_sd_test.
 * 03_sd_test is a read-only hardware/mount check.
 * This sketch demonstrates how an application can:
 *   - create directories and subdirectories;
 *   - create files;
 *   - write text;
 *   - close and reopen files;
 *   - read data back and compare it;
 *   - append additional data;
 *   - list the resulting directory tree.
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
 * Serial Monitor:
 *   115200 baud
 */

#include <SPI.h>
#include <SD.h>
#include <CYD_Board.h>

// The microSD slot uses its own SPI pin group on this CYD revision.
SPIClass sdSpi(VSPI);

static const char *LAB_ROOT = "/cyd_lab";

// Create a directory if it does not already exist.
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

// Write a complete text file, close it, reopen it and verify its contents.
bool writeAndVerify(const char *path, const char *expected) {
  Serial.printf("\nWRITE      : %s\n", path);

  File file = SD.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("  FAIL: cannot open for writing");
    return false;
  }

  const size_t expectedLength = strlen(expected);
  const size_t written = file.print(expected);
  file.close();

  if (written != expectedLength) {
    Serial.printf("  FAIL: wrote %u of %u bytes\n",
                  static_cast<unsigned>(written),
                  static_cast<unsigned>(expectedLength));
    return false;
  }

  File verifyFile = SD.open(path, FILE_READ);
  if (!verifyFile) {
    Serial.println("  FAIL: cannot reopen for reading");
    return false;
  }

  String actual = verifyFile.readString();
  verifyFile.close();

  if (actual != expected) {
    Serial.println("  FAIL: readback differs from written data");
    Serial.printf("  READ: %s\n", actual.c_str());
    return false;
  }

  Serial.printf("  PASS: %u bytes written and verified\n",
                static_cast<unsigned>(expectedLength));
  return true;
}

// Append text to an existing file and then verify the full final contents.
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

// Recursively print the directory tree so the user can see what was created.
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

  // Start the microSD SPI bus on the board-specific pins from CYD_Board.
  sdSpi.begin(cyd::SD_SCLK_PIN,
              cyd::SD_MISO_PIN,
              cyd::SD_MOSI_PIN,
              cyd::SD_CS_PIN);

  if (!SD.begin(cyd::SD_CS_PIN, sdSpi)) {
    Serial.println("microSD mount failed");
    return;
  }

  if (SD.cardType() == CARD_NONE) {
    Serial.println("No microSD card detected");
    return;
  }

  Serial.printf("Card size: %llu MB\n",
                SD.cardSize() / (1024ULL * 1024ULL));

  // Create a small hierarchy to demonstrate directories and subdirectories.
  bool ok = true;
  ok &= ensureDirectory(LAB_ROOT);
  ok &= ensureDirectory("/cyd_lab/config");
  ok &= ensureDirectory("/cyd_lab/logs");
  ok &= ensureDirectory("/cyd_lab/archive");
  ok &= ensureDirectory("/cyd_lab/archive/2026");

  if (!ok) {
    Serial.println("\nDirectory creation failed. Test stopped.");
    return;
  }

  // Create and verify several files at different hierarchy levels.
  ok &= writeAndVerify(
      "/cyd_lab/readme.txt",
      "CYD microSD laboratory\n");

  ok &= writeAndVerify(
      "/cyd_lab/config/device.txt",
      "board=ESP32-2432S028R\nmode=sd-rw-lab\n");

  ok &= writeAndVerify(
      "/cyd_lab/logs/session.txt",
      "session-start\n");

  ok &= writeAndVerify(
      "/cyd_lab/archive/2026/result.txt",
      "nested-write-pass\n");

  // Demonstrate append mode and verify the complete resulting file.
  ok &= appendAndVerify(
      "/cyd_lab/logs/session.txt",
      "write-read-verified\n",
      "session-start\nwrite-read-verified\n");

  Serial.println();
  Serial.println("--- /cyd_lab directory tree ---");
  listDirectory(SD, LAB_ROOT, 4);

  Serial.println();
  if (ok) {
    Serial.println("SD_RW_LAB_PASS");
    Serial.println("DIRECTORY_CREATE_CONFIRMED");
    Serial.println("NESTED_DIRECTORY_CONFIRMED");
    Serial.println("FILE_WRITE_CONFIRMED");
    Serial.println("FILE_READBACK_CONFIRMED");
    Serial.println("FILE_APPEND_CONFIRMED");
  } else {
    Serial.println("SD_RW_LAB_FAIL");
  }

  Serial.println();
  Serial.println("Files are intentionally left on the card in /cyd_lab");
  Serial.println("so they can also be inspected later on a computer.");
}

void loop() {
  // One-shot laboratory test. Press RESET/EN to run it again.
}
