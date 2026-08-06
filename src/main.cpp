#include <Arduino.h>
#include <TFT_eSPI.h>

#include <b2/HilSmoke.h>
#include <b2/SpiCoordinator.h>

namespace {
TFT_eSPI display;
cyd::b2::SpiCoordinator spi;
cyd::b2::HilSmoke smoke(display, spi);
}

void setup() {
  Serial.begin(115200);
  delay(300);

  smoke.begin();

  Serial.println();
  Serial.println("ESP32-2432S028R B2");
  Serial.println("Unified SPI and HIL smoke test started");
}

void loop() {
  const uint32_t now = millis();
  if (!smoke.due(now)) {
    return;
  }

  const cyd::b2::HilSnapshot& snapshot = smoke.run(now);
  smoke.render(snapshot);
  smoke.printSerial(snapshot);
}
