#pragma once

#include <Arduino.h>

namespace cyd::b6 {

enum class FaultMode : uint8_t { None, Stall, Restart, ArchiveFailure };

struct WatchdogStatus {
  bool enabled = false;
  uint32_t timeoutSeconds = 0;
  uint32_t feeds = 0;
  uint32_t lastFeedMs = 0;
  FaultMode armedFault = FaultMode::None;
  bool faultExecuted = false;
};

class WatchdogSupervisor {
 public:
  bool begin(uint32_t timeoutSeconds);
  void feed(uint32_t nowMs);
  bool arm(FaultMode mode);
  void executeArmedFault();
  const WatchdogStatus& status() const { return status_; }

  static const char* faultText(FaultMode mode);

 private:
  WatchdogStatus status_{};
};

}  // namespace cyd::b6
