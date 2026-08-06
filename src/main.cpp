#include <Arduino.h>
#include <TFT_eSPI.h>

#include <b2/SpiCoordinator.h>
#include <b7/QualificationApp.h>
#include <b8/ReleaseIdentity.h>

namespace {
TFT_eSPI display;
cyd::b2::SpiCoordinator spi;
cyd::b7::QualificationApp app(display, spi);
}

void setup() {
  Serial.begin(115200);
  delay(300);
  app.begin();
  Serial.printf("ESP32-2432S028R %s release=%s status=%s date=%s\n",
                cyd::b8::MILESTONE, cyd::b8::RELEASE_VERSION,
                cyd::b8::BASELINE_STATUS, cyd::b8::RELEASE_DATE);
}

void loop() {
  const uint32_t now = millis();
  if (!app.due(now)) return;
  const auto& snapshot = app.run(now);
  app.render(snapshot);
  app.printSerial(snapshot);
}
