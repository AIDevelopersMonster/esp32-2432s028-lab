/*
 * ESP32-2432S028R / Cheap Yellow Display (CYD)
 * ------------------------------------------------------------
 * Example: 01_display_test
 * Purpose: basic functional test of the built-in ILI9341 TFT.
 *
 * Project:
 *   https://github.com/AIDevelopersMonster/esp32-2432s028-lab
 *
 * Status:
 *   VERIFIED WORKING on 2026-08-07
 *
 * Verified hardware profile:
 *   - Board: ESP32-2432S028R / CYD
 *   - MCU: ESP32-D0WD-V3 rev. 3.0
 *   - TFT controller: ILI9341
 *   - Display size: 240 x 320 pixels
 *   - Test orientation: landscape, 320 x 240
 *   - Flash: 4 MB
 *
 * Required libraries:
 *   - TFT_eSPI by Bodmer
 *   - CYD_Board from this repository
 *
 * Important:
 *   TFT_eSPI must use the project configuration:
 *     config/tft_espi/User_Setup.h
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
 *   - CYD_Board basic hardware initialization
 *   - TFT backlight control
 *   - ILI9341 initialization through TFT_eSPI
 *   - landscape orientation
 *   - text drawing
 *   - rectangle drawing
 *   - basic RGB/cyan/magenta color output
 *
 * Expected result:
 *   The display shows the board name, the text
 *   "ILI9341 display test", five colored fields,
 *   a frame and "Display initialized successfully".
 *
 * Documentation:
 *   examples/01_display_test/README.md
 *   docs/tft-espi-installation.md
 *   libraries/CYD_Board/README.md
 */

#include <TFT_eSPI.h>   // TFT driver and graphics library.
#include <CYD_Board.h>  // Project-specific CYD pin map and helper functions.

// TFT_eSPI object. Pin assignments and ILI9341 driver selection are taken
// from the project's config/tft_espi/User_Setup.h file.
TFT_eSPI tft;

void setup() {
  // Serial is not required for the visible display test, but keeping it
  // enabled at 115200 baud is useful for later diagnostics and extensions.
  Serial.begin(115200);

  // Initialize the basic CYD hardware defined by our CYD_Board library.
  // Among other things, this enables the TFT backlight on GPIO 21.
  cyd::beginBasicHardware();

  // Initialize the ILI9341 through TFT_eSPI.
  tft.init();

  // Rotation 1 gives the verified landscape orientation: 320 x 240.
  tft.setRotation(1);

  // Clear the complete screen before drawing the test pattern.
  tft.fillScreen(TFT_BLACK);

  // Board name in yellow.
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("ESP32-2432S028R", 18, 20, 4);

  // Test description in white.
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("ILI9341 display test", 18, 62, 2);

  // Draw five basic color fields. This is a practical visual check that the
  // display accepts pixel data and that the main color channels are visible.
  // A photograph is not a colorimetric reference: camera exposure and white
  // balance can noticeably change how these colors look in a photo.
  const uint16_t colors[] = {
    TFT_RED,
    TFT_GREEN,
    TFT_BLUE,
    TFT_CYAN,
    TFT_MAGENTA
  };

  for (int i = 0; i < 5; ++i) {
    tft.fillRect(18 + i * 58, 100, 48, 48, colors[i]);
  }

  // Draw a frame around the useful test area.
  tft.drawRect(12, 12, 296, 216, TFT_DARKGREY);

  // Final visible indication that setup() reached the end successfully.
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("Display initialized successfully", 18, 180, 2);
}

void loop() {
  // Static display test: no periodic work is required.
}
