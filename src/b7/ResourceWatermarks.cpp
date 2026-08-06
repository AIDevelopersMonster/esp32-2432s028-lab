#include <b7/ResourceWatermarks.h>

#include <esp_heap_caps.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace cyd::b7 {

void ResourceWatermarks::begin() {
  snapshot_.freeHeap = ESP.getFreeHeap();
  snapshot_.minFreeHeap = ESP.getMinFreeHeap();
  snapshot_.largestBlock = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
  snapshot_.loopStackHighWater = uxTaskGetStackHighWaterMark(nullptr);
  snapshot_.samples = 0;
}

const ResourceSnapshot& ResourceWatermarks::sample() {
  snapshot_.freeHeap = ESP.getFreeHeap();
  snapshot_.minFreeHeap = ESP.getMinFreeHeap();
  snapshot_.largestBlock = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
  snapshot_.loopStackHighWater = uxTaskGetStackHighWaterMark(nullptr);
  ++snapshot_.samples;
  return snapshot_;
}

}  // namespace cyd::b7
