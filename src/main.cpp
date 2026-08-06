#include <Arduino.h>
#include <TFT_eSPI.h>

#include <b2/SpiCoordinator.h>
#include <b4/RecoveryApp.h>

namespace {
TFT_eSPI display;
cyd::b2::SpiCoordinator spi;
cyd::b4::RecoveryApp app(display, spi);
}

void setup() {
  Serial.begin(115200);
  delay(300);
  app.begin();
  Serial.println("ESP32-2432S028R B4");
}

void loop() {
  const uint32_t now = millis();
  if (!app.due(now)) return;
  const auto& record = app.run(now);
  app.render(record);
  app.printSerial(record);
}
