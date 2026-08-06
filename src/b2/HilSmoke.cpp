#include <b2/HilSmoke.h>

#include <SD.h>

#include <CYD_Board.h>
#include <b1/BoardProfile.h>

namespace cyd::b2 {
namespace {
constexpr uint32_t RUN_INTERVAL_MS = 750;
constexpr uint32_t SD_FREQUENCY_HZ = 10000000;

const char* passFail(bool value) {
  return value ? "PASS" : "FAIL";
}
}  // namespace

HilSmoke::HilSmoke(TFT_eSPI& display, SpiCoordinator& spi) : display_(display), spi_(spi) {}

void HilSmoke::begin() {
  cyd::beginBasicHardware();
  spi_.begin();

  display_.init();
  display_.setRotation(1);
  drawStaticUi();
  snapshot_.displayOk = true;

  probeStorage();
  Serial.println("B2 event=boot profile=esp32-2432s028r-sample-a topology=tft_hspi,touch_softspi,sd_vspi");
}

bool HilSmoke::due(uint32_t nowMs) const {
  return nowMs - lastRunMs_ >= RUN_INTERVAL_MS;
}

const HilSnapshot& HilSmoke::run(uint32_t nowMs) {
  lastRunMs_ = nowMs;
  ++snapshot_.sequence;
  snapshot_.uptimeMs = nowMs;

  snapshot_.touchSampleOk = spi_.readTouchRaw(
      snapshot_.touchX, snapshot_.touchY, snapshot_.touchZ);
  snapshot_.touchPressed = digitalRead(cyd::TOUCH_IRQ_PIN) == LOW;

  snapshot_.chipSelectIdle =
      digitalRead(cyd::TOUCH_CS_PIN) == HIGH && digitalRead(cyd::SD_CS_PIN) == HIGH;
  snapshot_.ownershipViolations = spi_.counters().ownershipViolations;

  spi_.noteDisplayTransaction();
  return snapshot_;
}

void HilSmoke::probeStorage() {
  if (!spi_.acquire(SpiOwner::Storage)) {
    snapshot_.storagePresent = false;
    return;
  }

  snapshot_.storagePresent = SD.begin(cyd::SD_CS_PIN, spi_.storageBus(), SD_FREQUENCY_HZ);
  if (snapshot_.storagePresent) {
    snapshot_.storageBytes = SD.cardSize();
  }
  digitalWrite(cyd::SD_CS_PIN, HIGH);
  spi_.release(SpiOwner::Storage);
}

void HilSmoke::drawStaticUi() {
  display_.fillScreen(TFT_BLACK);
  display_.setTextDatum(TL_DATUM);
  display_.setTextColor(TFT_YELLOW, TFT_BLACK);
  display_.drawString("B2 UNIFIED SPI + HIL", 10, 8, 2);
  display_.setTextColor(TFT_WHITE, TFT_BLACK);
  display_.drawString(b1::PROFILE_ID, 10, 28, 2);
  display_.drawFastHLine(10, 48, 300, TFT_DARKGREY);
  display_.drawString("Display", 10, 62, 2);
  display_.drawString("Touch sample", 10, 88, 2);
  display_.drawString("Touch raw", 10, 114, 2);
  display_.drawString("microSD", 10, 140, 2);
  display_.drawString("CS idle", 10, 166, 2);
  display_.drawString("Owner errors", 10, 192, 2);
  display_.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  display_.drawString("Serial prefix: B2", 10, 220, 2);
}

void HilSmoke::render(const HilSnapshot& snapshot) {
  display_.fillRect(145, 58, 165, 158, TFT_BLACK);

  display_.setTextColor(snapshot.displayOk ? TFT_GREEN : TFT_RED, TFT_BLACK);
  display_.drawString(passFail(snapshot.displayOk), 145, 62, 2);

  display_.setTextColor(snapshot.touchSampleOk ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
  display_.drawString(snapshot.touchPressed ? "PRESSED" : "idle", 145, 88, 2);

  display_.setTextColor(TFT_CYAN, TFT_BLACK);
  char touchText[40];
  snprintf(touchText, sizeof(touchText), "%u,%u z=%u", snapshot.touchX, snapshot.touchY,
           snapshot.touchZ);
  display_.drawString(touchText, 145, 114, 2);

  display_.setTextColor(snapshot.storagePresent ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
  display_.drawString(snapshot.storagePresent ? "PRESENT" : "ABSENT", 145, 140, 2);

  display_.setTextColor(snapshot.chipSelectIdle ? TFT_GREEN : TFT_RED, TFT_BLACK);
  display_.drawString(passFail(snapshot.chipSelectIdle), 145, 166, 2);

  display_.setTextColor(snapshot.ownershipViolations == 0 ? TFT_GREEN : TFT_RED, TFT_BLACK);
  display_.drawNumber(snapshot.ownershipViolations, 145, 192, 2);
}

void HilSmoke::printSerial(const HilSnapshot& snapshot) const {
  const auto& counters = spi_.counters();
  Serial.printf(
      "B2 profile=%s seq=%lu uptime_ms=%lu display=%s touch_sample=%s touch_pressed=%u "
      "touch_x=%u touch_y=%u touch_z=%u sd_present=%u sd_bytes=%llu cs_idle=%u "
      "owner=%s owner_errors=%lu tx_display=%lu tx_touch=%lu tx_storage=%lu\n",
      b1::PROFILE_ID,
      static_cast<unsigned long>(snapshot.sequence),
      static_cast<unsigned long>(snapshot.uptimeMs),
      passFail(snapshot.displayOk),
      snapshot.touchSampleOk ? "PASS" : "IDLE",
      snapshot.touchPressed ? 1U : 0U,
      snapshot.touchX,
      snapshot.touchY,
      snapshot.touchZ,
      snapshot.storagePresent ? 1U : 0U,
      static_cast<unsigned long long>(snapshot.storageBytes),
      snapshot.chipSelectIdle ? 1U : 0U,
      ownerText(spi_.owner()),
      static_cast<unsigned long>(snapshot.ownershipViolations),
      static_cast<unsigned long>(counters.displayTransactions),
      static_cast<unsigned long>(counters.touchTransactions),
      static_cast<unsigned long>(counters.storageTransactions));
}

}  // namespace cyd::b2
