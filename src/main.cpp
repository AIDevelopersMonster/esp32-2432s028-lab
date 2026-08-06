#include <Arduino.h>
#include <TFT_eSPI.h>

#include <b1/SelfTest.h>

namespace {
TFT_eSPI display;
cyd::b1::SelfTest selfTest(display);
}

void setup() {
  Serial.begin(115200);
  delay(300);

  selfTest.begin();

  Serial.println();
  Serial.println("ESP32-2432S028R B1");
  Serial.println("Electrical truth and modular self-test started");
}

void loop() {
  const uint32_t now = millis();
  if (!selfTest.due(now)) {
    return;
  }

  const cyd::b1::Snapshot& snapshot = selfTest.run(now);
  selfTest.render(snapshot);
  selfTest.printSerial(snapshot);
}
