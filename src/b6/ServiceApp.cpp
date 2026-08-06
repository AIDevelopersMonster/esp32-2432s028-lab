#include <b6/ServiceApp.h>

#include <SD.h>
#include <esp_system.h>

#include <CYD_Board.h>
#include <b1/BoardProfile.h>

namespace cyd::b6 {
namespace {
constexpr uint32_t RUN_INTERVAL_MS = 1000;
constexpr uint32_t HEALTHY_UPTIME_MS = 15000;
constexpr uint32_t CRASH_LOOP_THRESHOLD = 3;
constexpr uint32_t WATCHDOG_TIMEOUT_SECONDS = 8;
constexpr uint32_t SD_FREQUENCY_HZ = 10000000;
}

ServiceApp::ServiceApp(TFT_eSPI& display, b2::SpiCoordinator& spi)
    : display_(display), spi_(spi) {}

void ServiceApp::begin() {
  cyd::beginBasicHardware();
  spi_.begin();
  display_.init();
  display_.setRotation(1);

  ledger_.begin(static_cast<uint32_t>(esp_reset_reason()));
  const auto& ledger = ledger_.record();
  snapshot_.bootCount = ledger.bootCount;
  snapshot_.unhealthyBoots = ledger.consecutiveUnhealthyBoots;
  if (digitalRead(cyd::BOOT_BUTTON_PIN) == LOW) snapshot_.mode = ServiceMode::Recovery;
  else if (ledger.consecutiveUnhealthyBoots >= CRASH_LOOP_THRESHOLD) snapshot_.mode = ServiceMode::Containment;

  snapshot_.watchdogEnabled = watchdog_.begin(WATCHDOG_TIMEOUT_SECONDS);

  if (spi_.acquire(b2::SpiOwner::Storage)) {
    snapshot_.storagePresent = SD.begin(cyd::SD_CS_PIN, spi_.storageBus(), SD_FREQUENCY_HZ);
    digitalWrite(cyd::SD_CS_PIN, HIGH);
    spi_.release(b2::SpiOwner::Storage);
  }
  if (snapshot_.storagePresent) {
    archiveReady_ = archive_.begin(SD);
    exporter_.begin(SD);
  }

  drawFrame();
  Serial.printf("B6 event=boot profile=%s mode=%s watchdog=%u timeout_s=%lu commands=%s\n",
                b1::PROFILE_ID, modeText(snapshot_.mode), snapshot_.watchdogEnabled ? 1U : 0U,
                static_cast<unsigned long>(WATCHDOG_TIMEOUT_SECONDS),
                snapshot_.mode == ServiceMode::Recovery ? "enabled" : "locked");
}

bool ServiceApp::due(uint32_t nowMs) const {
  return nowMs - lastRunMs_ >= RUN_INTERVAL_MS;
}

const ServiceSnapshot& ServiceApp::run(uint32_t nowMs) {
  lastRunMs_ = nowMs;
  ++snapshot_.sequence;
  snapshot_.uptimeMs = nowMs;
  pollCommands();

  if (!snapshot_.healthyMarked && snapshot_.mode == ServiceMode::Normal && nowMs >= HEALTHY_UPTIME_MS) {
    snapshot_.healthyMarked = ledger_.markHealthy(nowMs);
    snapshot_.unhealthyBoots = ledger_.record().consecutiveUnhealthyBoots;
  }

  if (exportRequested_) createExport();

  snapshot_.csIdle = digitalRead(cyd::TOUCH_CS_PIN) == HIGH && digitalRead(cyd::SD_CS_PIN) == HIGH;
  snapshot_.ownerErrors = spi_.counters().ownershipViolations;
  snapshot_.archiveWritable = archiveReady_ && archive_.status().writable && !archiveFailureInjected_;
  snapshot_.watchdogFeeds = watchdog_.status().feeds;
  snapshot_.fault = watchdog_.status().armedFault;
  const auto& e = exporter_.status();
  snapshot_.exportOk = e.lastExportOk;
  snapshot_.exports = e.exports;
  snapshot_.exportFiles = e.filesCopied;
  snapshot_.exportBytes = e.bytesCopied;
  spi_.noteDisplayTransaction();
  watchdog_.feed(nowMs);
  snapshot_.watchdogFeeds = watchdog_.status().feeds;

  if (watchdog_.status().armedFault != FaultMode::None) watchdog_.executeArmedFault();
  return snapshot_;
}

void ServiceApp::pollCommands() {
  while (Serial.available()) {
    const char ch = static_cast<char>(Serial.read());
    if (ch == '\r') continue;
    if (ch == '\n') {
      command_.trim();
      if (command_.length()) handleCommand(command_);
      command_ = "";
    } else if (command_.length() < 80) {
      command_ += ch;
    }
  }
}

void ServiceApp::handleCommand(const String& command) {
  if (snapshot_.mode != ServiceMode::Recovery) {
    Serial.println("B6 event=command_rejected reason=recovery_mode_required");
    return;
  }
  if (command == "B6 EXPORT") {
    exportRequested_ = true;
    Serial.println("B6 event=export_armed");
  } else if (command == "B6 ARM STALL") {
    Serial.printf("B6 event=fault_arm result=%s fault=stall\n", watchdog_.arm(FaultMode::Stall) ? "accepted" : "rejected");
  } else if (command == "B6 ARM RESTART") {
    Serial.printf("B6 event=fault_arm result=%s fault=restart\n", watchdog_.arm(FaultMode::Restart) ? "accepted" : "rejected");
  } else if (command == "B6 ARM ARCHIVE_FAILURE") {
    archiveFailureInjected_ = true;
    Serial.println("B6 event=fault_arm result=accepted fault=archive_failure");
  } else {
    Serial.println("B6 event=command_rejected reason=unknown_command");
  }
}

void ServiceApp::createExport() {
  exportRequested_ = false;
  char manifest[320];
  snprintf(manifest, sizeof(manifest),
           "profile=%s firmware=B6 boot_count=%lu unhealthy_boots=%lu mode=%s uptime_ms=%lu reset_reason=%lu",
           b1::PROFILE_ID, static_cast<unsigned long>(snapshot_.bootCount),
           static_cast<unsigned long>(snapshot_.unhealthyBoots), modeText(snapshot_.mode),
           static_cast<unsigned long>(snapshot_.uptimeMs),
           static_cast<unsigned long>(esp_reset_reason()));
  const bool ok = snapshot_.storagePresent && !archiveFailureInjected_ && exporter_.createBundle(manifest);
  Serial.printf("B6 event=service_export result=%s\n", ok ? "PASS" : "FAIL");
}

void ServiceApp::drawFrame() {
  display_.fillScreen(TFT_BLACK);
  display_.setTextDatum(TL_DATUM);
  display_.setTextColor(TFT_YELLOW, TFT_BLACK);
  display_.drawString("B6 WATCHDOG + SERVICE", 8, 6, 2);
  display_.setTextColor(TFT_WHITE, TFT_BLACK);
  display_.drawString("Mode", 8, 48, 2);
  display_.drawString("Watchdog", 8, 74, 2);
  display_.drawString("Feeds", 8, 100, 2);
  display_.drawString("Armed fault", 8, 126, 2);
  display_.drawString("Archive", 8, 152, 2);
  display_.drawString("Service export", 8, 178, 2);
  display_.drawString("SPI invariant", 8, 204, 2);
}

void ServiceApp::render(const ServiceSnapshot& s) {
  display_.fillRect(170, 44, 142, 188, TFT_BLACK);
  display_.setTextColor(s.mode == ServiceMode::Normal ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
  display_.drawString(modeText(s.mode), 170, 48, 2);
  display_.setTextColor(s.watchdogEnabled ? TFT_GREEN : TFT_RED, TFT_BLACK);
  display_.drawString(s.watchdogEnabled ? "ENABLED" : "FAILED", 170, 74, 2);
  display_.setTextColor(TFT_CYAN, TFT_BLACK);
  display_.drawNumber(s.watchdogFeeds, 170, 100, 2);
  display_.setTextColor(s.fault == FaultMode::None ? TFT_LIGHTGREY : TFT_ORANGE, TFT_BLACK);
  display_.drawString(WatchdogSupervisor::faultText(s.fault), 170, 126, 2);
  display_.setTextColor(s.archiveWritable ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
  display_.drawString(s.archiveWritable ? "WRITABLE" : "UNAVAILABLE", 170, 152, 2);
  display_.setTextColor(s.exportOk ? TFT_GREEN : TFT_LIGHTGREY, TFT_BLACK);
  display_.drawString(s.exportOk ? "EXPORTED" : "idle", 170, 178, 2);
  display_.setTextColor(s.csIdle && s.ownerErrors == 0 ? TFT_GREEN : TFT_RED, TFT_BLACK);
  display_.drawString(s.csIdle && s.ownerErrors == 0 ? "PASS" : "FAIL", 170, 204, 2);
}

void ServiceApp::printSerial(const ServiceSnapshot& s) {
  char line[384];
  snprintf(line, sizeof(line),
           "B6 profile=%s seq=%lu uptime_ms=%lu mode=%s boot_count=%lu unhealthy_boots=%lu "
           "healthy_marked=%u watchdog_enabled=%u watchdog_feeds=%lu fault=%s fault_executed=%u "
           "sd_present=%u archive_writable=%u export_ok=%u exports=%lu export_files=%lu export_bytes=%llu "
           "cs_idle=%u owner=%s owner_errors=%lu",
           b1::PROFILE_ID, static_cast<unsigned long>(s.sequence), static_cast<unsigned long>(s.uptimeMs),
           modeText(s.mode), static_cast<unsigned long>(s.bootCount),
           static_cast<unsigned long>(s.unhealthyBoots), s.healthyMarked ? 1U : 0U,
           s.watchdogEnabled ? 1U : 0U, static_cast<unsigned long>(s.watchdogFeeds),
           WatchdogSupervisor::faultText(s.fault), watchdog_.status().faultExecuted ? 1U : 0U,
           s.storagePresent ? 1U : 0U, s.archiveWritable ? 1U : 0U, s.exportOk ? 1U : 0U,
           static_cast<unsigned long>(s.exports), static_cast<unsigned long>(s.exportFiles),
           static_cast<unsigned long long>(s.exportBytes), s.csIdle ? 1U : 0U,
           b2::ownerText(spi_.owner()), static_cast<unsigned long>(s.ownerErrors));
  Serial.println(line);
  if (archiveReady_ && !archiveFailureInjected_) archive_.append(line);
}

const char* ServiceApp::modeText(ServiceMode mode) {
  switch (mode) {
    case ServiceMode::Containment: return "containment";
    case ServiceMode::Recovery: return "recovery";
    default: return "normal";
  }
}

}  // namespace cyd::b6
