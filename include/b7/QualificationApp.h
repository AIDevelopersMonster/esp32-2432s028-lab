#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

#include <b2/SpiCoordinator.h>
#include <b5/BootLedger.h>
#include <b5/DiagnosticArchive.h>
#include <b6/WatchdogSupervisor.h>
#include <b7/ResourceWatermarks.h>

namespace cyd::b7 {

struct SoakSnapshot {
  uint32_t sequence = 0;
  uint32_t uptimeMs = 0;
  uint32_t bootCount = 0;
  bool healthyMarked = false;
  bool storagePresent = false;
  bool archiveWritable = false;
  uint32_t archiveRecords = 0;
  uint32_t archiveRotations = 0;
  uint64_t archiveBytes = 0;
  ResourceSnapshot resources{};
  uint32_t watchdogFeeds = 0;
  bool csIdle = false;
  uint32_t ownerErrors = 0;
};

class QualificationApp {
 public:
  QualificationApp(TFT_eSPI& display, b2::SpiCoordinator& spi);
  void begin();
  bool due(uint32_t nowMs) const;
  const SoakSnapshot& run(uint32_t nowMs);
  void render(const SoakSnapshot& snapshot);
  void printSerial(const SoakSnapshot& snapshot);

 private:
  TFT_eSPI& display_;
  b2::SpiCoordinator& spi_;
  b5::BootLedger ledger_;
  b5::DiagnosticArchive archive_;
  b6::WatchdogSupervisor watchdog_;
  ResourceWatermarks resources_;
  SoakSnapshot snapshot_{};
  uint32_t lastRunMs_ = 0;
  bool archiveReady_ = false;

  void drawFrame();
};

}  // namespace cyd::b7
