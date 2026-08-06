# 00_board_test — базовая проверка ESP32-2432S028 / CYD

## Статус

**ПРОВЕРЕНО: РАБОТАЕТ**

Дата проверки: **06.08.2026**

Проверенная плата:

- ESP32-2432S028 / CYD (Cheap Yellow Display)
- микроконтроллер: ESP32-D0WD-V3, revision 3.0
- кварц: 40 MHz
- Flash: 4 MB
- порт при проверке: COM12

> Номер COM-порта зависит от компьютера и может быть другим.

## Назначение скетча

Скетч `00_board_test.ino` выполняет базовую проверку платы:

- выводит диагностические сообщения в Serial Monitor;
- поочерёдно включает красный, зелёный и синий каналы RGB-светодиода;
- считывает значение фоторезистора LDR;
- считывает состояние кнопки BOOT;
- инициализирует базовое оборудование платы через `cyd::beginBasicHardware()`.

## Что наблюдалось при проверке

При исправной работе:

1. RGB-светодиод переключается по циклу:

   `красный → зелёный → синий → выключен`

2. В Serial Monitor примерно каждые 700 мс появляются строки вида:

   ```text
   LDR=1234, BOOT=released
   ```

3. При нажатии кнопки BOOT состояние меняется на:

   ```text
   BOOT=pressed
   ```

4. Значение `LDR` изменяется при изменении освещения.

5. **Подсветка TFT-дисплея горит.**

Подсветка TFT подтверждает, что цепь подсветки включена, но данный скетч **не выводит графику на экран** и не является полной проверкой TFT-контроллера.

## Проверенные настройки Arduino IDE

Скетч был проверен при следующих параметрах меню **Tools / Инструменты**:

| Параметр | Значение |
|---|---|
| Board | `ESP32 Dev Module` |
| Port | `COM12` |
| CPU Frequency | `240MHz (WiFi/BT)` |
| Core Debug Level | `None` |
| Erase All Flash Before Sketch Upload | `Enabled` |
| Events Run On | `Core 1` |
| Flash Frequency | `40MHz` |
| Flash Mode | `DIO` |
| Flash Size | `4MB (32Mb)` |
| JTAG Adapter | `Disabled` |
| Arduino Runs On | `Core 1` |
| Partition Scheme | `Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)` |
| PSRAM | `Disabled` |
| Upload Speed | `115200` |
| Zigbee Mode | `Disabled` |

## Критически важные параметры

Для проверенной платы устойчивый запуск был получен при сочетании:

```text
Flash Frequency: 40MHz
Flash Mode: DIO
Upload Speed: 115200
Erase All Flash Before Sketch Upload: Enabled
```

При прежних настройках `80MHz` и `QIO` наблюдалась ошибка загрузки приложения:

```text
esp_image: Checksum failed
No bootable app partitions in the partition table
```

## Serial Monitor

Скорость Serial Monitor:

```text
115200 baud
```

После загрузки скетча при необходимости нажмите кнопку `EN/RST`.

Ожидаемое стартовое сообщение:

```text
================================
CYD basic board test
ESP32-2432S028: STARTED
Flash: DIO, 40 MHz
TFT backlight: ON (observed)
================================
```

## Что этот скетч не проверяет

Этот пример не выполняет полную проверку:

- TFT-контроллера и вывода графики;
- сенсорной панели;
- microSD;
- Wi‑Fi;
- Bluetooth;
- аудиовыхода.

Для этих узлов нужны отдельные тестовые скетчи.
