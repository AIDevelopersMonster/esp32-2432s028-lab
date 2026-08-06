#pragma once

#include <Arduino.h>
#include <SPI.h>

namespace cyd::b2 {

enum class SpiOwner : uint8_t { None, Display, Touch, Storage };

struct SpiCounters {
  uint32_t displayTransactions = 0;
  uint32_t touchTransactions = 0;
  uint32_t storageTransactions = 0;
  uint32_t ownershipViolations = 0;
};

class SpiCoordinator {
 public:
  void begin();
  bool acquire(SpiOwner owner);
  void release(SpiOwner owner);
  SpiOwner owner() const { return owner_; }
  const SpiCounters& counters() const { return counters_; }
  SPIClass& storageBus() { return storageBus_; }

  bool readTouchRaw(uint16_t& x, uint16_t& y, uint16_t& z);
  void noteDisplayTransaction();

 private:
  SpiOwner owner_ = SpiOwner::None;
  SpiCounters counters_{};
  SPIClass storageBus_{VSPI};

  uint16_t touchTransfer12(uint8_t command);
  uint8_t touchTransfer8(uint8_t value);
};

const char* ownerText(SpiOwner owner);

}  // namespace cyd::b2
