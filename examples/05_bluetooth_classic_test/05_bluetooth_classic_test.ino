/*
 * ESP32-2432S028R / Cheap Yellow Display (CYD)
 * ------------------------------------------------------------
 * Example: 05_bluetooth_classic_test
 * Purpose: verify the ESP32 built-in Bluetooth Classic radio and
 *          bidirectional Serial Port Profile (SPP) data exchange.
 *
 * Project:
 *   https://github.com/AIDevelopersMonster/esp32-2432s028-lab
 *
 * Documentation:
 *   examples/05_bluetooth_classic_test/README.md
 *
 * Video:
 *   ESP32-2432S028R / CYD: проверка Bluetooth Classic SPP — двусторонняя связь с телефоном
 *   https://youtube.com/shorts/fpp9V5QYG7o
 *
 * Status:
 *   VERIFIED WORKING on 2026-08-08
 *
 * Hardware:
 *   - Board: ESP32-2432S028R / CYD
 *   - MCU family: original ESP32 with Bluetooth Classic + BLE
 *   - No external Bluetooth module is required
 *   - No GPIO connections are required
 *
 * Required library:
 *   - BluetoothSerial from the ESP32 Arduino core
 *
 * The sketch creates a Bluetooth Classic SPP device named:
 *   CYD-BT-CLASSIC
 *
 * Test idea:
 *   1. Upload the sketch and open Serial Monitor at 115200 baud.
 *   2. Pair/connect a phone or computer to CYD-BT-CLASSIC.
 *   3. A connected SPP terminal receives a heartbeat every 5 seconds.
 *   4. Data sent from Bluetooth is printed to USB Serial and echoed back.
 *   5. Data typed in Arduino Serial Monitor is forwarded to Bluetooth.
 *
 * This verifies both directions:
 *   USB Serial -> Bluetooth SPP
 *   Bluetooth SPP -> USB Serial + Bluetooth echo
 */

#include <Arduino.h>
#include "BluetoothSerial.h"

// The Arduino ESP32 core must be built with Bluetooth enabled.
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled in the installed ESP32 Arduino core.
#endif

// Bluetooth Classic SPP is available on the original ESP32 family used here.
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Bluetooth Classic SPP is not available for the selected ESP32 target.
#endif

BluetoothSerial SerialBT;

static const char *DEVICE_NAME = "CYD-BT-CLASSIC";
static const uint32_t HEARTBEAT_INTERVAL_MS = 5000;

void setup() {
  // USB Serial is the diagnostic side of the bridge.
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("ESP32-2432S028R Bluetooth Classic SPP test");
  Serial.printf("Starting Bluetooth device: %s\n", DEVICE_NAME);

  // Start Bluetooth Classic in SPP server mode.
  // The phone/computer should discover this exact device name.
  SerialBT.begin(DEVICE_NAME);

  Serial.println("CYD_BT_CLASSIC_READY");
  Serial.println("Pair/connect with an SPP terminal.");
  Serial.println("Bluetooth data will be echoed and copied to USB Serial.");
  Serial.println("USB Serial data will be forwarded to Bluetooth.");
}

void loop() {
  // Forward anything typed in Arduino Serial Monitor to the Bluetooth client.
  while (Serial.available()) {
    const int value = Serial.read();
    if (value >= 0) {
      SerialBT.write(static_cast<uint8_t>(value));
    }
  }

  // Receive bytes from the Bluetooth SPP client.
  // Every byte is shown in USB Serial and immediately echoed back so the
  // remote terminal can verify that the return data path also works.
  while (SerialBT.available()) {
    const int value = SerialBT.read();
    if (value >= 0) {
      Serial.write(static_cast<uint8_t>(value));
      SerialBT.write(static_cast<uint8_t>(value));
    }
  }

  // Periodic message makes the transmit path visible even before the user
  // sends any manual test string from the Bluetooth terminal.
  static uint32_t lastHeartbeat = 0;
  const uint32_t now = millis();
  if (now - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
    lastHeartbeat = now;
    SerialBT.println("[CYD] Bluetooth Classic SPP alive");
  }

  delay(10);
}
