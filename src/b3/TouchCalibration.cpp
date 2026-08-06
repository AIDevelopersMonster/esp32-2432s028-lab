#include <b3/TouchCalibration.h>

#include <algorithm>
#include <cstring>

namespace cyd::b3 {
namespace {
constexpr const char* NAMESPACE = "cyd-b3";
constexpr const char* KEY = "touch-cal";
constexpr uint32_t FNV_OFFSET = 2166136261UL;
constexpr uint32_t FNV_PRIME = 16777619UL;

int16_t scaleAxis(uint16_t value, uint16_t minimum, uint16_t maximum, int16_t size, bool invert) {
  if (maximum <= minimum || size <= 1) return -1;
  const uint16_t clamped = std::min<uint16_t>(maximum, std::max<uint16_t>(minimum, value));
  int32_t scaled = static_cast<int32_t>(clamped - minimum) * (size - 1) / (maximum - minimum);
  if (invert) scaled = (size - 1) - scaled;
  return static_cast<int16_t>(scaled);
}
}  // namespace

uint32_t CalibrationStore::checksum(const CalibrationData& data) {
  CalibrationData copy = data;
  copy.checksum = 0;
  const auto* bytes = reinterpret_cast<const uint8_t*>(&copy);
  uint32_t hash = FNV_OFFSET;
  for (size_t i = 0; i < sizeof(copy); ++i) {
    hash ^= bytes[i];
    hash *= FNV_PRIME;
  }
  return hash;
}

bool CalibrationStore::validate(const CalibrationData& data) {
  return data.schema == 1 && data.rotation == 1 &&
         data.xMax > data.xMin + 300 && data.yMax > data.yMin + 300 &&
         data.checksum == checksum(data);
}

bool CalibrationStore::load(CalibrationData& data) {
  Preferences prefs;
  if (!prefs.begin(NAMESPACE, true)) return false;
  const size_t length = prefs.getBytesLength(KEY);
  const size_t read = length == sizeof(data) ? prefs.getBytes(KEY, &data, sizeof(data)) : 0;
  prefs.end();
  return read == sizeof(data) && validate(data);
}

bool CalibrationStore::save(CalibrationData data) {
  data.checksum = checksum(data);
  if (!validate(data)) return false;
  Preferences prefs;
  if (!prefs.begin(NAMESPACE, false)) return false;
  const size_t written = prefs.putBytes(KEY, &data, sizeof(data));
  prefs.end();
  return written == sizeof(data);
}

bool CalibrationStore::erase() {
  Preferences prefs;
  if (!prefs.begin(NAMESPACE, false)) return false;
  const bool removed = prefs.remove(KEY);
  prefs.end();
  return removed;
}

ScreenPoint TouchMapper::map(const RawPoint& raw, int16_t width, int16_t height) const {
  uint16_t sourceX = data_.swapAxes ? raw.y : raw.x;
  uint16_t sourceY = data_.swapAxes ? raw.x : raw.y;
  ScreenPoint point;
  point.x = scaleAxis(sourceX, data_.xMin, data_.xMax, width, data_.invertX);
  point.y = scaleAxis(sourceY, data_.yMin, data_.yMax, height, data_.invertY);
  point.valid = point.x >= 0 && point.y >= 0;
  return point;
}

void FourPointCalibration::reset() {
  count_ = 0;
  std::memset(points_, 0, sizeof(points_));
}

bool FourPointCalibration::add(const RawPoint& point) {
  if (count_ >= TARGET_COUNT || point.z < 100) return false;
  points_[count_++] = point;
  return true;
}

ScreenPoint FourPointCalibration::target(uint8_t index) const {
  static constexpr ScreenPoint targets[TARGET_COUNT] = {
      {24, 24, true}, {295, 24, true}, {295, 215, true}, {24, 215, true}};
  return index < TARGET_COUNT ? targets[index] : ScreenPoint{};
}

CalibrationData FourPointCalibration::build() const {
  CalibrationData data;
  if (!complete()) return data;
  uint16_t xMin = 4095, xMax = 0, yMin = 4095, yMax = 0;
  for (const auto& point : points_) {
    xMin = std::min(xMin, point.y);
    xMax = std::max(xMax, point.y);
    yMin = std::min(yMin, point.x);
    yMax = std::max(yMax, point.x);
  }
  data.xMin = xMin;
  data.xMax = xMax;
  data.yMin = yMin;
  data.yMax = yMax;
  data.checksum = CalibrationStore::checksum(data);
  return data;
}

}  // namespace cyd::b3
