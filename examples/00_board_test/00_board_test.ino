#include <CYD_Board.h>

void setup() {
  Serial.begin(115200);
  delay(300);
  cyd::beginBasicHardware();
  analogReadResolution(12);
  Serial.println("CYD basic board test");
}

void loop() {
  static uint8_t step = 0;

  switch (step++ % 4) {
    case 0: cyd::setRgb(true, false, false); break;
    case 1: cyd::setRgb(false, true, false); break;
    case 2: cyd::setRgb(false, false, true); break;
    default: cyd::rgbOff(); break;
  }

  Serial.printf("LDR=%u, BOOT=%s\n",
                cyd::readLightRaw(),
                digitalRead(cyd::BOOT_BUTTON_PIN) == LOW ? "pressed" : "released");
  delay(700);
}
