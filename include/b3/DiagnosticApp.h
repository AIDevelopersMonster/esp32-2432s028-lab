#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

#include <b2/SpiCoordinator.h>
#include <b3/TouchCalibration.h>

namespace cyd::b3 {

enum class UiMode : uint8_t { Calibration, Diagnostics };

struct DiagnosticSnapshot {
  uint32_t sequence = 0;
  uint32_t uptimeMs = 0;
  UiMode mode = UiMode::Calibration;
  bool calibrationValid = false;
  uint8_t calibrationStep = 0;
  RawPoint raw{};
  ScreenPoint mapped{};
  bool touchPressed = false;
  bool storagePresent = false;
  uint64_t storageBytes = 0;
  bool chipSelectIdle = false;
  uint32_t ownerErrors = 0;
};

class DiagnosticApp {
 public:
  DiagnosticApp(TFT_eSPI& display, b2::SpiCoordinator& spi);
  void begin();
  bool due(uint32_t nowMs) const;
  const DiagnosticSnapshot& run(uint32_t nowMs);
  void render(const DiagnosticSnapshot& snapshot);
  void printSerial(const DiagnosticSnapshot& snapshot) const;

 private:
  TFT_eSPI& display_;
  b2::SpiCoordinator& spi_;
  CalibrationStore store_;
  FourPointCalibration calibration_;
  CalibrationData calibrationData_{};
  DiagnosticSnapshot snapshot_{};
  uint32_t lastRunMs_ = 0;
  bool previousPressed_ = false;

  void probeStorage();
  void drawFrame();
  void drawCalibrationTarget();
  void handleCalibrationRelease();
  static const char* modeText(UiMode mode);
};

}  // namespace cyd::b3
