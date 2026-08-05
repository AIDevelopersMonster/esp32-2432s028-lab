#pragma once

#include <Arduino.h>

namespace cyd::b1 {

enum class Evidence : uint8_t {
  SoftwareTested,
  ComponentAndSoftware,
  BoardTraceAndSoftware,
  BoardFunction,
  ConnectorLabel,
  ConnectorLabelAndSoftware,
  Unresolved,
};

enum class Direction : uint8_t {
  Input,
  Output,
  Bidirectional,
  InputOnly,
};

struct Signal {
  const char* name;
  int8_t gpio;
  Direction direction;
  Evidence evidence;
  bool activeLow;
  bool bootStrap;
  const char* owner;
  const char* conflict;
};

inline constexpr const char* PROFILE_ID = "esp32-2432s028r-sample-a";
inline constexpr uint32_t PROFILE_SCHEMA_VERSION = 1;

inline constexpr Signal SIGNALS[] = {
    {"tft_miso", 12, Direction::Input, Evidence::SoftwareTested, false, false, "display", nullptr},
    {"tft_mosi", 13, Direction::Output, Evidence::SoftwareTested, false, false, "display", nullptr},
    {"tft_sclk", 14, Direction::Output, Evidence::SoftwareTested, false, false, "display", nullptr},
    {"tft_cs", 15, Direction::Output, Evidence::SoftwareTested, true, false, "display", nullptr},
    {"tft_dc", 2, Direction::Output, Evidence::SoftwareTested, false, false, "display", nullptr},
    {"tft_backlight", 21, Direction::Output, Evidence::SoftwareTested, false, false, "display", "p3_gpio21"},
    {"touch_irq", 36, Direction::InputOnly, Evidence::ComponentAndSoftware, true, false, "touch", nullptr},
    {"touch_mosi", 32, Direction::Output, Evidence::ComponentAndSoftware, false, false, "touch", nullptr},
    {"touch_miso", 39, Direction::InputOnly, Evidence::ComponentAndSoftware, false, false, "touch", nullptr},
    {"touch_sclk", 25, Direction::Output, Evidence::ComponentAndSoftware, false, false, "touch", nullptr},
    {"touch_cs", 33, Direction::Output, Evidence::ComponentAndSoftware, true, false, "touch", nullptr},
    {"sd_miso", 19, Direction::Input, Evidence::SoftwareTested, false, false, "microSD", nullptr},
    {"sd_mosi", 23, Direction::Output, Evidence::SoftwareTested, false, false, "microSD", nullptr},
    {"sd_sclk", 18, Direction::Output, Evidence::SoftwareTested, false, false, "microSD", nullptr},
    {"sd_cs", 5, Direction::Output, Evidence::SoftwareTested, true, false, "microSD", nullptr},
    {"led_red", 4, Direction::Output, Evidence::SoftwareTested, true, false, "rgb_led", nullptr},
    {"led_green", 16, Direction::Output, Evidence::SoftwareTested, true, false, "rgb_led", nullptr},
    {"led_blue", 17, Direction::Output, Evidence::SoftwareTested, true, false, "rgb_led", nullptr},
    {"ldr", 34, Direction::InputOnly, Evidence::SoftwareTested, false, false, "ldr", nullptr},
    {"audio", 26, Direction::Output, Evidence::BoardTraceAndSoftware, false, false, "audio", nullptr},
    {"boot_button", 0, Direction::Input, Evidence::SoftwareTested, true, true, "boot_button", nullptr},
    {"uart0_tx", 1, Direction::Output, Evidence::BoardFunction, false, false, "programming-console", nullptr},
    {"uart0_rx", 3, Direction::Input, Evidence::BoardFunction, false, false, "programming-console", nullptr},
    {"p3_gpio35", 35, Direction::InputOnly, Evidence::ConnectorLabel, false, false, "expansion-candidate", nullptr},
    {"p3_gpio22", 22, Direction::Bidirectional, Evidence::ConnectorLabel, false, false, "expansion-candidate", nullptr},
    {"p3_gpio21", 21, Direction::Output, Evidence::ConnectorLabelAndSoftware, false, false, "display-backlight-shared", "tft_backlight"},
    {"cn1_gpio27", 27, Direction::Bidirectional, Evidence::ConnectorLabel, false, false, "expansion-candidate", nullptr},
};

inline constexpr size_t SIGNAL_COUNT = sizeof(SIGNALS) / sizeof(SIGNALS[0]);

constexpr bool isModuleFlashGpio(int gpio) {
  return gpio >= 6 && gpio <= 11;
}

constexpr bool isInputOnlyGpio(int gpio) {
  return gpio == 34 || gpio == 35 || gpio == 36 || gpio == 39;
}

constexpr bool validateProfile() {
  for (size_t i = 0; i < SIGNAL_COUNT; ++i) {
    const Signal& signal = SIGNALS[i];
    if (signal.gpio < 0 || signal.gpio > 39 || isModuleFlashGpio(signal.gpio)) {
      return false;
    }
    if (isInputOnlyGpio(signal.gpio) &&
        signal.direction != Direction::Input &&
        signal.direction != Direction::InputOnly) {
      return false;
    }
  }
  return true;
}

static_assert(validateProfile(), "Invalid B1 board profile");

}  // namespace cyd::b1
