#include <SPI.h>
#include <SD.h>
#include <CYD_Board.h>

SPIClass sdSpi(VSPI);

void listDirectory(fs::FS &fs, const char *path, uint8_t levels) {
  File root = fs.open(path);
  if (!root || !root.isDirectory()) {
    Serial.println("Failed to open directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.printf("DIR : %s\n", file.name());
      if (levels > 0) {
        listDirectory(fs, file.path(), levels - 1);
      }
    } else {
      Serial.printf("FILE: %-24s SIZE: %u\n", file.name(), static_cast<unsigned>(file.size()));
    }
    file = root.openNextFile();
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  sdSpi.begin(cyd::SD_SCLK_PIN,
              cyd::SD_MISO_PIN,
              cyd::SD_MOSI_PIN,
              cyd::SD_CS_PIN);

  if (!SD.begin(cyd::SD_CS_PIN, sdSpi)) {
    Serial.println("microSD mount failed");
    return;
  }

  const uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No microSD card detected");
    return;
  }

  Serial.printf("Card size: %llu MB\n", SD.cardSize() / (1024ULL * 1024ULL));
  listDirectory(SD, "/", 2);
}

void loop() {
}
