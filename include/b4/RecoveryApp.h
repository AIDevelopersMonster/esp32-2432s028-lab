#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

#include <b2/SpiCoordinator.h>
#include <b3/TouchCalibration.h>
#include <b4/ConfigJournal.h>

namespace cyd::b4 {

enum class RecoveryMode : uint8_t { Normal, Safe, Recovery, FactoryResetPending };

struct EvidenceRecord {
  uint32_t sequence = 0;
  uint32_t uptimeMs = 0;
  RecoveryMode mode = RecoveryMode::Normal;
  ConfigState configState = ConfigState::Empty;
  uint32_t configGeneration = 0;
  uint32_t recoveryCount = 0;
  bool calibrationValid = false;
  bool storagePresent = false;
  bool chipSelectIdle = false;
  uint32_t ownerErrors = 0;
  uint32_t resetReason = 0;
};

class RecoveryApp {
 public:
  RecoveryApp(TFT_eSPI& display, b2::SpiCoordinator& spi);
  void begin();
  bool due(uint32_t nowMs) const;
  const EvidenceRecord& run(uint32_t nowMs);
  void render(const EvidenceRecord& record);
  void printSerial(const EvidenceRecord& record) const;

 private:
  TFT_eSPI& display_;
  b2::SpiCoordinator& spi_;
  ConfigJournal journal_;
  ConfigLoadResult config_{};
  b3::CalibrationStore calibrationStore_;
  b3::CalibrationData calibration_{};
  EvidenceRecord record_{};
  uint32_t lastRunMs_ = 0;
  uint32_t bootPressedSince_ = 0;
  bool resetArmed_ = false;

  void selectMode();
  void probeStorage();
  void drawFrame();
  void handleBootButton(uint32_t nowMs);
  static const char* modeText(RecoveryMode mode);
};

}  // namespace cyd::b4
