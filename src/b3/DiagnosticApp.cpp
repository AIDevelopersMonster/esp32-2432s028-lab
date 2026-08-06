#include <b3/DiagnosticApp.h>

#include <SD.h>

#include <CYD_Board.h>
#include <b1/BoardProfile.h>

namespace cyd::b3 {
namespace {
constexpr uint32_t RUN_INTERVAL_MS = 120;
constexpr uint32_t SD_FREQUENCY_HZ = 10000000;
const char* yesNo(bool value) { return value ? "1" : "0"; }
}  // namespace

DiagnosticApp::DiagnosticApp(TFT_eSPI& display, b2::SpiCoordinator& spi)
    : display_(display), spi_(spi) {}

void DiagnosticApp::begin() {
  cyd::beginBasicHardware();
  spi_.begin();
  display_.init();
  display_.setRotation(1);

  snapshot_.calibrationValid = store_.load(calibrationData_);
  const bool forceCalibration = digitalRead(cyd::BOOT_BUTTON_PIN) == LOW;
  snapshot_.mode = snapshot_.calibrationValid && !forceCalibration
                       ? UiMode::Diagnostics
                       : UiMode::Calibration;
  calibration_.reset();
  probeStorage();
  drawFrame();

  Serial.printf("B3 event=boot profile=%s mode=%s calibration_valid=%s\n",
                b1::PROFILE_ID, modeText(snapshot_.mode), yesNo(snapshot_.calibrationValid));
}

bool DiagnosticApp::due(uint32_t nowMs) const {
  return nowMs - lastRunMs_ >= RUN_INTERVAL_MS;
}

const DiagnosticSnapshot& DiagnosticApp::run(uint32_t nowMs) {
  lastRunMs_ = nowMs;
  ++snapshot_.sequence;
  snapshot_.uptimeMs = nowMs;

  uint16_t x = 0, y = 0, z = 0;
  spi_.readTouchRaw(x, y, z);
  snapshot_.raw = RawPoint{x, y, z};
  snapshot_.touchPressed = digitalRead(cyd::TOUCH_IRQ_PIN) == LOW || z > 100;
  if (snapshot_.touchPressed) lastPressedRaw_ = snapshot_.raw;

  if (snapshot_.mode == UiMode::Calibration) {
    if (previousPressed_ && !snapshot_.touchPressed) handleCalibrationRelease();
    snapshot_.calibrationStep = calibration_.count();
    snapshot_.mapped = ScreenPoint{};
  } else {
    snapshot_.mapped = TouchMapper(calibrationData_).map(snapshot_.raw);
  }
  previousPressed_ = snapshot_.touchPressed;

  snapshot_.chipSelectIdle = digitalRead(cyd::TOUCH_CS_PIN) == HIGH &&
                             digitalRead(cyd::SD_CS_PIN) == HIGH;
  snapshot_.ownerErrors = spi_.counters().ownershipViolations;
  spi_.noteDisplayTransaction();
  return snapshot_;
}

void DiagnosticApp::handleCalibrationRelease() {
  if (!calibration_.add(lastPressedRaw_)) return;
  if (!calibration_.complete()) {
    drawCalibrationTarget();
    return;
  }

  CalibrationData candidate = calibration_.build();
  snapshot_.calibrationValid = store_.save(candidate);
  if (snapshot_.calibrationValid) {
    calibrationData_ = candidate;
    snapshot_.mode = UiMode::Diagnostics;
    drawFrame();
    Serial.printf("B3 event=calibration_saved checksum=%lu xmin=%u xmax=%u ymin=%u ymax=%u\n",
                  static_cast<unsigned long>(candidate.checksum), candidate.xMin,
                  candidate.xMax, candidate.yMin, candidate.yMax);
  } else {
    calibration_.reset();
    drawFrame();
    Serial.println("B3 event=calibration_rejected reason=invalid_range_or_storage");
  }
}

void DiagnosticApp::probeStorage() {
  if (!spi_.acquire(b2::SpiOwner::Storage)) return;
  snapshot_.storagePresent = SD.begin(cyd::SD_CS_PIN, spi_.storageBus(), SD_FREQUENCY_HZ);
  snapshot_.storageBytes = snapshot_.storagePresent ? SD.cardSize() : 0;
  digitalWrite(cyd::SD_CS_PIN, HIGH);
  spi_.release(b2::SpiOwner::Storage);
}

void DiagnosticApp::drawFrame() {
  display_.fillScreen(TFT_BLACK);
  display_.setTextDatum(TL_DATUM);
  display_.setTextColor(TFT_YELLOW, TFT_BLACK);
  display_.drawString("B3 TOUCH + DIAGNOSTICS", 8, 6, 2);
  display_.setTextColor(TFT_WHITE, TFT_BLACK);
  display_.drawString(b1::PROFILE_ID, 8, 26, 2);
  display_.drawFastHLine(8, 46, 304, TFT_DARKGREY);

  if (snapshot_.mode == UiMode::Calibration) {
    display_.drawString("Touch each target, then release", 8, 54, 2);
    display_.drawString("Hold BOOT during reset to recalibrate", 8, 214, 1);
    drawCalibrationTarget();
    return;
  }

  display_.drawString("Calibration", 8, 58, 2);
  display_.drawString("Raw X/Y/Z", 8, 84, 2);
  display_.drawString("Mapped X/Y", 8, 110, 2);
  display_.drawString("microSD", 8, 136, 2);
  display_.drawString("CS idle", 8, 162, 2);
  display_.drawString("Owner errors", 8, 188, 2);
}

void DiagnosticApp::drawCalibrationTarget() {
  display_.fillRect(0, 76, 320, 132, TFT_BLACK);
  const ScreenPoint target = calibration_.target(calibration_.count());
  if (!target.valid) return;
  display_.drawCircle(target.x, target.y, 10, TFT_CYAN);
  display_.drawFastHLine(target.x - 14, target.y, 28, TFT_CYAN);
  display_.drawFastVLine(target.x, target.y - 14, 28, TFT_CYAN);
  display_.setTextColor(TFT_WHITE, TFT_BLACK);
  display_.drawString(String("Target ") + String(calibration_.count() + 1) + "/4", 8, 76, 2);
}

void DiagnosticApp::render(const DiagnosticSnapshot& snapshot) {
  if (snapshot.mode == UiMode::Calibration) {
    display_.fillRect(8, 96, 150, 22, TFT_BLACK);
    display_.setTextColor(snapshot.touchPressed ? TFT_GREEN : TFT_LIGHTGREY, TFT_BLACK);
    display_.drawString(snapshot.touchPressed ? "PRESS" : "release", 8, 96, 2);
    return;
  }

  display_.fillRect(145, 54, 167, 158, TFT_BLACK);
  display_.setTextColor(snapshot.calibrationValid ? TFT_GREEN : TFT_RED, TFT_BLACK);
  display_.drawString(snapshot.calibrationValid ? "VALID" : "INVALID", 145, 58, 2);

  display_.setTextColor(TFT_CYAN, TFT_BLACK);
  char raw[48];
  snprintf(raw, sizeof(raw), "%u/%u/%u", snapshot.raw.x, snapshot.raw.y, snapshot.raw.z);
  display_.drawString(raw, 145, 84, 2);
  char mapped[32];
  snprintf(mapped, sizeof(mapped), "%d/%d", snapshot.mapped.x, snapshot.mapped.y);
  display_.drawString(mapped, 145, 110, 2);

  display_.setTextColor(snapshot.storagePresent ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
  display_.drawString(snapshot.storagePresent ? "PRESENT" : "ABSENT", 145, 136, 2);
  display_.setTextColor(snapshot.chipSelectIdle ? TFT_GREEN : TFT_RED, TFT_BLACK);
  display_.drawString(snapshot.chipSelectIdle ? "PASS" : "FAIL", 145, 162, 2);
  display_.setTextColor(snapshot.ownerErrors == 0 ? TFT_GREEN : TFT_RED, TFT_BLACK);
  display_.drawNumber(snapshot.ownerErrors, 145, 188, 2);

  if (snapshot.touchPressed && snapshot.mapped.valid) {
    display_.fillCircle(snapshot.mapped.x, snapshot.mapped.y, 3, TFT_MAGENTA);
  }
}

void DiagnosticApp::printSerial(const DiagnosticSnapshot& snapshot) const {
  Serial.printf(
      "B3 profile=%s seq=%lu uptime_ms=%lu mode=%s calibration_valid=%s cal_step=%u "
      "pressed=%s raw_x=%u raw_y=%u raw_z=%u mapped_valid=%s mapped_x=%d mapped_y=%d "
      "sd_present=%s sd_bytes=%llu cs_idle=%s owner=%s owner_errors=%lu\n",
      b1::PROFILE_ID, static_cast<unsigned long>(snapshot.sequence),
      static_cast<unsigned long>(snapshot.uptimeMs), modeText(snapshot.mode),
      yesNo(snapshot.calibrationValid), snapshot.calibrationStep,
      yesNo(snapshot.touchPressed), snapshot.raw.x, snapshot.raw.y, snapshot.raw.z,
      yesNo(snapshot.mapped.valid), snapshot.mapped.x, snapshot.mapped.y,
      yesNo(snapshot.storagePresent), static_cast<unsigned long long>(snapshot.storageBytes),
      yesNo(snapshot.chipSelectIdle), b2::ownerText(spi_.owner()),
      static_cast<unsigned long>(snapshot.ownerErrors));
}

const char* DiagnosticApp::modeText(UiMode mode) {
  return mode == UiMode::Calibration ? "calibration" : "diagnostics";
}

}  // namespace cyd::b3
