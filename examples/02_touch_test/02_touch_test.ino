/*
 * ESP32-2432S028R / Cheap Yellow Display (CYD)
 * ------------------------------------------------------------
 * Example: 02_touch_test
 * Purpose: basic RAW test of the built-in XPT2046 resistive
 *          touchscreen while the ILI9341 TFT remains active.
 *
 * Project:
 *   https://github.com/AIDevelopersMonster/esp32-2432s028-lab
 *
 * Example documentation:
 *   examples/02_touch_test/README.md
 *
 * XPT2046 installation guide:
 *   docs/xpt2046-touchscreen-installation.md
 *
 * Video confirmation:
 *   https://youtube.com/shorts/DmOkwDvkmLM
 *
 * Status:
 *   VERIFIED WORKING on 2026-08-07
 *   Basic RAW touch communication confirmed on real hardware.
 *
 * Verified hardware profile:
 *   - Board: ESP32-2432S028R / CYD
 *   - MCU: ESP32-D0WD-V3 rev. 3.0
 *   - TFT controller: ILI9341
 *   - Touch controller: XPT2046
 *   - Display test orientation: landscape, 320 x 240
 *   - Flash: 4 MB
 *
 * Required libraries:
 *   - TFT_eSPI by Bodmer
 *   - XPT2046_Touchscreen by Paul Stoffregen
 *   - CYD_Board from this repository
 *
 * Important TFT configuration:
 *   TFT_eSPI must use the project setup file:
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
 * XPT2046 pin map used by CYD_Board:
 *   - IRQ  : GPIO 36
 *   - MOSI : GPIO 32
 *   - MISO : GPIO 39
 *   - CLK  : GPIO 25
 *   - CS   : GPIO 33
 *
 * The touch controller uses its own SPIClass instance. These pins are
 * different from the SPI pins used by the TFT display.
 *
 * What this sketch checks:
 *   - CYD_Board basic hardware initialization
 *   - TFT initialization and stable simultaneous operation
 *   - XPT2046 communication over the dedicated touch SPI bus
 *   - touch detection
 *   - RAW X coordinate reading
 *   - RAW Y coordinate reading
 *   - RAW Z pressure/contact reading
 *   - output of X/Y/Z to both TFT and Serial Monitor
 *
 * Expected result:
 *   The TFT shows "XPT2046 raw touch test". When the screen is pressed,
 *   RAW X/Y/Z values are updated on the display and printed to Serial.
 *
 * Serial Monitor:
 *   115200 baud
 *
 * Important limitation:
 *   This is a RAW test only. X/Y are NOT calibrated screen coordinates.
 *   The sketch does not perform axis correction, filtering, calibration,
 *   or conversion to pixel coordinates 0..319 / 0..239.
 */

#include <SPI.h>                   // ESP32 SPI interface support.
#include <TFT_eSPI.h>              // ILI9341 display driver and graphics.
#include <XPT2046_Touchscreen.h>   // Resistive touchscreen controller.
#include <CYD_Board.h>             // Project-specific CYD pin map/helpers.

// TFT_eSPI uses the project's config/tft_espi/User_Setup.h configuration.
TFT_eSPI tft;

// Create a separate hardware SPI instance for the XPT2046 touch controller.
// The CYD touch lines are different from the TFT SPI lines.
SPIClass touchSpi(VSPI);

// XPT2046 object. Chip-select and interrupt pins come from CYD_Board.
XPT2046_Touchscreen touch(cyd::TOUCH_CS_PIN, cyd::TOUCH_IRQ_PIN);

void setup() {
  // Serial output is used to observe the same RAW X/Y/Z values that are
  // displayed on the TFT.
  Serial.begin(115200);

  // Initialize common CYD hardware. This also enables the TFT backlight.
  cyd::beginBasicHardware();

  // Initialize the ILI9341 display using TFT_eSPI.
  tft.init();

  // Rotation 1 is the verified landscape orientation: 320 x 240.
  tft.setRotation(1);

  // Prepare a simple static interface for the touch test.
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("XPT2046 raw touch test", 12, 12, 2);

  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString("Touch the screen and watch X/Y/Z", 12, 36, 2);

  // Start the dedicated touch SPI bus using the pin map defined in CYD_Board:
  // CLK=25, MISO=39, MOSI=32, CS=33.
  touchSpi.begin(cyd::TOUCH_CLK_PIN,
                 cyd::TOUCH_MISO_PIN,
                 cyd::TOUCH_MOSI_PIN,
                 cyd::TOUCH_CS_PIN);

  // Attach XPT2046_Touchscreen to our dedicated SPI instance.
  touch.begin(touchSpi);

  // Keep the touch library orientation aligned with the TFT test orientation.
  // RAW coordinates can still be inverted or swapped; calibration is a later
  // step and is intentionally not performed in this example.
  touch.setRotation(1);
}

void loop() {
  // Do nothing until the XPT2046 reports an active touch.
  if (!touch.touched()) {
    delay(20);
    return;
  }

  // Read one RAW touch sample.
  // point.x and point.y are uncalibrated controller values.
  // point.z is a relative pressure/contact value, not a physical force unit.
  const TS_Point point = touch.getPoint();

  // Send the RAW sample to Serial Monitor for diagnostics and recording.
  Serial.printf("x=%d y=%d z=%d\n", point.x, point.y, point.z);

  // Clear only the previous numeric line instead of redrawing the whole TFT.
  // This keeps the display stable while repeatedly reading the touch panel.
  tft.fillRect(12, 75, 296, 40, TFT_BLACK);

  // Show the latest RAW touch sample directly on the display.
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(12, 78, 4);
  tft.printf("X:%4d Y:%4d Z:%4d", point.x, point.y, point.z);

  // Small delay limits update frequency and makes the numbers easier to read.
  delay(60);
}
