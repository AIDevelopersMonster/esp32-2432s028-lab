#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

#include <b2/SpiCoordinator.h>
#include <b5/BootLedger.h>
#include <b5/DiagnosticArchive.h>
#include <b6/ServiceExport.h>
#include <b6/WatchdogSupervisor.h>

namespace cyd::b6 {

enum class ServiceMode : uint8_t { Normal, Containment, Recovery };

struct ServiceSnapshot {
  uint32_t sequence = 0;
  uint32_t uptimeMs = 0;
  ServiceMode mode = ServiceMode::Normal;
  uint32_t bootCount = 0;
  uint32_t unhealthyBoots = 0;
  bool healthyMarked = false;
  bool storagePresent = false;
  bool archiveWritable = false;
  bool watchdogEnabled = false;
  uint32_t watchdogFeeds = 0;
  FaultMode fault = FaultMode::None;
  bool exportOk = false;
  uint32_t exports = 0;
  uint32_t exportFiles = 0;
  uint64_t exportBytes = 0;
  bool csIdle = false;
  uint32_t ownerErrors = 0;
};

class ServiceApp {
 public:
  ServiceApp(TFT_eSPI& display, b2::SpiCoordinator& spi);
  void begin();
  bool due(uint32_t nowMs) const;
  const ServiceSnapshot& run(uint32_t nowMs);
  void render(const ServiceSnapshot& snapshot);
  void printSerial(const ServiceSnapshot& snapshot);

 private:
  TFT_eSPI& display_;
  b2::SpiCoordinator& spi_;
  b5::BootLedger ledger_;
  b5::DiagnosticArchive archive_;
  WatchdogSupervisor watchdog_;
  ServiceExport exporter_;
  ServiceSnapshot snapshot_{};
  uint32_t lastRunMs_ = 0;
  bool archiveReady_ = false;
  bool exportRequested_ = false;
  bool archiveFailureInjected_ = false;
  String command_;

  void pollCommands();
  void handleCommand(const String& command);
  void createExport();
  void drawFrame();
  static const char* modeText(ServiceMode mode);
};

}  // namespace cyd::b6
