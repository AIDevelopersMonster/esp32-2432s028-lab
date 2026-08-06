#include <b7/QualificationApp.h>

#include <SD.h>
#include <esp_system.h>

#include <CYD_Board.h>
#include <b1/BoardProfile.h>

namespace cyd::b7 {
namespace {
constexpr uint32_t RUN_INTERVAL_MS = 5000;
constexpr uint32_t HEALTHY_UPTIME_MS = 15000;
constexpr uint32_t SD_FREQUENCY_HZ = 10000000;
}

QualificationApp::QualificationApp(TFT_eSPI& display, b2::SpiCoordinator& spi)
    : display_(display), spi_(spi) {}

void QualificationApp::begin() {
  cyd::beginBasicHardware();
  spi_.begin();
  display_.init();
  display_.setRotation(1);
  resources_.begin();
  watchdog_.begin(8);
  ledger_.begin(static_cast<uint32_t>(esp_reset_reason()));
  snapshot_.bootCount = ledger_.record().bootCount;

  if (spi_.acquire(b2::SpiOwner::Storage)) {
    snapshot_.storagePresent = SD.begin(cyd::SD_CS_PIN, spi_.storageBus(), SD_FREQUENCY_HZ);
    digitalWrite(cyd::SD_CS_PIN, HIGH);
    spi_.release(b2::SpiOwner::Storage);
  }
  archiveReady_ = snapshot_.storagePresent && archive_.begin(SD);
  drawFrame();
  Serial.printf("B7 event=boot profile=%s boot_count=%lu soak_interval_ms=%lu\n",
                b1::PROFILE_ID, static_cast<unsigned long>(snapshot_.bootCount),
                static_cast<unsigned long>(RUN_INTERVAL_MS));
}

bool QualificationApp::due(uint32_t nowMs) const {
  return nowMs - lastRunMs_ >= RUN_INTERVAL_MS;
}

const SoakSnapshot& QualificationApp::run(uint32_t nowMs) {
  lastRunMs_ = nowMs;
  ++snapshot_.sequence;
  snapshot_.uptimeMs = nowMs;
  if (!snapshot_.healthyMarked && nowMs >= HEALTHY_UPTIME_MS) {
    snapshot_.healthyMarked = ledger_.markHealthy(nowMs);
  }
  watchdog_.feed(nowMs);
  snapshot_.watchdogFeeds = watchdog_.status().feeds;
  snapshot_.resources = resources_.sample();
  snapshot_.csIdle = digitalRead(cyd::TOUCH_CS_PIN) == HIGH && digitalRead(cyd::SD_CS_PIN) == HIGH;
  snapshot_.ownerErrors = spi_.counters().ownershipViolations;
  const auto& a = archive_.status();
  snapshot_.archiveWritable = archiveReady_ && a.writable;
  snapshot_.archiveRecords = a.records;
  snapshot_.archiveRotations = a.rotations;
  snapshot_.archiveBytes = a.bytes;
  spi_.noteDisplayTransaction();
  return snapshot_;
}

void QualificationApp::drawFrame() {
  display_.fillScreen(TFT_BLACK);
  display_.setTextDatum(TL_DATUM);
  display_.setTextColor(TFT_YELLOW, TFT_BLACK);
  display_.drawString("B7 LONG-RUN SOAK", 8, 6, 2);
  display_.setTextColor(TFT_WHITE, TFT_BLACK);
  display_.drawString("Uptime", 8, 46, 2);
  display_.drawString("Free / minimum heap", 8, 72, 2);
  display_.drawString("Largest block", 8, 98, 2);
  display_.drawString("Stack watermark", 8, 124, 2);
  display_.drawString("Watchdog feeds", 8, 150, 2);
  display_.drawString("Archive rec / rot", 8, 176, 2);
  display_.drawString("SPI invariant", 8, 202, 2);
}

void QualificationApp::render(const SoakSnapshot& s) {
  display_.fillRect(174, 42, 138, 190, TFT_BLACK);
  display_.setTextColor(TFT_CYAN, TFT_BLACK);
  char value[48];
  snprintf(value, sizeof(value), "%lus", static_cast<unsigned long>(s.uptimeMs / 1000U));
  display_.drawString(value, 174, 46, 2);
  snprintf(value, sizeof(value), "%lu/%lu", (unsigned long)s.resources.freeHeap,
           (unsigned long)s.resources.minFreeHeap);
  display_.drawString(value, 174, 72, 2);
  display_.drawNumber(s.resources.largestBlock, 174, 98, 2);
  display_.drawNumber(s.resources.loopStackHighWater, 174, 124, 2);
  display_.drawNumber(s.watchdogFeeds, 174, 150, 2);
  snprintf(value, sizeof(value), "%lu/%lu", (unsigned long)s.archiveRecords,
           (unsigned long)s.archiveRotations);
  display_.drawString(value, 174, 176, 2);
  display_.setTextColor(s.csIdle && s.ownerErrors == 0 ? TFT_GREEN : TFT_RED, TFT_BLACK);
  display_.drawString(s.csIdle && s.ownerErrors == 0 ? "PASS" : "FAIL", 174, 202, 2);
}

void QualificationApp::printSerial(const SoakSnapshot& s) {
  char line[420];
  snprintf(line, sizeof(line),
           "B7 profile=%s seq=%lu uptime_ms=%lu boot_count=%lu healthy_marked=%u "
           "free_heap=%lu min_free_heap=%lu largest_block=%lu stack_watermark=%lu "
           "resource_samples=%lu watchdog_feeds=%lu sd_present=%u archive_writable=%u "
           "archive_records=%lu archive_rotations=%lu archive_bytes=%llu cs_idle=%u "
           "owner=%s owner_errors=%lu",
           b1::PROFILE_ID, (unsigned long)s.sequence, (unsigned long)s.uptimeMs,
           (unsigned long)s.bootCount, s.healthyMarked ? 1U : 0U,
           (unsigned long)s.resources.freeHeap, (unsigned long)s.resources.minFreeHeap,
           (unsigned long)s.resources.largestBlock,
           (unsigned long)s.resources.loopStackHighWater,
           (unsigned long)s.resources.samples, (unsigned long)s.watchdogFeeds,
           s.storagePresent ? 1U : 0U, s.archiveWritable ? 1U : 0U,
           (unsigned long)s.archiveRecords, (unsigned long)s.archiveRotations,
           (unsigned long long)s.archiveBytes, s.csIdle ? 1U : 0U,
           b2::ownerText(spi_.owner()), (unsigned long)s.ownerErrors);
  Serial.println(line);
  if (archiveReady_ && archive_.append(line)) {
    const auto& a = archive_.status();
    snapshot_.archiveRecords = a.records;
    snapshot_.archiveRotations = a.rotations;
    snapshot_.archiveBytes = a.bytes;
  }
}

}  // namespace cyd::b7
