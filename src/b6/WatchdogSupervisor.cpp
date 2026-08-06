#include <b6/WatchdogSupervisor.h>

#include <esp_task_wdt.h>
#include <esp_system.h>

namespace cyd::b6 {

bool WatchdogSupervisor::begin(uint32_t timeoutSeconds) {
  status_.timeoutSeconds = timeoutSeconds;
  const esp_err_t init = esp_task_wdt_init(timeoutSeconds, true);
  if (init != ESP_OK && init != ESP_ERR_INVALID_STATE) return false;
  const esp_err_t add = esp_task_wdt_add(nullptr);
  status_.enabled = add == ESP_OK || add == ESP_ERR_INVALID_STATE;
  return status_.enabled;
}

void WatchdogSupervisor::feed(uint32_t nowMs) {
  if (!status_.enabled || status_.faultExecuted) return;
  if (esp_task_wdt_reset() == ESP_OK) {
    ++status_.feeds;
    status_.lastFeedMs = nowMs;
  }
}

bool WatchdogSupervisor::arm(FaultMode mode) {
  if (mode == FaultMode::None || status_.armedFault != FaultMode::None || status_.faultExecuted) {
    return false;
  }
  status_.armedFault = mode;
  return true;
}

void WatchdogSupervisor::executeArmedFault() {
  if (status_.armedFault == FaultMode::None || status_.faultExecuted) return;
  status_.faultExecuted = true;
  switch (status_.armedFault) {
    case FaultMode::Restart:
      delay(50);
      ESP.restart();
      break;
    case FaultMode::Stall:
      while (true) delay(1000);
      break;
    case FaultMode::ArchiveFailure:
    case FaultMode::None:
      break;
  }
}

const char* WatchdogSupervisor::faultText(FaultMode mode) {
  switch (mode) {
    case FaultMode::Stall: return "stall";
    case FaultMode::Restart: return "restart";
    case FaultMode::ArchiveFailure: return "archive_failure";
    default: return "none";
  }
}

}  // namespace cyd::b6
