/*
 * ESP32-2432S028R / Cheap Yellow Display (CYD)
 * ------------------------------------------------------------
 * Example: 07_audio_test
 * Purpose: verify the board audio path driven from ESP32 DAC2 / GPIO26.
 *
 * Project:
 *   https://github.com/AIDevelopersMonster/esp32-2432s028-lab
 *
 * Documentation:
 *   examples/07_audio_test/README.md
 *
 * Status:
 *   READY FOR HARDWARE TEST
 *   Do not mark VERIFIED until the real board audio output is heard or
 *   otherwise measured on the actual ESP32-2432S028R.
 *
 * Hardware profile:
 *   - Board: ESP32-2432S028R / CYD
 *   - Audio signal pin in this project: GPIO26
 *   - ESP32 GPIO26 is DAC2 on the original ESP32
 *   - No external Arduino library is required
 *
 * Test sequence:
 *   1. Silence / center level
 *   2. 440 Hz sine-like DAC waveform
 *   3. 880 Hz sine-like DAC waveform
 *   4. 1760 Hz sine-like DAC waveform
 *   5. Short silence, then repeat
 *
 * Expected result:
 *   Three clearly different tones should be audible from the board audio
 *   output/speaker path if the tested board revision contains the expected
 *   audio hardware and a suitable speaker/load is connected.
 *
 * Serial Monitor:
 *   115200 baud
 *
 * Important safety / scope notes:
 *   - Do not connect GPIO26 directly to a low-impedance speaker unless the
 *     board audio circuit is known to provide the required driver/amplifier.
 *   - Use the board's intended audio connector/path for the tested revision.
 *   - This sketch verifies signal generation and the board audio path; it is
 *     not an audio fidelity, power, THD or calibrated amplitude test.
 */

#include <Arduino.h>
#include <CYD_Board.h>

// Number of DAC samples used for one waveform period.
constexpr uint8_t WAVE_SAMPLES = 32;

// Centered 8-bit sine-like table, intentionally kept away from 0 and 255
// to reduce the chance of large full-scale steps in the simple lab test.
const uint8_t SINE_TABLE[WAVE_SAMPLES] = {
    128, 146, 163, 179, 194, 207, 217, 225,
    230, 232, 230, 225, 217, 207, 194, 179,
    128, 110,  93,  77,  62,  49,  39,  31,
     26,  24,  26,  31,  39,  49,  62,  77};

// Generate an approximate sine tone by stepping the ESP32 8-bit DAC.
// Timing is intentionally simple and blocking: this is a hardware test,
// not a production audio engine.
void playDacTone(uint16_t frequencyHz, uint32_t durationMs) {
  const uint32_t sampleRate = static_cast<uint32_t>(frequencyHz) * WAVE_SAMPLES;
  const uint32_t samplePeriodUs = 1000000UL / sampleRate;
  const uint32_t totalSamples = (static_cast<uint64_t>(sampleRate) * durationMs) / 1000UL;

  Serial.printf("AUDIO_TONE_START frequency=%uHz duration=%lums\n",
                frequencyHz,
                static_cast<unsigned long>(durationMs));

  for (uint32_t i = 0; i < totalSamples; ++i) {
    dacWrite(cyd::AUDIO_PIN, SINE_TABLE[i % WAVE_SAMPLES]);
    delayMicroseconds(samplePeriodUs);
  }

  // Return to the middle DAC code between tones to avoid a large DC step.
  dacWrite(cyd::AUDIO_PIN, 128);
  Serial.printf("AUDIO_TONE_END frequency=%uHz\n", frequencyHz);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Initialize the common CYD hardware state. The audio pin itself is driven
  // by the ESP32 DAC peripheral through dacWrite().
  cyd::beginBasicHardware();

  Serial.println();
  Serial.println("ESP32-2432S028R GPIO26 DAC audio test");
  Serial.printf("Audio/DAC pin: GPIO %u\n", cyd::AUDIO_PIN);
  Serial.println("AUDIO_TEST_READY");
  Serial.println("Expected sequence: 440 Hz -> 880 Hz -> 1760 Hz -> repeat");

  // Mid-scale idle value before the first tone.
  dacWrite(cyd::AUDIO_PIN, 128);
}

void loop() {
  Serial.println("AUDIO_SEQUENCE_START");

  playDacTone(440, 1500);
  delay(400);

  playDacTone(880, 1500);
  delay(400);

  playDacTone(1760, 1500);
  delay(400);

  Serial.println("AUDIO_SEQUENCE_COMPLETE");

  // A longer pause makes sequence boundaries easy to hear and record.
  dacWrite(cyd::AUDIO_PIN, 128);
  delay(2500);
}
