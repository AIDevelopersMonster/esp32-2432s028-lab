#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

#include <b2/SpiCoordinator.h>

namespace cyd::b2 {

struct HilSnapshot {
  uint32_t sequence = 0;
  uint32_t uptimeMs = 0;
  bool displayOk = false;
  bool touchSampleOk = false;
  bool touchPressed = false;
  uint16_t touchX = 0;
  uint16_t touchY = 0;
  uint16_t touchZ = 0;
  bool storagePresent = false;
  uint64_t storageBytes = 0;
  bool chipSelectIdle = false;
  uint32_t ownershipViolations = 0;
};

class HilSmoke {
 public:
  HilSmoke(TFT_eSPI& display, SpiCoordinator& spi);

  void begin();
  bool due(uint32_t nowMs) const;
  const HilSnapshot& run(uint32_t nowMs);
  void render(const HilSnapshot& snapshot);
  void printSerial(const HilSnapshot& snapshot) const;

 private:
  TFT_eSPI& display_;
  SpiCoordinator& spi_;
  HilSnapshot snapshot_{};
  uint32_t lastRunMs_ = 0;

  void drawStaticUi();
  void probeStorage();
};

}  // namespace cyd::b2
