/*
 * ESP32-2432S028R / Cheap Yellow Display (CYD)
 * ------------------------------------------------------------
 * Example: 08_expansion_io_test
 * Purpose: verify selected expansion GPIOs that are documented for this
 *          board family without depending on any external sensor library.
 *
 * Project:
 *   https://github.com/AIDevelopersMonster/esp32-2432s028-lab
 *
 * Documentation:
 *   examples/08_expansion_io_test/README.md
 *
 * Status:
 *   READY FOR HARDWARE TEST
 *   Do not mark VERIFIED until the actual connector pins are identified on
 *   the tested board revision and the jumper tests are completed.
 *
 * GPIOs used by this laboratory:
 *   GPIO22 : general-purpose digital I/O candidate on expansion connector
 *   GPIO27 : general-purpose digital I/O candidate on expansion connector
 *   GPIO35 : input-only ADC-capable pin candidate on expansion connector
 *
 * IMPORTANT:
 *   Connector ordering varies between CYD revisions. Identify the actual
 *   GPIO pins from the board markings/documentation or continuity before
 *   installing jumpers. Never assume connector position solely from this
 *   sketch.
 *
 * Phase A: GPIO22 <-> GPIO27 loopback
 *   Connect GPIO22 and GPIO27 with ONE jumper.
 *   The sketch drives HIGH/LOW patterns in both directions and verifies that
 *   the receiving pin follows the transmitting pin.
 *
 * Phase B: GPIO35 input observation
 *   Remove the GPIO22-GPIO27 jumper after the automatic loopback test if
 *   desired. Connect GPIO35 first to GND and then to 3V3 while watching
 *   Serial Monitor. GPIO35 is input-only and has no software pull-up/down.
 *
 * Safety:
 *   - ESP32 GPIOs are 3.3 V logic. Do not apply 5 V to these pins.
 *   - Never configure GPIO35 as an output; the ESP32 hardware does not support it.
 */

#include <Arduino.h>

constexpr uint8_t LOOP_A_PIN = 22;
constexpr uint8_t LOOP_B_PIN = 27;
constexpr uint8_t INPUT_ONLY_PIN = 35;
constexpr uint8_t LOOP_ITERATIONS = 20;

bool verifyLevel(uint8_t outputPin, uint8_t inputPin, uint8_t level) {
  digitalWrite(outputPin, level);
  delay(5);
  return digitalRead(inputPin) == level;
}

bool testDirection(uint8_t outputPin, uint8_t inputPin) {
  pinMode(outputPin, OUTPUT);
  pinMode(inputPin, INPUT);

  for (uint8_t i = 0; i < LOOP_ITERATIONS; ++i) {
    if (!verifyLevel(outputPin, inputPin, LOW)) {
      digitalWrite(outputPin, LOW);
      return false;
    }
    if (!verifyLevel(outputPin, inputPin, HIGH)) {
      digitalWrite(outputPin, LOW);
      return false;
    }
  }

  digitalWrite(outputPin, LOW);
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(700);

  Serial.println();
  Serial.println("ESP32-2432S028R expansion GPIO test");
  Serial.println("Required jumper for automatic test: GPIO22 <-> GPIO27");
  Serial.println("Never apply 5 V to ESP32 GPIO pins.");
  Serial.println();

  // GPIO35 is one of the input-only pins on the original ESP32.
  pinMode(INPUT_ONLY_PIN, INPUT);

  Serial.println("EXPANSION_LOOPBACK_START");

  const bool aToB = testDirection(LOOP_A_PIN, LOOP_B_PIN);
  Serial.printf("GPIO22_TO_GPIO27_%s\n", aToB ? "PASS" : "FAIL");

  const bool bToA = testDirection(LOOP_B_PIN, LOOP_A_PIN);
  Serial.printf("GPIO27_TO_GPIO22_%s\n", bToA ? "PASS" : "FAIL");

  // Leave both bidirectional candidates in a safe input state after testing.
  pinMode(LOOP_A_PIN, INPUT);
  pinMode(LOOP_B_PIN, INPUT);

  if (aToB && bToA) {
    Serial.println("EXPANSION_DIGITAL_LOOPBACK_PASS");
  } else {
    Serial.println("EXPANSION_DIGITAL_LOOPBACK_FAIL");
    Serial.println("Check jumper placement and connector pin identification.");
  }

  Serial.println();
  Serial.println("GPIO35_INPUT_MONITOR_READY");
  Serial.println("For manual test: connect GPIO35 to GND, then to 3V3.");
  Serial.println("Observe digital and ADC values below.");
}

void loop() {
  // GPIO35 has no internal pull-up/pull-down, so an unconnected pin may float.
  const int digitalValue = digitalRead(INPUT_ONLY_PIN);
  const int analogValue = analogRead(INPUT_ONLY_PIN);

  Serial.printf("GPIO35 digital=%d adc=%d\n", digitalValue, analogValue);
  delay(500);
}
