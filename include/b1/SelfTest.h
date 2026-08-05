#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

namespace cyd::b1 {

enum class TestState : uint8_t {
  NotRun,
  Pass,
  Warn,
  Fail,
  Unsupported,
};

struct TestResult {
  const char* id;
  TestState state;
  int32_t value;
  const char* detail;
};

struct Snapshot {
  uint32_t sequence;
  uint32_t uptimeMs;
  uint16_t lightRaw;
  bool bootPressed;
  TestResult profile;
  TestResult display;
  TestResult rgb;
  TestResult ldr;
  TestResult button;
};

class SelfTest {
 public:
  explicit SelfTest(TFT_eSPI& display);

  void begin();
  bool due(uint32_t nowMs) const;
  const Snapshot& run(uint32_t nowMs);
  void render(const Snapshot& snapshot);
  void printSerial(const Snapshot& snapshot) const;

 private:
  TFT_eSPI& display_;
  Snapshot snapshot_{};
  uint32_t lastRunMs_ = 0;
  uint8_t rgbStep_ = 0;

  void drawStaticUi();
  void advanceRgb();
  static const char* stateText(TestState state);
  static uint16_t stateColor(TestState state);
};

}  // namespace cyd::b1
