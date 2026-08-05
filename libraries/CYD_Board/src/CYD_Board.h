#pragma once

#include <Arduino.h>

namespace cyd {

// TFT ILI9341 (HSPI wiring)
constexpr uint8_t TFT_MISO_PIN = 12;
constexpr uint8_t TFT_MOSI_PIN = 13;
constexpr uint8_t TFT_SCLK_PIN = 14;
constexpr uint8_t TFT_CS_PIN = 15;
constexpr uint8_t TFT_DC_PIN = 2;
constexpr int8_t TFT_RST_PIN = -1;
constexpr uint8_t TFT_BACKLIGHT_PIN = 21;

// Resistive touch XPT2046
constexpr uint8_t TOUCH_IRQ_PIN = 36;
constexpr uint8_t TOUCH_MOSI_PIN = 32;
constexpr uint8_t TOUCH_MISO_PIN = 39;
constexpr uint8_t TOUCH_CLK_PIN = 25;
constexpr uint8_t TOUCH_CS_PIN = 33;

// microSD
constexpr uint8_t SD_MISO_PIN = 19;
constexpr uint8_t SD_MOSI_PIN = 23;
constexpr uint8_t SD_SCLK_PIN = 18;
constexpr uint8_t SD_CS_PIN = 5;

// On-board devices
constexpr uint8_t LED_RED_PIN = 4;
constexpr uint8_t LED_GREEN_PIN = 16;
constexpr uint8_t LED_BLUE_PIN = 17;
constexpr uint8_t LDR_PIN = 34;
constexpr uint8_t AUDIO_PIN = 26;
constexpr uint8_t BOOT_BUTTON_PIN = 0;

inline void beginBasicHardware() {
  pinMode(TFT_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(TFT_BACKLIGHT_PIN, HIGH);

  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);

  // RGB LED is active LOW.
  digitalWrite(LED_RED_PIN, HIGH);
  digitalWrite(LED_GREEN_PIN, HIGH);
  digitalWrite(LED_BLUE_PIN, HIGH);

  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LDR_PIN, INPUT);
}

inline void setBacklight(bool enabled) {
  digitalWrite(TFT_BACKLIGHT_PIN, enabled ? HIGH : LOW);
}

inline void setRgb(bool red, bool green, bool blue) {
  digitalWrite(LED_RED_PIN, red ? LOW : HIGH);
  digitalWrite(LED_GREEN_PIN, green ? LOW : HIGH);
  digitalWrite(LED_BLUE_PIN, blue ? LOW : HIGH);
}

inline void rgbOff() {
  setRgb(false, false, false);
}

inline uint16_t readLightRaw() {
  return static_cast<uint16_t>(analogRead(LDR_PIN));
}

}  // namespace cyd
