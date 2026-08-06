#include <b4/ConfigJournal.h>

#include <cstring>

namespace cyd::b4 {
namespace {
constexpr const char* NAMESPACE = "b4cfg";
constexpr const char* SLOT_KEYS[] = {"slot0", "slot1"};
constexpr const char* ACTIVE_KEY = "active";
}

uint32_t ConfigJournal::checksum(const RuntimeConfig& config) {
  RuntimeConfig copy = config;
  copy.checksum = 0;
  const auto* bytes = reinterpret_cast<const uint8_t*>(&copy);
  uint32_t hash = 2166136261UL;
  for (size_t i = 0; i < sizeof(copy); ++i) {
    hash ^= bytes[i];
    hash *= 16777619UL;
  }
  return hash;
}

bool ConfigJournal::validate(const RuntimeConfig& config) {
  return config.schema == 1 && config.brightness <= 100 &&
         config.sampleIntervalMs >= 100 && config.sampleIntervalMs <= 5000 &&
         config.checksum == checksum(config);
}

bool ConfigJournal::readSlot(uint8_t slot, RuntimeConfig& config) {
  if (slot > 1) return false;
  const size_t size = preferences_.getBytesLength(SLOT_KEYS[slot]);
  return size == sizeof(config) &&
         preferences_.getBytes(SLOT_KEYS[slot], &config, sizeof(config)) == sizeof(config) &&
         validate(config);
}

bool ConfigJournal::writeSlot(uint8_t slot, RuntimeConfig config) {
  config.checksum = checksum(config);
  if (preferences_.putBytes(SLOT_KEYS[slot], &config, sizeof(config)) != sizeof(config)) return false;
  RuntimeConfig verify{};
  return readSlot(slot, verify) && verify.generation == config.generation;
}

ConfigLoadResult ConfigJournal::load() {
  ConfigLoadResult result{};
  if (!preferences_.begin(NAMESPACE, false)) {
    result.state = ConfigState::Corrupt;
    return result;
  }

  RuntimeConfig slots[2]{};
  const bool valid[2] = {readSlot(0, slots[0]), readSlot(1, slots[1])};
  const uint8_t marker = preferences_.getUChar(ACTIVE_KEY, 0xFF);

  if (!valid[0] && !valid[1]) {
    result.state = ConfigState::Empty;
    return result;
  }

  uint8_t selected = valid[1] && (!valid[0] || slots[1].generation > slots[0].generation) ? 1 : 0;
  result.config = slots[selected];
  result.activeSlot = selected;
  result.state = marker == selected ? ConfigState::Valid : ConfigState::Recovered;
  if (result.state == ConfigState::Recovered) {
    result.recoveryCount = preferences_.getUInt("recoveries", 0) + 1;
    preferences_.putUInt("recoveries", result.recoveryCount);
    preferences_.putUChar(ACTIVE_KEY, selected);
  } else {
    result.recoveryCount = preferences_.getUInt("recoveries", 0);
  }
  return result;
}

bool ConfigJournal::commit(const RuntimeConfig& candidate, ConfigLoadResult& result) {
  RuntimeConfig next = candidate;
  if (next.schema != 1 || next.brightness > 100 || next.sampleIntervalMs < 100 ||
      next.sampleIntervalMs > 5000) {
    result.state = ConfigState::Corrupt;
    return false;
  }
  const uint8_t target = result.activeSlot == 0 ? 1 : 0;
  next.generation = result.config.generation + 1;
  if (!writeSlot(target, next)) {
    result.state = ConfigState::WriteFailed;
    return false;
  }
  if (preferences_.putUChar(ACTIVE_KEY, target) != 1) {
    result.state = ConfigState::WriteFailed;
    return false;
  }
  result.config = next;
  result.config.checksum = checksum(result.config);
  result.activeSlot = target;
  result.state = ConfigState::Valid;
  return true;
}

bool ConfigJournal::clear() {
  if (!preferences_.begin(NAMESPACE, false)) return false;
  return preferences_.clear();
}

const char* ConfigJournal::stateText(ConfigState state) {
  switch (state) {
    case ConfigState::Valid: return "valid";
    case ConfigState::Recovered: return "recovered";
    case ConfigState::Corrupt: return "corrupt";
    case ConfigState::WriteFailed: return "write_failed";
    default: return "empty";
  }
}

}  // namespace cyd::b4
