#pragma once

#include <Arduino.h>

namespace cyd::b7 {

struct ResourceSnapshot {
  uint32_t freeHeap = 0;
  uint32_t minFreeHeap = 0;
  uint32_t largestBlock = 0;
  uint32_t loopStackHighWater = 0;
  uint32_t samples = 0;
};

class ResourceWatermarks {
 public:
  void begin();
  const ResourceSnapshot& sample();
  const ResourceSnapshot& snapshot() const { return snapshot_; }

 private:
  ResourceSnapshot snapshot_{};
};

}  // namespace cyd::b7
