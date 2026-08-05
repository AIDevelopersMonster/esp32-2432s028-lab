#include <TFT_eSPI.h>
#include <CYD_Board.h>

TFT_eSPI tft;

void setup() {
  Serial.begin(115200);
  cyd::beginBasicHardware();

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("ESP32-2432S028R", 18, 20, 4);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("ILI9341 display test", 18, 62, 2);

  const uint16_t colors[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_CYAN, TFT_MAGENTA};
  for (int i = 0; i < 5; ++i) {
    tft.fillRect(18 + i * 58, 100, 48, 48, colors[i]);
  }

  tft.drawRect(12, 12, 296, 216, TFT_DARKGREY);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("Display initialized successfully", 18, 180, 2);
}

void loop() {
}
