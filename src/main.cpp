#include <Arduino.h>
#include <TFT_eSPI.h>

#include <b2/SpiCoordinator.h>
#include <b6/ServiceApp.h>

namespace {
TFT_eSPI display;
cyd::b2::SpiCoordinator spi;
cyd::b6::ServiceApp app(display, spi);
}

void setup() {
  Serial.begin(115200);
  delay(300);
  app.begin();
  Serial.println("ESP32-2432S028R B6");
}

void loop() {
  const uint32_t now = millis();
  if (!app.due(now)) return;
  const auto& snapshot = app.run(now);
  app.render(snapshot);
  app.printSerial(snapshot);
}
