/*
 * ESP32-2432S028R / Cheap Yellow Display (CYD)
 * ------------------------------------------------------------
 * Example: 07_audio_test
 *
 * Purpose:
 *   Conservative hardware test of the onboard audio path:
 *
 *     ESP32 GPIO26 / DAC2 -> onboard 8002A-family amplifier -> P4 -> speaker
 *
 * IMPORTANT:
 *   This version intentionally uses a VERY LOW DAC amplitude (+/-2 DAC codes)
 *   because higher audio levels caused USB/UART instability on the tested board.
 *
 *   TFT/backlight and CYD_Board initialization are intentionally NOT used here,
 *   so the test isolates the audio path as much as possible.
 *
 * Hardware:
 *   - Board: ESP32-2432S028R / CYD
 *   - Audio source: GPIO26 / DAC2
 *   - Speaker: 8 ohm, connected only to the board speaker connector P4
 *   - Do NOT connect either speaker lead to GND
 *
 * Arduino IDE profile:
 *   - Board: ESP32 Dev Module
 *   - CPU Frequency: 240 MHz
 *   - Flash Frequency: 40 MHz
 *   - Flash Mode: DIO
 *   - Flash Size: 4 MB
 *   - PSRAM: Disabled
 *   - Upload Speed: 115200
 *   - Serial Monitor: 115200
 *
 * Test sequence:
 *   1. 440 Hz, 500 ms
 *   2. 880 Hz, 500 ms
 *   3. 1760 Hz, 500 ms
 *   4. Audio stops
 *   5. ESP32 prints "ESP32 ALIVE" once per second
 *
 * Expected result:
 *   - Three different tones are audible.
 *   - After the tones, the ESP32 continues running.
 *   - millis() in ESP32 ALIVE messages must continue increasing.
 *
 * Project:
 *   https://github.com/AIDevelopersMonster/esp32-2432s028-lab
 */

#include <Arduino.h>

// -----------------------------------------------------------------------------
// Hardware configuration
// -----------------------------------------------------------------------------

constexpr uint8_t AUDIO_PIN = 26;       // ESP32 DAC2
constexpr uint8_t WAVE_SAMPLES = 32;

// Very conservative output level.
// Actual DAC output range during this test is approximately 126..130.
constexpr uint8_t AUDIO_AMPLITUDE = 2;

// Tone timing.
constexpr uint32_t TONE_DURATION_MS = 500;
constexpr uint32_t PAUSE_BETWEEN_TONES_MS = 1500;

// -----------------------------------------------------------------------------
// Normalized sine-like waveform.
//
// Values are approximately -16..+16.
// They are scaled at runtime by AUDIO_AMPLITUDE / 16.
//
// With AUDIO_AMPLITUDE = 2:
//   DAC output is approximately 128 +/- 2.
// -----------------------------------------------------------------------------

const int8_t SINE_SHAPE[WAVE_SAMPLES] = {
     0,  3,  6,  9, 11, 13, 15, 16,
    16, 16, 15, 13, 11,  9,  6,  3,
     0, -3, -6, -9,-11,-13,-15,-16,
   -16,-16,-15,-13,-11, -9, -6, -3
};

// -----------------------------------------------------------------------------
// Generate one low-level DAC tone.
//
// This is intentionally simple and blocking.
// It is a hardware bring-up test, not a production audio engine.
// -----------------------------------------------------------------------------

void playDacTone(uint16_t frequencyHz,
                 uint32_t durationMs,
                 uint8_t amplitudeCodes) {

  const uint32_t sampleRate =
      static_cast<uint32_t>(frequencyHz) * WAVE_SAMPLES;

  const uint32_t samplePeriodUs =
      1000000UL / sampleRate;

  const uint32_t totalSamples =
      (static_cast<uint64_t>(sampleRate) * durationMs) / 1000UL;

  Serial.printf(
      "START %u Hz amplitude=+/-%u\n",
      frequencyHz,
      amplitudeCodes
  );

  for (uint32_t i = 0; i < totalSamples; ++i) {

    const int16_t sample =
        128 +
        (static_cast<int16_t>(
             SINE_SHAPE[i % WAVE_SAMPLES]
         ) * amplitudeCodes) / 16;

    dacWrite(AUDIO_PIN, static_cast<uint8_t>(sample));
    delayMicroseconds(samplePeriodUs);
  }

  // Return DAC to midpoint after each tone.
  dacWrite(AUDIO_PIN, 128);

  Serial.printf(
      "END %u Hz amplitude=+/-%u\n",
      frequencyHz,
      amplitudeCodes
  );
}

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Keep DAC at midpoint before starting audio.
  dacWrite(AUDIO_PIN, 128);

  Serial.println();
  Serial.println("================================");
  Serial.println("CYD LOW-LEVEL AUDIO TEST");
  Serial.println("ESP32-2432S028R");
  Serial.println("GPIO26 / DAC2");
  Serial.printf("DAC amplitude: +/-%u codes\n", AUDIO_AMPLITUDE);
  Serial.println("TFT/backlight: NOT initialized");
  Serial.println("================================");

  delay(1000);

  playDacTone(
      440,
      TONE_DURATION_MS,
      AUDIO_AMPLITUDE
  );

  delay(PAUSE_BETWEEN_TONES_MS);

  playDacTone(
      880,
      TONE_DURATION_MS,
      AUDIO_AMPLITUDE
  );

  delay(PAUSE_BETWEEN_TONES_MS);

  playDacTone(
      1760,
      TONE_DURATION_MS,
      AUDIO_AMPLITUDE
  );

  // Return to quiet midpoint.
  dacWrite(AUDIO_PIN, 128);

  Serial.println("AUDIO TEST FINISHED");
  Serial.println("ESP32 should now remain running.");
}

// -----------------------------------------------------------------------------
// Main loop.
//
// No more audio is generated.
// This heartbeat lets us see whether the ESP32 itself remains alive even if
// the onboard USB/UART path becomes unstable.
// -----------------------------------------------------------------------------

void loop() {
  static uint32_t lastReportMs = 0;

  if (millis() - lastReportMs >= 1000) {
    lastReportMs = millis();

    Serial.printf(
        "ESP32 ALIVE: %lu ms\n",
        static_cast<unsigned long>(millis())
    );
  }
}