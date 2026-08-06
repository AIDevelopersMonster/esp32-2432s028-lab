/*
  ============================================================================
  Проект: 00_board_test
  Плата:   ESP32-2432S028 / CYD (Cheap Yellow Display)
  Назначение:
    Базовая проверка аппаратных узлов платы:
      - RGB-светодиода;
      - фоторезистора LDR;
      - кнопки BOOT;
      - последовательного порта Serial.

  Статус: ПРОВЕРЕНО — РАБОТАЕТ
  Дата проверки: 06.08.2026
  Среда: Arduino IDE
  Плата в Arduino IDE: ESP32 Dev Module
  Микроконтроллер: ESP32-D0WD-V3, revision 3.0
  Flash: 4 MB

  --------------------------------------------------------------------------
  ПРОВЕРЕННЫЕ НАСТРОЙКИ ARDUINO IDE
  --------------------------------------------------------------------------

  Board:
    "ESP32 Dev Module"

  Port:
    "COM12"
    Примечание: номер COM-порта зависит от компьютера.

  CPU Frequency:
    "240MHz (WiFi/BT)"

  Core Debug Level:
    "None"

  Erase All Flash Before Sketch Upload:
    "Enabled"

  Events Run On:
    "Core 1"

  Flash Frequency:
    "40MHz"

  Flash Mode:
    "DIO"

  Flash Size:
    "4MB (32Mb)"

  JTAG Adapter:
    "Disabled"

  Arduino Runs On:
    "Core 1"

  Partition Scheme:
    "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)"

  PSRAM:
    "Disabled"

  Upload Speed:
    "115200"

  Zigbee Mode:
    "Disabled"

  --------------------------------------------------------------------------
  ВАЖНО
  --------------------------------------------------------------------------

  При настройках Flash Frequency = 80 MHz и Flash Mode = QIO на проверенной
  плате возникала ошибка загрузки приложения:

    esp_image: Checksum failed
    No bootable app partitions in the partition table

  Устойчивый запуск получен при следующих параметрах:

    Flash Frequency = 40 MHz
    Flash Mode      = DIO
    Upload Speed    = 115200
    Erase All Flash Before Sketch Upload = Enabled

  После загрузки открыть монитор последовательного порта на скорости 115200 бод.

  Ожидаемая работа:
    1. RGB-светодиод переключается:
       красный -> зелёный -> синий -> выключен.
    2. В Serial Monitor выводится значение освещённости LDR.
    3. При нажатии BOOT состояние меняется:
       released -> pressed.

  Этот пример НЕ тестирует:
    - TFT-дисплей;
    - сенсорную панель;
    - microSD;
    - Wi-Fi;
    - Bluetooth;
    - аудиовыход.
  ============================================================================
*/

#include <CYD_Board.h>

void setup() {
  // Запускаем последовательный порт.
  // В Serial Monitor также необходимо выбрать скорость 115200 бод.
  Serial.begin(115200);

  // Небольшая задержка позволяет последовательному порту и плате
  // стабилизироваться после запуска.
  delay(300);

  // Инициализация базовых аппаратных узлов CYD:
  // RGB-светодиода, кнопки BOOT и других используемых библиотекой GPIO.
  cyd::beginBasicHardware();

  // ESP32 будет возвращать результат analogRead()
  // в диапазоне от 0 до 4095.
  analogReadResolution(12);

  // Сообщение подтверждает, что программа дошла до setup()
  // и успешно начала выполняться.
  Serial.println();
  Serial.println("================================");
  Serial.println("CYD basic board test");
  Serial.println("ESP32-2432S028: STARTED");
  Serial.println("Flash: DIO, 40 MHz");
  Serial.println("================================");
}

void loop() {
  // Номер текущего шага теста RGB-светодиода.
  static uint8_t step = 0;

  // Последовательно включаем три цвета RGB-светодиода,
  // а на четвёртом шаге полностью его выключаем.
  switch (step++ % 4) {
    case 0:
      cyd::setRgb(true, false, false);   // Красный
      break;

    case 1:
      cyd::setRgb(false, true, false);   // Зелёный
      break;

    case 2:
      cyd::setRgb(false, false, true);   // Синий
      break;

    default:
      cyd::rgbOff();                     // Все цвета выключены
      break;
  }

  // Читаем необработанное 12-битное значение фоторезистора.
  uint16_t lightValue = cyd::readLightRaw();

  // Кнопка BOOT активна низким уровнем:
  // LOW  — кнопка нажата;
  // HIGH — кнопка отпущена.
  bool bootPressed =
      digitalRead(cyd::BOOT_BUTTON_PIN) == LOW;

  // Выводим результаты проверки в Serial Monitor.
  Serial.printf(
      "LDR=%u, BOOT=%s\n",
      lightValue,
      bootPressed ? "pressed" : "released"
  );

  // Пауза между шагами теста.
  // Полный цикл RGB занимает приблизительно 2,8 секунды.
  delay(700);
}