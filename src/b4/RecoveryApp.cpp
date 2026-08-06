#include <b4/RecoveryApp.h>

#include <SD.h>
#include <esp_system.h>

#include <CYD_Board.h>
#include <b1/BoardProfile.h>

namespace cyd::b4 {
namespace {
constexpr uint32_t RUN_INTERVAL_MS = 250;
constexpr uint32_t FACTORY_RESET_HOLD_MS = 5000;
constexpr uint32_t SD_FREQUENCY_HZ = 10000000;
const char* yesNo(bool value) { return value ? "1" : "0"; }
}

RecoveryApp::RecoveryApp(TFT_eSPI& display, b2::SpiCoordinator& spi)
    : display_(display), spi_(spi) {}

void RecoveryApp::begin() {
  cyd::beginBasicHardware();
  spi_.begin();
  display_.init();
  display_.setRotation(1);

  config_ = journal_.load();
  if (config_.state == ConfigState::Empty) {
    RuntimeConfig defaults{};
    defaults.checksum = ConfigJournal::checksum(defaults);
    journal_.commit(defaults, config_);
  }
  record_.calibrationValid = calibrationStore_.load(calibration_);
  record_.resetReason = static_cast<uint32_t>(esp_reset_reason());
  selectMode();
  probeStorage();
  drawFrame();

  Serial.printf("B4 event=boot profile=%s mode=%s config_state=%s generation=%lu reset_reason=%lu\n",
                b1::PROFILE_ID, modeText(record_.mode), ConfigJournal::stateText(config_.state),
                static_cast<unsigned long>(config_.config.generation),
                static_cast<unsigned long>(record_.resetReason));
}

void RecoveryApp::selectMode() {
  const bool bootHeld = digitalRead(cyd::BOOT_BUTTON_PIN) == LOW;
  if (bootHeld) record_.mode = RecoveryMode::Recovery;
  else if (config_.state == ConfigState::Corrupt || config_.state == ConfigState::WriteFailed)
    record_.mode = RecoveryMode::Safe;
  else
    record_.mode = RecoveryMode::Normal;
}

bool RecoveryApp::due(uint32_t nowMs) const {
  return nowMs - lastRunMs_ >= RUN_INTERVAL_MS;
}

const EvidenceRecord& RecoveryApp::run(uint32_t nowMs) {
  lastRunMs_ = nowMs;
  ++record_.sequence;
  record_.uptimeMs = nowMs;
  record_.configState = config_.state;
  record_.configGeneration = config_.config.generation;
  record_.recoveryCount = config_.recoveryCount;
  record_.chipSelectIdle = digitalRead(cyd::TOUCH_CS_PIN) == HIGH &&
                           digitalRead(cyd::SD_CS_PIN) == HIGH;
  record_.ownerErrors = spi_.counters().ownershipViolations;
  handleBootButton(nowMs);
  spi_.noteDisplayTransaction();
  return record_;
}

void RecoveryApp::handleBootButton(uint32_t nowMs) {
  const bool pressed = digitalRead(cyd::BOOT_BUTTON_PIN) == LOW;
  if (!pressed) {
    bootPressedSince_ = 0;
    if (resetArmed_) {
      resetArmed_ = false;
      record_.mode = RecoveryMode::Recovery;
      drawFrame();
    }
    return;
  }
  if (bootPressedSince_ == 0) bootPressedSince_ = nowMs;
  if (!resetArmed_ && nowMs - bootPressedSince_ >= FACTORY_RESET_HOLD_MS) {
    resetArmed_ = true;
    record_.mode = RecoveryMode::FactoryResetPending;
    Serial.println("B4 event=factory_reset_armed action=release_to_cancel_serial_confirm_required");
    drawFrame();
  }
}

void RecoveryApp::probeStorage() {
  if (!spi_.acquire(b2::SpiOwner::Storage)) return;
  record_.storagePresent = SD.begin(cyd::SD_CS_PIN, spi_.storageBus(), SD_FREQUENCY_HZ);
  digitalWrite(cyd::SD_CS_PIN, HIGH);
  spi_.release(b2::SpiOwner::Storage);
}

void RecoveryApp::drawFrame() {
  display_.fillScreen(TFT_BLACK);
  display_.setTextDatum(TL_DATUM);
  display_.setTextColor(TFT_YELLOW, TFT_BLACK);
  display_.drawString("B4 CONFIG + RECOVERY", 8, 6, 2);
  display_.setTextColor(TFT_WHITE, TFT_BLACK);
  display_.drawString(b1::PROFILE_ID, 8, 26, 2);
  display_.drawFastHLine(8, 46, 304, TFT_DARKGREY);
  display_.drawString("Mode", 8, 60, 2);
  display_.drawString("Config", 8, 86, 2);
  display_.drawString("Generation", 8, 112, 2);
  display_.drawString("Recoveries", 8, 138, 2);
  display_.drawString("Calibration", 8, 164, 2);
  display_.drawString("Storage / SPI", 8, 190, 2);
}

void RecoveryApp::render(const EvidenceRecord& r) {
  display_.fillRect(145, 56, 167, 160, TFT_BLACK);
  display_.setTextColor(r.mode == RecoveryMode::Normal ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
  display_.drawString(modeText(r.mode), 145, 60, 2);
  display_.setTextColor(r.configState == ConfigState::Valid ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
  display_.drawString(ConfigJournal::stateText(r.configState), 145, 86, 2);
  display_.setTextColor(TFT_CYAN, TFT_BLACK);
  display_.drawNumber(r.configGeneration, 145, 112, 2);
  display_.drawNumber(r.recoveryCount, 145, 138, 2);
  display_.setTextColor(r.calibrationValid ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
  display_.drawString(r.calibrationValid ? "VALID" : "MISSING", 145, 164, 2);
  display_.setTextColor(r.storagePresent && r.chipSelectIdle && r.ownerErrors == 0 ? TFT_GREEN : TFT_ORANGE,
                        TFT_BLACK);
  char status[48];
  snprintf(status, sizeof(status), "sd=%u cs=%u err=%lu", r.storagePresent ? 1U : 0U,
           r.chipSelectIdle ? 1U : 0U, static_cast<unsigned long>(r.ownerErrors));
  display_.drawString(status, 145, 190, 2);
}

void RecoveryApp::printSerial(const EvidenceRecord& r) const {
  Serial.printf(
      "B4 profile=%s seq=%lu uptime_ms=%lu mode=%s config_state=%s config_generation=%lu "
      "recovery_count=%lu calibration_valid=%s sd_present=%s cs_idle=%s owner=%s "
      "owner_errors=%lu reset_reason=%lu\n",
      b1::PROFILE_ID, static_cast<unsigned long>(r.sequence),
      static_cast<unsigned long>(r.uptimeMs), modeText(r.mode),
      ConfigJournal::stateText(r.configState), static_cast<unsigned long>(r.configGeneration),
      static_cast<unsigned long>(r.recoveryCount), yesNo(r.calibrationValid),
      yesNo(r.storagePresent), yesNo(r.chipSelectIdle), b2::ownerText(spi_.owner()),
      static_cast<unsigned long>(r.ownerErrors), static_cast<unsigned long>(r.resetReason));
}

const char* RecoveryApp::modeText(RecoveryMode mode) {
  switch (mode) {
    case RecoveryMode::Safe: return "safe";
    case RecoveryMode::Recovery: return "recovery";
    case RecoveryMode::FactoryResetPending: return "factory_reset_pending";
    default: return "normal";
  }
}

}  // namespace cyd::b4
