#pragma once

#include <Arduino.h>
#include <Preferences.h>

namespace cyd::b4 {

enum class ConfigState : uint8_t { Empty, Valid, Recovered, Corrupt, WriteFailed };

struct RuntimeConfig {
  uint32_t schema = 1;
  uint32_t generation = 0;
  uint8_t brightness = 100;
  uint16_t sampleIntervalMs = 250;
  bool diagnosticsEnabled = true;
  uint32_t checksum = 0;
};

struct ConfigLoadResult {
  ConfigState state = ConfigState::Empty;
  RuntimeConfig config{};
  uint8_t activeSlot = 0xFF;
  uint32_t recoveryCount = 0;
};

class ConfigJournal {
 public:
  ConfigLoadResult load();
  bool commit(const RuntimeConfig& candidate, ConfigLoadResult& result);
  bool clear();
  static bool validate(const RuntimeConfig& config);
  static uint32_t checksum(const RuntimeConfig& config);
  static const char* stateText(ConfigState state);

 private:
  Preferences preferences_;
  bool readSlot(uint8_t slot, RuntimeConfig& config);
  bool writeSlot(uint8_t slot, RuntimeConfig config);
};

}  // namespace cyd::b4
