#include <Arduino.h>
#include <TFT_eSPI.h>
#include <CYD_Board.h>

namespace {
TFT_eSPI tft;
uint32_t lastUpdate = 0;
uint8_t colorStep = 0;

void drawStaticUi() {
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(1);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("ESP32-2432S028R LAB", 12, 12, 4);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Board self-test", 12, 48, 2);
  tft.drawFastHLine(12, 70, 296, TFT_DARKGREY);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("Display: OK", 12, 84, 2);
  tft.drawString("RGB LED: running", 12, 108, 2);
  tft.drawString("LDR raw:", 12, 132, 2);
  tft.drawString("BOOT button:", 12, 156, 2);

  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString("Open Serial Monitor at 115200 baud", 12, 208, 2);
}

void updateRgb() {
  switch (colorStep++ % 4U) {
    case 0: cyd::setRgb(true, false, false); break;
    case 1: cyd::setRgb(false, true, false); break;
    case 2: cyd::setRgb(false, false, true); break;
    default: cyd::rgbOff(); break;
  }
}
}  // namespace

void setup() {
  Serial.begin(115200);
  delay(300);

  cyd::beginBasicHardware();
  analogReadResolution(12);

  tft.init();
  drawStaticUi();

  Serial.println();
  Serial.println("ESP32-2432S028R LAB");
  Serial.println("Self-test started");
}

void loop() {
  const uint32_t now = millis();
  if (now - lastUpdate < 500) {
    return;
  }
  lastUpdate = now;

  const uint16_t light = cyd::readLightRaw();
  const bool bootPressed = digitalRead(cyd::BOOT_BUTTON_PIN) == LOW;

  tft.fillRect(112, 128, 190, 24, TFT_BLACK);
  tft.fillRect(140, 152, 160, 24, TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawNumber(light, 112, 132, 2);
  tft.drawString(bootPressed ? "PRESSED" : "released", 140, 156, 2);

  updateRgb();

  Serial.printf("LDR=%u, BOOT=%s\n", light, bootPressed ? "pressed" : "released");
}
