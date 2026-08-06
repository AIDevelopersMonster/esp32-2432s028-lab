#include <b2/SpiCoordinator.h>

#include <CYD_Board.h>

namespace cyd::b2 {

void SpiCoordinator::begin() {
  pinMode(cyd::TOUCH_CS_PIN, OUTPUT);
  digitalWrite(cyd::TOUCH_CS_PIN, HIGH);
  pinMode(cyd::TOUCH_CLK_PIN, OUTPUT);
  digitalWrite(cyd::TOUCH_CLK_PIN, LOW);
  pinMode(cyd::TOUCH_MOSI_PIN, OUTPUT);
  pinMode(cyd::TOUCH_MISO_PIN, INPUT);
  pinMode(cyd::TOUCH_IRQ_PIN, INPUT);

  pinMode(cyd::SD_CS_PIN, OUTPUT);
  digitalWrite(cyd::SD_CS_PIN, HIGH);
  storageBus_.begin(cyd::SD_SCLK_PIN, cyd::SD_MISO_PIN, cyd::SD_MOSI_PIN, cyd::SD_CS_PIN);
}

bool SpiCoordinator::acquire(SpiOwner owner) {
  if (owner_ != SpiOwner::None) {
    ++counters_.ownershipViolations;
    return false;
  }
  owner_ = owner;
  switch (owner) {
    case SpiOwner::Display: ++counters_.displayTransactions; break;
    case SpiOwner::Touch: ++counters_.touchTransactions; break;
    case SpiOwner::Storage: ++counters_.storageTransactions; break;
    default: break;
  }
  return true;
}

void SpiCoordinator::release(SpiOwner owner) {
  if (owner_ != owner) {
    ++counters_.ownershipViolations;
    owner_ = SpiOwner::None;
    return;
  }
  owner_ = SpiOwner::None;
}

void SpiCoordinator::noteDisplayTransaction() {
  if (acquire(SpiOwner::Display)) {
    release(SpiOwner::Display);
  }
}

uint8_t SpiCoordinator::touchTransfer8(uint8_t value) {
  uint8_t received = 0;
  for (int bit = 7; bit >= 0; --bit) {
    digitalWrite(cyd::TOUCH_MOSI_PIN, (value >> bit) & 1U);
    digitalWrite(cyd::TOUCH_CLK_PIN, HIGH);
    received = static_cast<uint8_t>((received << 1U) | digitalRead(cyd::TOUCH_MISO_PIN));
    digitalWrite(cyd::TOUCH_CLK_PIN, LOW);
  }
  return received;
}

uint16_t SpiCoordinator::touchTransfer12(uint8_t command) {
  touchTransfer8(command);
  const uint8_t high = touchTransfer8(0x00);
  const uint8_t low = touchTransfer8(0x00);
  return static_cast<uint16_t>(((static_cast<uint16_t>(high) << 8U) | low) >> 3U) & 0x0FFFU;
}

bool SpiCoordinator::readTouchRaw(uint16_t& x, uint16_t& y, uint16_t& z) {
  if (!acquire(SpiOwner::Touch)) {
    return false;
  }
  digitalWrite(cyd::TOUCH_CS_PIN, LOW);
  const uint16_t z1 = touchTransfer12(0xB1);
  const uint16_t z2 = touchTransfer12(0xC1);
  x = touchTransfer12(0xD1);
  y = touchTransfer12(0x91);
  digitalWrite(cyd::TOUCH_CS_PIN, HIGH);
  release(SpiOwner::Touch);

  z = z1 > z2 ? z1 : static_cast<uint16_t>(z1 + 4095U - z2);
  return digitalRead(cyd::TOUCH_IRQ_PIN) == LOW || z > 100U;
}

const char* ownerText(SpiOwner owner) {
  switch (owner) {
    case SpiOwner::Display: return "display";
    case SpiOwner::Touch: return "touch";
    case SpiOwner::Storage: return "storage";
    default: return "none";
  }
}

}  // namespace cyd::b2
