#pragma once

#include <Arduino.h>
#include <Preferences.h>

namespace cyd::b3 {

struct RawPoint {
  uint16_t x = 0;
  uint16_t y = 0;
  uint16_t z = 0;
};

struct ScreenPoint {
  int16_t x = -1;
  int16_t y = -1;
  bool valid = false;
};

struct CalibrationData {
  uint32_t schema = 1;
  uint16_t xMin = 0;
  uint16_t xMax = 4095;
  uint16_t yMin = 0;
  uint16_t yMax = 4095;
  uint8_t rotation = 1;
  bool swapAxes = true;
  bool invertX = false;
  bool invertY = true;
  uint32_t checksum = 0;
};

class CalibrationStore {
 public:
  bool load(CalibrationData& data);
  bool save(CalibrationData data);
  bool erase();
  static bool validate(const CalibrationData& data);
  static uint32_t checksum(const CalibrationData& data);
};

class TouchMapper {
 public:
  explicit TouchMapper(const CalibrationData& data) : data_(data) {}
  ScreenPoint map(const RawPoint& raw, int16_t width = 320, int16_t height = 240) const;

 private:
  CalibrationData data_;
};

class FourPointCalibration {
 public:
  static constexpr uint8_t TARGET_COUNT = 4;

  void reset();
  bool add(const RawPoint& point);
  bool complete() const { return count_ == TARGET_COUNT; }
  uint8_t count() const { return count_; }
  CalibrationData build() const;
  ScreenPoint target(uint8_t index) const;

 private:
  RawPoint points_[TARGET_COUNT]{};
  uint8_t count_ = 0;
};

}  // namespace cyd::b3
