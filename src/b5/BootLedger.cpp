#include <b5/BootLedger.h>

namespace cyd::b5 {
namespace {
constexpr const char* NS = "b5_boot";
constexpr const char* KEY = "ledger";
}

uint32_t BootLedger::checksum(const BootLedgerRecord& record) {
  const uint32_t* words = reinterpret_cast<const uint32_t*>(&record);
  uint32_t value = 0xB5005EEDU;
  for (size_t i = 0; i < (sizeof(BootLedgerRecord) / sizeof(uint32_t)) - 1; ++i) {
    value = (value << 5U) | (value >> 27U);
    value ^= words[i];
  }
  return value;
}

bool BootLedger::valid(const BootLedgerRecord& record) {
  return record.schema == 1 && record.checksum == checksum(record);
}

bool BootLedger::store() {
  record_.checksum = checksum(record_);
  return prefs_.putBytes(KEY, &record_, sizeof(record_)) == sizeof(record_);
}

bool BootLedger::begin(uint32_t resetReason) {
  if (!prefs_.begin(NS, false)) return false;
  BootLedgerRecord loaded{};
  const size_t bytes = prefs_.getBytes(KEY, &loaded, sizeof(loaded));
  if (bytes == sizeof(loaded) && valid(loaded)) {
    record_ = loaded;
    state_ = LedgerState::Valid;
  } else {
    record_ = BootLedgerRecord{};
    state_ = bytes == 0 ? LedgerState::Created : LedgerState::Recovered;
  }
  ++record_.bootCount;
  ++record_.consecutiveUnhealthyBoots;
  record_.lastResetReason = resetReason;
  return store();
}

bool BootLedger::markHealthy(uint32_t uptimeMs) {
  record_.consecutiveUnhealthyBoots = 0;
  record_.lastHealthyUptimeMs = uptimeMs;
  return store();
}

bool BootLedger::enterContainment() {
  ++record_.containmentEntries;
  return store();
}

const char* ledgerStateText(LedgerState state) {
  switch (state) {
    case LedgerState::Valid: return "valid";
    case LedgerState::Created: return "created";
    case LedgerState::Recovered: return "recovered";
    default: return "corrupt";
  }
}

}  // namespace cyd::b5
