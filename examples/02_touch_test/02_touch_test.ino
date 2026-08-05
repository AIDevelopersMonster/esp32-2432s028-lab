#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <CYD_Board.h>

TFT_eSPI tft;
SPIClass touchSpi(VSPI);
XPT2046_Touchscreen touch(cyd::TOUCH_CS_PIN, cyd::TOUCH_IRQ_PIN);

void setup() {
  Serial.begin(115200);
  cyd::beginBasicHardware();

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("XPT2046 raw touch test", 12, 12, 2);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString("Touch the screen and watch X/Y/Z", 12, 36, 2);

  touchSpi.begin(cyd::TOUCH_CLK_PIN,
                 cyd::TOUCH_MISO_PIN,
                 cyd::TOUCH_MOSI_PIN,
                 cyd::TOUCH_CS_PIN);
  touch.begin(touchSpi);
  touch.setRotation(1);
}

void loop() {
  if (!touch.touched()) {
    delay(20);
    return;
  }

  const TS_Point point = touch.getPoint();
  Serial.printf("x=%d y=%d z=%d\n", point.x, point.y, point.z);

  tft.fillRect(12, 75, 296, 40, TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(12, 78, 4);
  tft.printf("X:%4d Y:%4d Z:%4d", point.x, point.y, point.z);

  delay(60);
}
