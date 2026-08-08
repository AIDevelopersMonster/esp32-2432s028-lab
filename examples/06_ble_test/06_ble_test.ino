/*
 * ESP32-2432S028R / Cheap Yellow Display (CYD)
 * ------------------------------------------------------------
 * Example: 06_ble_test
 * Purpose: verify the ESP32 built-in Bluetooth Low Energy radio using
 *          advertising, a simple GATT service and bidirectional data flow.
 *
 * Project:
 *   https://github.com/AIDevelopersMonster/esp32-2432s028-lab
 *
 * Documentation:
 *   examples/06_ble_test/README.md
 *
 * Status:
 *   READY FOR HARDWARE TEST
 *   Not yet marked VERIFIED until the real board is tested.
 *
 * Hardware:
 *   - Board: ESP32-2432S028R / CYD
 *   - No external BLE module is required
 *   - No GPIO connections are required
 *
 * Required library:
 *   - BLE support from the ESP32 Arduino core
 *
 * The sketch advertises as:
 *   CYD-BLE
 *
 * GATT layout:
 *   Service UUID:
 *     4fafc201-1fb5-459e-8fcc-c5c9c331914b
 *
 *   Characteristic UUID:
 *     beb5483e-36e1-4688-b7f5-ea07361b26a8
 *
 * Characteristic properties:
 *   READ, WRITE, NOTIFY
 *
 * Test idea:
 *   1. Upload and open Serial Monitor at 115200 baud.
 *   2. Scan with a BLE application and find CYD-BLE.
 *   3. Connect and open the characteristic.
 *   4. Read the initial value.
 *   5. Enable notifications: ESP32 sends a counter every 5 seconds.
 *   6. Write text from the phone; ESP32 prints it to USB Serial and
 *      updates/notifies the characteristic with an ACK value.
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

static const char *DEVICE_NAME = "CYD-BLE";
static const char *SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
static const char *CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
static const uint32_t NOTIFY_INTERVAL_MS = 5000;

BLECharacteristic *testCharacteristic = nullptr;
bool clientConnected = false;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *server) override {
    (void)server;
    clientConnected = true;
    Serial.println("BLE_CLIENT_CONNECTED");
  }

  void onDisconnect(BLEServer *server) override {
    (void)server;
    clientConnected = false;
    Serial.println("BLE_CLIENT_DISCONNECTED");

    // Resume advertising so a client can reconnect without resetting the board.
    BLEDevice::startAdvertising();
    Serial.println("BLE_ADVERTISING_RESTARTED");
  }
};

class CharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) override {
    // Current Arduino-ESP32 BLE API returns an Arduino String here.
    const String value = characteristic->getValue();

    if (value.length() == 0) {
      Serial.println("BLE_WRITE_EMPTY");
      return;
    }

    Serial.print("BLE RX: ");
    Serial.println(value);

    // Return a simple acknowledgement through the same characteristic.
    String ack = "ACK:" + value;
    characteristic->setValue(ack.c_str());
    characteristic->notify();
    Serial.println("BLE_WRITE_ACK_NOTIFIED");
  }
};

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("ESP32-2432S028R BLE GATT test");
  Serial.printf("Starting BLE device: %s\n", DEVICE_NAME);

  // Initialize the BLE host stack and assign the advertised device name.
  BLEDevice::init(DEVICE_NAME);

  // Create a GATT server and observe connection/disconnection events.
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  // Create one intentionally simple service for the laboratory test.
  BLEService *service = server->createService(SERVICE_UUID);

  // One characteristic is enough to demonstrate read, write and notify.
  testCharacteristic = service->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY);

  // Client Characteristic Configuration descriptor allows BLE clients to
  // enable/disable notifications in the standard way.
  testCharacteristic->addDescriptor(new BLE2902());
  testCharacteristic->setCallbacks(new CharacteristicCallbacks());
  testCharacteristic->setValue("CYD BLE ready");

  service->start();

  // Publish the service UUID so scanners can identify the test service.
  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("CYD_BLE_READY");
  Serial.printf("Service UUID: %s\n", SERVICE_UUID);
  Serial.printf("Characteristic UUID: %s\n", CHARACTERISTIC_UUID);
  Serial.println("Scan for CYD-BLE and connect with a BLE/GATT client.");
}

void loop() {
  // When a client is connected, periodically update the characteristic and
  // send a notification. This demonstrates the ESP32 -> BLE client path.
  static uint32_t lastNotify = 0;
  static uint32_t counter = 0;

  const uint32_t now = millis();
  if (clientConnected && now - lastNotify >= NOTIFY_INTERVAL_MS) {
    lastNotify = now;
    ++counter;

    String message = "CYD BLE counter=" + String(counter);
    testCharacteristic->setValue(message.c_str());
    testCharacteristic->notify();

    Serial.print("BLE TX: ");
    Serial.println(message);
  }

  delay(10);
}
