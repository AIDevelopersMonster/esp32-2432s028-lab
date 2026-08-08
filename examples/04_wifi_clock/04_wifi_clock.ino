/*
 * ESP32-2432S028R / Cheap Yellow Display (CYD)
 * ------------------------------------------------------------
 * Example: 04_wifi_clock
 * Purpose: connect the ESP32 to Wi-Fi, synchronize time through
 *          NTP and display the current date/time on the ILI9341 TFT.
 *
 * Project:
 *   https://github.com/AIDevelopersMonster/esp32-2432s028-lab
 *
 * Documentation:
 *   examples/04_wifi_clock/README.md
 *
 * Video confirmation:
 *   https://youtube.com/shorts/Ys7NKqPrfCg
 *
 * Status:
 *   VERIFIED WORKING on 2026-08-08
 *
 * Verified hardware profile:
 *   - Board: ESP32-2432S028R / CYD
 *   - MCU: ESP32-D0WD-V3 rev. 3.0
 *   - TFT controller: ILI9341
 *   - Display orientation: landscape, 320 x 240
 *   - Wi-Fi: 2.4 GHz
 *
 * Required components:
 *   - ESP32 Arduino Core
 *   - WiFi.h from ESP32 Arduino Core
 *   - time.h from ESP32 Arduino Core
 *   - TFT_eSPI by Bodmer
 *   - CYD_Board from this repository
 *   - local secrets.h with WIFI_SSID and WIFI_PASSWORD
 *
 * TFT_eSPI configuration:
 *   config/tft_espi/User_Setup.h
 *
 * Verified Arduino IDE settings:
 *   - Board: ESP32 Dev Module
 *   - CPU Frequency: 240 MHz
 *   - Flash Frequency: 40 MHz
 *   - Flash Mode: DIO
 *   - Flash Size: 4 MB
 *   - Upload Speed: 115200
 *
 * What this sketch checks:
 *   - CYD_Board basic hardware initialization;
 *   - ILI9341 initialization through TFT_eSPI;
 *   - Wi-Fi station mode;
 *   - connection to a 2.4 GHz access point;
 *   - NTP time setup using pool.ntp.org and time.nist.gov;
 *   - local time retrieval through getLocalTime();
 *   - date/time formatting with strftime();
 *   - periodic TFT updates while Wi-Fi remains active.
 *
 * Expected result:
 *   The display first shows "Connecting to Wi-Fi...".
 *   After successful connection it shows "Time synchronized" and
 *   then continuously displays HH:MM:SS and YYYY-MM-DD.
 *
 * Security note:
 *   Do not commit your real secrets.h to GitHub.
 */

#include <WiFi.h>       // ESP32 Wi-Fi support from Arduino Core.
#include <time.h>       // NTP/local time functions from the ESP32 toolchain.
#include <TFT_eSPI.h>   // ILI9341 display driver and graphics library.
#include <CYD_Board.h>  // Project-specific CYD initialization and pin map.
#include "secrets.h"    // Local Wi-Fi credentials; never publish real values.

// TFT_eSPI uses the project's config/tft_espi/User_Setup.h settings.
TFT_eSPI tft;

// Time zone offset from UTC in seconds.
// Current laboratory default is UTC. For UTC+3 use 3 * 3600.
constexpr long GMT_OFFSET_SECONDS = 0;

// Additional daylight-saving offset in seconds.
// This minimal example does not implement automatic DST rules.
constexpr int DAYLIGHT_OFFSET_SECONDS = 0;

// Draw a one-line status message in the lower part of the TFT.
// The rectangle is cleared first so a new status replaces the old one.
void drawStatus(const char *message, uint16_t color = TFT_WHITE) {
  tft.fillRect(10, 180, 300, 30, TFT_BLACK);
  tft.setTextColor(color, TFT_BLACK);
  tft.drawCentreString(message, 160, 184, 2);
}

void setup() {
  // Serial is kept enabled for possible diagnostics and future expansion.
  Serial.begin(115200);

  // Initialize common hardware for this CYD revision, including TFT backlight.
  cyd::beginBasicHardware();

  // Initialize the ILI9341 display.
  tft.init();

  // Rotation 1 is the verified 320 x 240 landscape orientation.
  tft.setRotation(1);

  // Draw the static screen header before starting Wi-Fi.
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawCentreString("CYD Wi-Fi clock", 160, 20, 4);

  // Put the ESP32 radio into station mode: the board connects to an existing
  // access point instead of creating its own AP.
  WiFi.mode(WIFI_STA);

  // Start connection using credentials from local secrets.h.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  drawStatus("Connecting to Wi-Fi...");

  // Wait for Wi-Fi, but stop waiting after about 20 seconds so a wrong SSID,
  // password or unavailable network cannot block setup() forever.
  const uint32_t deadline = millis() + 20000;
  while (WiFi.status() != WL_CONNECTED && millis() < deadline) {
    delay(250);
  }

  // If the connection did not complete by the deadline, leave a clear error
  // on the display. loop() will continue trying to read time, but without a
  // network/NTP result it will simply return after getLocalTime() fails.
  if (WiFi.status() != WL_CONNECTED) {
    drawStatus("Wi-Fi connection failed", TFT_RED);
    return;
  }

  // Configure the system SNTP/NTP client. These servers provide UTC; the
  // constants above apply the requested fixed time-zone/daylight offsets.
  configTime(GMT_OFFSET_SECONDS,
             DAYLIGHT_OFFSET_SECONDS,
             "pool.ntp.org",
             "time.nist.gov");

  // This message means Wi-Fi is connected and NTP configuration was started.
  // The actual time value is accepted in loop() only when getLocalTime()
  // successfully returns a valid structure.
  drawStatus("Time synchronized", TFT_GREEN);
}

void loop() {
  // Ask the ESP32 time subsystem for the current local time.
  // The 200 ms timeout keeps the UI loop responsive if time is temporarily
  // unavailable, for example immediately after startup.
  struct tm timeInfo {};
  if (!getLocalTime(&timeInfo, 200)) {
    delay(500);
    return;
  }

  // Convert the tm structure into human-readable strings.
  char timeText[16];
  char dateText[32];
  strftime(timeText, sizeof(timeText), "%H:%M:%S", &timeInfo);
  strftime(dateText, sizeof(dateText), "%Y-%m-%d", &timeInfo);

  // Clear only the changing date/time area rather than redrawing the complete
  // screen. This reduces unnecessary TFT traffic and keeps the static header.
  tft.fillRect(10, 70, 300, 95, TFT_BLACK);

  // Large cyan current time.
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawCentreString(timeText, 160, 72, 7);

  // Smaller white calendar date below the clock.
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawCentreString(dateText, 160, 142, 2);

  // Approximately four display refreshes per second. The clock itself is
  // maintained by the ESP32 time subsystem, so this delay only affects redraw.
  delay(250);
}
