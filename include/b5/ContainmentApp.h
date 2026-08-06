#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <b2/SpiCoordinator.h>
#include <b4/ConfigJournal.h>
#include <b5/BootLedger.h>
#include <b5/DiagnosticArchive.h>

namespace cyd::b5 {

enum class RunMode : uint8_t { Normal, Containment, Recovery };

struct Snapshot {
  uint32_t sequence = 0;
  uint32_t uptimeMs = 0;
  RunMode mode = RunMode::Normal;
  uint32_t bootCount = 0;
  uint32_t unhealthyBoots = 0;
  uint32_t containmentEntries = 0;
  uint32_t resetReason = 0;
  bool healthyMarked = false;
  bool storagePresent = false;
  bool archiveWritable = false;
  uint32_t archiveRecords = 0;
  uint32_t archiveRotations = 0;
  uint64_t archiveBytes = 0;
  bool csIdle = false;
  uint32_t ownerErrors = 0;
};

class ContainmentApp {
 public:
  ContainmentApp(TFT_eSPI& display, b2::SpiCoordinator& spi);
  void begin();
  bool due(uint32_t nowMs) const;
  const Snapshot& run(uint32_t nowMs);
  void render(const Snapshot& snapshot);
  void printSerial(const Snapshot& snapshot);

 private:
  TFT_eSPI& display_;
  b2::SpiCoordinator& spi_;
  BootLedger ledger_;
  DiagnosticArchive archive_;
  Snapshot snapshot_{};
  uint32_t lastRunMs_ = 0;
  bool archiveReady_ = false;

  void drawFrame();
  static const char* modeText(RunMode mode);
};

}  // namespace cyd::b5
