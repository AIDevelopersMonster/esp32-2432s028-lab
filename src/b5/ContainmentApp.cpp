#include <b5/ContainmentApp.h>

#include <SD.h>
#include <esp_system.h>
#include <CYD_Board.h>
#include <b1/BoardProfile.h>

namespace cyd::b5 {
namespace {
constexpr uint32_t RUN_INTERVAL_MS = 1000;
constexpr uint32_t HEALTHY_UPTIME_MS = 15000;
constexpr uint32_t CRASH_LOOP_THRESHOLD = 3;
constexpr uint32_t SD_FREQUENCY_HZ = 10000000;
}

ContainmentApp::ContainmentApp(TFT_eSPI& display, b2::SpiCoordinator& spi)
    : display_(display), spi_(spi) {}

void ContainmentApp::begin() {
  cyd::beginBasicHardware();
  spi_.begin();
  display_.init();
  display_.setRotation(1);

  snapshot_.resetReason = static_cast<uint32_t>(esp_reset_reason());
  ledger_.begin(snapshot_.resetReason);
  const auto& record = ledger_.record();
  snapshot_.bootCount = record.bootCount;
  snapshot_.unhealthyBoots = record.consecutiveUnhealthyBoots;
  snapshot_.containmentEntries = record.containmentEntries;

  if (digitalRead(cyd::BOOT_BUTTON_PIN) == LOW) {
    snapshot_.mode = RunMode::Recovery;
  } else if (record.consecutiveUnhealthyBoots >= CRASH_LOOP_THRESHOLD) {
    snapshot_.mode = RunMode::Containment;
    ledger_.enterContainment();
    snapshot_.containmentEntries = ledger_.record().containmentEntries;
  }

  if (spi_.acquire(b2::SpiOwner::Storage)) {
    snapshot_.storagePresent = SD.begin(cyd::SD_CS_PIN, spi_.storageBus(), SD_FREQUENCY_HZ);
    digitalWrite(cyd::SD_CS_PIN, HIGH);
    spi_.release(b2::SpiOwner::Storage);
  }
  if (snapshot_.storagePresent) archiveReady_ = archive_.begin(SD);
  drawFrame();
  Serial.printf("B5 event=boot profile=%s mode=%s boot_count=%lu unhealthy=%lu reset_reason=%lu\n",
                b1::PROFILE_ID, modeText(snapshot_.mode),
                static_cast<unsigned long>(snapshot_.bootCount),
                static_cast<unsigned long>(snapshot_.unhealthyBoots),
                static_cast<unsigned long>(snapshot_.resetReason));
}

bool ContainmentApp::due(uint32_t nowMs) const {
  return nowMs - lastRunMs_ >= RUN_INTERVAL_MS;
}

const Snapshot& ContainmentApp::run(uint32_t nowMs) {
  lastRunMs_ = nowMs;
  ++snapshot_.sequence;
  snapshot_.uptimeMs = nowMs;
  if (!snapshot_.healthyMarked && snapshot_.mode == RunMode::Normal && nowMs >= HEALTHY_UPTIME_MS) {
    snapshot_.healthyMarked = ledger_.markHealthy(nowMs);
    snapshot_.unhealthyBoots = ledger_.record().consecutiveUnhealthyBoots;
  }
  snapshot_.csIdle = digitalRead(cyd::TOUCH_CS_PIN) == HIGH && digitalRead(cyd::SD_CS_PIN) == HIGH;
  snapshot_.ownerErrors = spi_.counters().ownershipViolations;
  const auto& archiveStatus = archive_.status();
  snapshot_.archiveWritable = archiveReady_ && archiveStatus.writable;
  snapshot_.archiveRecords = archiveStatus.records;
  snapshot_.archiveRotations = archiveStatus.rotations;
  snapshot_.archiveBytes = archiveStatus.bytes;
  spi_.noteDisplayTransaction();
  return snapshot_;
}

void ContainmentApp::drawFrame() {
  display_.fillScreen(TFT_BLACK);
  display_.setTextDatum(TL_DATUM);
  display_.setTextColor(TFT_YELLOW, TFT_BLACK);
  display_.drawString("B5 BOOT LEDGER + ARCHIVE", 8, 6, 2);
  display_.setTextColor(TFT_WHITE, TFT_BLACK);
  display_.drawString("Mode", 8, 48, 2);
  display_.drawString("Boots / unhealthy", 8, 74, 2);
  display_.drawString("Containment entries", 8, 100, 2);
  display_.drawString("Healthy marker", 8, 126, 2);
  display_.drawString("Archive", 8, 152, 2);
  display_.drawString("Archive bytes", 8, 178, 2);
  display_.drawString("SPI invariant", 8, 204, 2);
}

void ContainmentApp::render(const Snapshot& s) {
  display_.fillRect(170, 44, 142, 188, TFT_BLACK);
  display_.setTextColor(s.mode == RunMode::Normal ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
  display_.drawString(modeText(s.mode), 170, 48, 2);
  display_.setTextColor(TFT_CYAN, TFT_BLACK);
  char value[48];
  snprintf(value, sizeof(value), "%lu / %lu", (unsigned long)s.bootCount, (unsigned long)s.unhealthyBoots);
  display_.drawString(value, 170, 74, 2);
  display_.drawNumber(s.containmentEntries, 170, 100, 2);
  display_.setTextColor(s.healthyMarked ? TFT_GREEN : TFT_LIGHTGREY, TFT_BLACK);
  display_.drawString(s.healthyMarked ? "SET" : "pending", 170, 126, 2);
  display_.setTextColor(s.archiveWritable ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
  display_.drawString(s.archiveWritable ? "WRITABLE" : "UNAVAILABLE", 170, 152, 2);
  display_.setTextColor(TFT_CYAN, TFT_BLACK);
  display_.drawNumber((uint32_t)s.archiveBytes, 170, 178, 2);
  display_.setTextColor(s.csIdle && s.ownerErrors == 0 ? TFT_GREEN : TFT_RED, TFT_BLACK);
  display_.drawString(s.csIdle && s.ownerErrors == 0 ? "PASS" : "FAIL", 170, 204, 2);
}

void ContainmentApp::printSerial(const Snapshot& s) {
  char line[320];
  snprintf(line, sizeof(line),
           "B5 profile=%s seq=%lu uptime_ms=%lu mode=%s boot_count=%lu unhealthy_boots=%lu "
           "containment_entries=%lu reset_reason=%lu healthy_marked=%u sd_present=%u "
           "archive_writable=%u archive_records=%lu archive_rotations=%lu archive_bytes=%llu "
           "cs_idle=%u owner=%s owner_errors=%lu",
           b1::PROFILE_ID, (unsigned long)s.sequence, (unsigned long)s.uptimeMs,
           modeText(s.mode), (unsigned long)s.bootCount, (unsigned long)s.unhealthyBoots,
           (unsigned long)s.containmentEntries, (unsigned long)s.resetReason,
           s.healthyMarked ? 1U : 0U, s.storagePresent ? 1U : 0U,
           s.archiveWritable ? 1U : 0U, (unsigned long)s.archiveRecords,
           (unsigned long)s.archiveRotations, (unsigned long long)s.archiveBytes,
           s.csIdle ? 1U : 0U, b2::ownerText(spi_.owner()), (unsigned long)s.ownerErrors);
  Serial.println(line);
  if (archiveReady_ && archive_.append(line)) {
    const auto& a = archive_.status();
    snapshot_.archiveRecords = a.records;
    snapshot_.archiveRotations = a.rotations;
    snapshot_.archiveBytes = a.bytes;
  }
}

const char* ContainmentApp::modeText(RunMode mode) {
  switch (mode) {
    case RunMode::Containment: return "containment";
    case RunMode::Recovery: return "recovery";
    default: return "normal";
  }
}

}  // namespace cyd::b5
