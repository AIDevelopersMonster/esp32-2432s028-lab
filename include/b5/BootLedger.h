#pragma once

#include <Arduino.h>
#include <Preferences.h>

namespace cyd::b5 {

struct BootLedgerRecord {
  uint32_t schema = 1;
  uint32_t bootCount = 0;
  uint32_t consecutiveUnhealthyBoots = 0;
  uint32_t lastResetReason = 0;
  uint32_t lastHealthyUptimeMs = 0;
  uint32_t containmentEntries = 0;
  uint32_t checksum = 0;
};

enum class LedgerState : uint8_t { Valid, Created, Recovered, Corrupt };

class BootLedger {
 public:
  bool begin(uint32_t resetReason);
  bool markHealthy(uint32_t uptimeMs);
  bool enterContainment();
  const BootLedgerRecord& record() const { return record_; }
  LedgerState state() const { return state_; }

 private:
  Preferences prefs_;
  BootLedgerRecord record_{};
  LedgerState state_ = LedgerState::Corrupt;

  static uint32_t checksum(const BootLedgerRecord& record);
  static bool valid(const BootLedgerRecord& record);
  bool store();
};

const char* ledgerStateText(LedgerState state);

}  // namespace cyd::b5
