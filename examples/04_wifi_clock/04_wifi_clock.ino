#include <WiFi.h>
#include <time.h>
#include <TFT_eSPI.h>
#include <CYD_Board.h>
#include "secrets.h"

TFT_eSPI tft;

constexpr long GMT_OFFSET_SECONDS = 0;
constexpr int DAYLIGHT_OFFSET_SECONDS = 0;

void drawStatus(const char *message, uint16_t color = TFT_WHITE) {
  tft.fillRect(10, 180, 300, 30, TFT_BLACK);
  tft.setTextColor(color, TFT_BLACK);
  tft.drawCentreString(message, 160, 184, 2);
}

void setup() {
  Serial.begin(115200);
  cyd::beginBasicHardware();

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawCentreString("CYD Wi-Fi clock", 160, 20, 4);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  drawStatus("Connecting to Wi-Fi...");

  const uint32_t deadline = millis() + 20000;
  while (WiFi.status() != WL_CONNECTED && millis() < deadline) {
    delay(250);
  }

  if (WiFi.status() != WL_CONNECTED) {
    drawStatus("Wi-Fi connection failed", TFT_RED);
    return;
  }

  configTime(GMT_OFFSET_SECONDS, DAYLIGHT_OFFSET_SECONDS, "pool.ntp.org", "time.nist.gov");
  drawStatus("Time synchronized", TFT_GREEN);
}

void loop() {
  struct tm timeInfo {};
  if (!getLocalTime(&timeInfo, 200)) {
    delay(500);
    return;
  }

  char timeText[16];
  char dateText[32];
  strftime(timeText, sizeof(timeText), "%H:%M:%S", &timeInfo);
  strftime(dateText, sizeof(dateText), "%Y-%m-%d", &timeInfo);

  tft.fillRect(10, 70, 300, 95, TFT_BLACK);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawCentreString(timeText, 160, 72, 7);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawCentreString(dateText, 160, 142, 2);

  delay(250);
}
