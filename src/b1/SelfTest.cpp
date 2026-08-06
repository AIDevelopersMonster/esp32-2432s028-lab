#include <b1/SelfTest.h>

#include <CYD_Board.h>
#include <b1/BoardProfile.h>

namespace cyd::b1 {
namespace {
constexpr uint32_t TEST_INTERVAL_MS = 500;
constexpr uint16_t PANEL_WIDTH = 320;

TestResult makeResult(const char* id, TestState state, int32_t value, const char* detail) {
  return TestResult{id, state, value, detail};
}
}  // namespace

SelfTest::SelfTest(TFT_eSPI& display) : display_(display) {}

void SelfTest::begin() {
  cyd::beginBasicHardware();
  analogReadResolution(12);

  display_.init();
  display_.setRotation(1);
  drawStaticUi();

  snapshot_.profile = makeResult("profile", validateProfile() ? TestState::Pass : TestState::Fail,
                                 static_cast<int32_t>(SIGNAL_COUNT), PROFILE_ID);
  snapshot_.display = makeResult("display", TestState::Pass, 1, "ILI9341 path initialized");
  snapshot_.rgb = makeResult("rgb", TestState::NotRun, 0, "active-low sequence");
  snapshot_.ldr = makeResult("ldr", TestState::NotRun, 0, "12-bit raw ADC");
  snapshot_.button = makeResult("button", TestState::NotRun, 0, "GPIO0 active-low");
}

bool SelfTest::due(uint32_t nowMs) const {
  return nowMs - lastRunMs_ >= TEST_INTERVAL_MS;
}

const Snapshot& SelfTest::run(uint32_t nowMs) {
  lastRunMs_ = nowMs;
  snapshot_.sequence++;
  snapshot_.uptimeMs = nowMs;
  snapshot_.lightRaw = cyd::readLightRaw();
  snapshot_.bootPressed = digitalRead(cyd::BOOT_BUTTON_PIN) == LOW;

  snapshot_.ldr = makeResult("ldr", TestState::Pass, snapshot_.lightRaw,
                             "raw value; illumination not calibrated");
  snapshot_.button = makeResult("button", TestState::Pass, snapshot_.bootPressed ? 1 : 0,
                                snapshot_.bootPressed ? "pressed" : "released");

  advanceRgb();
  snapshot_.rgb = makeResult("rgb", TestState::Pass, rgbStep_,
                             "commanded state only; optical feedback not measured");
  return snapshot_;
}

void SelfTest::advanceRgb() {
  switch (rgbStep_++ % 4U) {
    case 0:
      cyd::setRgb(true, false, false);
      break;
    case 1:
      cyd::setRgb(false, true, false);
      break;
    case 2:
      cyd::setRgb(false, false, true);
      break;
    default:
      cyd::rgbOff();
      break;
  }
}

void SelfTest::drawStaticUi() {
  display_.fillScreen(TFT_BLACK);
  display_.setTextDatum(TL_DATUM);
  display_.setTextColor(TFT_YELLOW, TFT_BLACK);
  display_.drawString("B1 ELECTRICAL TRUTH", 10, 8, 2);
  display_.setTextColor(TFT_WHITE, TFT_BLACK);
  display_.drawString(PROFILE_ID, 10, 28, 2);
  display_.drawFastHLine(10, 48, PANEL_WIDTH - 20, TFT_DARKGREY);

  display_.drawString("Profile", 10, 60, 2);
  display_.drawString("Display", 10, 84, 2);
  display_.drawString("RGB command", 10, 108, 2);
  display_.drawString("LDR raw", 10, 132, 2);
  display_.drawString("BOOT", 10, 156, 2);
  display_.drawString("Seq", 10, 180, 2);

  display_.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  display_.drawString("Serial: machine-readable B1 records", 10, 214, 2);
}

void SelfTest::render(const Snapshot& snapshot) {
  display_.fillRect(150, 56, 160, 150, TFT_BLACK);

  const TestResult results[] = {
      snapshot.profile,
      snapshot.display,
      snapshot.rgb,
      snapshot.ldr,
      snapshot.button,
  };
  const int y[] = {60, 84, 108, 132, 156};
  for (size_t i = 0; i < 5; ++i) {
    display_.setTextColor(stateColor(results[i].state), TFT_BLACK);
    if (i == 3) {
      display_.drawNumber(results[i].value, 150, y[i], 2);
    } else if (i == 4) {
      display_.drawString(snapshot.bootPressed ? "PRESSED" : "released", 150, y[i], 2);
    } else {
      display_.drawString(stateText(results[i].state), 150, y[i], 2);
    }
  }
  display_.setTextColor(TFT_CYAN, TFT_BLACK);
  display_.drawNumber(snapshot.sequence, 150, 180, 2);
}

void SelfTest::printSerial(const Snapshot& snapshot) const {
  Serial.printf(
      "B1 profile=%s schema=%lu seq=%lu uptime_ms=%lu ldr_raw=%u boot=%s "
      "profile_state=%s display_state=%s rgb_state=%s\n",
      PROFILE_ID,
      static_cast<unsigned long>(PROFILE_SCHEMA_VERSION),
      static_cast<unsigned long>(snapshot.sequence),
      static_cast<unsigned long>(snapshot.uptimeMs),
      snapshot.lightRaw,
      snapshot.bootPressed ? "pressed" : "released",
      stateText(snapshot.profile.state),
      stateText(snapshot.display.state),
      stateText(snapshot.rgb.state));
}

const char* SelfTest::stateText(TestState state) {
  switch (state) {
    case TestState::Pass:
      return "PASS";
    case TestState::Warn:
      return "WARN";
    case TestState::Fail:
      return "FAIL";
    case TestState::Unsupported:
      return "UNSUPPORTED";
    default:
      return "NOT_RUN";
  }
}

uint16_t SelfTest::stateColor(TestState state) {
  switch (state) {
    case TestState::Pass:
      return TFT_GREEN;
    case TestState::Warn:
      return TFT_ORANGE;
    case TestState::Fail:
      return TFT_RED;
    case TestState::Unsupported:
      return TFT_DARKGREY;
    default:
      return TFT_LIGHTGREY;
  }
}

}  // namespace cyd::b1
