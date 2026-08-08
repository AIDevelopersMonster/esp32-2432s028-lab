# Установка и настройка Arduino IDE

Актуальное состояние инструкции для проекта **ESP32-2432S028R / Cheap Yellow Display (CYD)**.

Проверено на реальной плате в ходе лабораторных тестов 00–04.

## Проверенный профиль платы

Для текущего экземпляра ESP32-2432S028R подтверждены:

- MCU: ESP32-D0WD-V3 rev. 3.0;
- Flash: 4 MB;
- TFT: ILI9341;
- Touch: XPT2046;
- USB-UART: CH340C;
- рабочая частота Flash: 40 MHz;
- рабочий режим Flash: DIO.

Главное правило проекта: **использовать сначала проверенные настройки ниже, а не более быстрые значения по умолчанию из случайных инструкций для других ESP32-плат**.

## 1. Установить пакет ESP32

Откройте в Arduino IDE:

**File → Preferences**

и добавьте URL менеджера плат Espressif, если он ещё не добавлен:

```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```

Затем откройте:

**Tools → Board → Boards Manager**

Найдите:

```
esp32 by Espressif Systems
```

и установите пакет.

После установки в списке плат должна быть доступна:

```
ESP32 Dev Module
```

Именно она используется в текущей лаборатории.

## 2. Библиотеки

### TFT_eSPI

Для тестов дисплея и Wi-Fi часов требуется:

```
TFT_eSPI by Bodmer
```

Установите её через **Library Manager**.

Полная инструкция проекта:

- [Установка и настройка TFT_eSPI](tft-espi-installation.md)

После установки обязательно примените конфигурацию проекта:

- [`config/tft_espi/User_Setup.h`](../config/tft_espi/User_Setup.h)

Без этого generic-конфигурация TFT_eSPI может использовать неправильный контроллер или GPIO.

### XPT2046_Touchscreen

Для теста тачскрина требуется:

```
XPT2046_Touchscreen by Paul Stoffregen
```

Установите её через **Library Manager**.

Подробная инструкция:

- [Установка XPT2046_Touchscreen](xpt2046-touchscreen-installation.md)

### Стандартные библиотеки ESP32

Отдельно устанавливать не требуется:

```
WiFi
SPI
SD
time
```

Они входят в установленный пакет ESP32 Arduino.

## 3. Установить локальную библиотеку CYD_Board

`CYD_Board` — собственная библиотека этого проекта. В Arduino Library Manager её нет.

Исходник находится здесь:

- [`libraries/CYD_Board`](../libraries/CYD_Board/README.md)

Каталог:

```
libraries/CYD_Board
```

нужно скопировать в каталог библиотек Arduino.

Обычно в Windows это:

```
C:\Users\<USER>\Documents\Arduino\libraries\CYD_Board
```

Из корня клонированного репозитория можно выполнить в PowerShell:

```
Copy-Item ".\libraries\CYD_Board" `
  "$HOME\Documents\Arduino\libraries" `
  -Recurse -Force
```

Проверка:

```
Test-Path "$HOME\Documents\Arduino\libraries\CYD_Board\library.properties"
Test-Path "$HOME\Documents\Arduino\libraries\CYD_Board\src\CYD_Board.h"
```

Обе команды должны вернуть:

```
True
```

После первой установки или обновления библиотеки полностью перезапустите Arduino IDE.

Не должно получиться вложенной структуры:

```
CYD_Board\CYD_Board\src\CYD_Board.h
```

Правильная структура:

```
Arduino\libraries\CYD_Board\library.properties
Arduino\libraries\CYD_Board\src\CYD_Board.h
```

## 4. Настроить TFT_eSPI

Найдите установленную библиотеку `TFT_eSPI` в Arduino sketchbook и сохраните резервную копию её текущего файла:

```
User_Setup.h
```

Затем замените его конфигурацией из проекта:

```
config/tft_espi/User_Setup.h
```

Если репозиторий открыт в PowerShell и текущий каталог — корень проекта:

```
$repo = (Get-Location).Path
$tft = "$HOME\Documents\Arduino\libraries\TFT_eSPI"

Copy-Item "$tft\User_Setup.h" `
  "$tft\User_Setup.h.backup" `
  -Force

Copy-Item "$repo\config\tft_espi\User_Setup.h" `
  "$tft\User_Setup.h" `
  -Force
```

После замены перезапустите Arduino IDE.

Подробности:

- [docs/tft-espi-installation.md](tft-espi-installation.md)

Важно: `TFT_eSPI` использует общую конфигурацию библиотеки. Замена `User_Setup.h` может повлиять на другие проекты с TFT. Для этой лаборатории используется конфигурация, сохранённая непосредственно в репозитории.

## 5. Проверенные настройки Arduino IDE

Для текущей ESP32-2432S028R используйте:

| Параметр | Проверенное значение |
|---|---|
| Board | ESP32 Dev Module |
| CPU Frequency | 240 MHz |
| Flash Frequency | 40 MHz |
| Flash Mode | DIO |
| Flash Size | 4 MB |
| Partition Scheme | Default 4MB with spiffs |
| PSRAM | Disabled |
| Upload Speed | 115200 |
| Erase All Flash Before Sketch Upload | Enabled для чистой лабораторной загрузки |
| Port | фактический COM-порт CH340C вашей платы |

### Почему здесь DIO и 40 MHz

Эти параметры выбраны не как теоретическая рекомендация, а по фактическому тестированию платы.

Комбинация более агрессивных настроек Flash, использовавшаяся ранее, приводила после загрузки к ошибкам вида:

```
Checksum failed
OTA app partition slot 0 is not bootable
No bootable app partitions in the partition table
```

После перехода на:

```
Flash Frequency: 40 MHz
Flash Mode: DIO
Upload Speed: 115200
```

загрузка и последующий старт скетчей работают стабильно.

Поэтому для лаборатории **не начинайте с QIO, 80 MHz или высокой Upload Speed**. Сначала воспроизведите проверенный профиль.

## 6. Проверить COM-порт

Подключите плату USB-кабелем с линиями данных.

В Arduino IDE откройте:

**Tools → Port**

и выберите COM-порт CH340C этой платы.

Номер COM-порта не является постоянной характеристикой платы и может изменяться между компьютерами и USB-разъёмами.

Если порт не появляется:

- убедитесь, что USB-кабель поддерживает передачу данных;
- попробуйте другой USB-порт;
- проверьте наличие драйвера CH340;
- закройте программы, которые уже используют этот COM-порт.

В ходе диагностики этой платы замена проблемного USB-кабеля устранила ошибки чтения Flash, поэтому качество кабеля здесь является реально проверенным фактором.

## 7. Компиляция и загрузка

Для каждого примера сначала нажмите:

```
Verify
```

После успешной компиляции:

```
Upload
```

Если автоматический переход в bootloader не срабатывает, можно использовать BOOT/RESET вручную. Для обычной работы платы это, как правило, не требуется.

После загрузки примеров с диагностикой откройте Serial Monitor на:

```
115200 baud
```

## 8. Рекомендуемый порядок лабораторных примеров

Рабочая последовательность на текущий момент:

1. [`00_board_test`](../examples/00_board_test/README.md) — базовая плата, `CYD_Board`, RGB, LDR, BOOT;
2. [`01_display_test`](../examples/01_display_test/README.md) — TFT ILI9341;
3. [`02_touch_test`](../examples/02_touch_test/README.md) — XPT2046 raw touch;
4. [`03_sd_test`](../examples/03_sd_test/README.md) — безопасное чтение microSD;
5. [`03_sd_rw_lab`](../examples/03_sd_rw_lab/README.md) — создание каталогов, запись, readback и append;
6. [`04_wifi_clock`](../examples/04_wifi_clock/README.md) — Wi-Fi, NTP и вывод времени на TFT.

Общий индекс:

- [examples/README.md](../examples/README.md)

На текущем этапе все перечисленные аппаратные тесты были фактически запущены на реальной ESP32-2432S028R; результаты фиксируются в README каждого примера.

## 9. Что требуется для каждого примера

| Пример | Дополнительные компоненты |
|---|---|
| 00_board_test | `CYD_Board` |
| 01_display_test | `CYD_Board`, `TFT_eSPI`, проектный `User_Setup.h` |
| 02_touch_test | `CYD_Board`, `TFT_eSPI`, `XPT2046_Touchscreen`, проектный `User_Setup.h` |
| 03_sd_test | `CYD_Board`, стандартные `SPI` и `SD` |
| 03_sd_rw_lab | `CYD_Board`, стандартные `SPI` и `SD` |
| 04_wifi_clock | `CYD_Board`, `TFT_eSPI`, стандартные `WiFi` и `time`, локальный `secrets.h` |

Для `04_wifi_clock` реальные имя и пароль Wi-Fi храните только в локальном:

```
secrets.h
```

В Git оставляйте только:

```
secrets.example.h
```

## 10. Типичные неисправности

### `CYD_Board.h: No such file or directory`

Проверьте установку:

- [libraries/CYD_Board/README.md](../libraries/CYD_Board/README.md)

и наличие:

```
Documents\Arduino\libraries\CYD_Board\src\CYD_Board.h
```

### `TFT_eSPI.h: No such file or directory`

Установите `TFT_eSPI by Bodmer` через Library Manager:

- [инструкция TFT_eSPI](tft-espi-installation.md)

### `XPT2046_Touchscreen.h: No such file or directory`

Установите `XPT2046_Touchscreen by Paul Stoffregen`:

- [инструкция XPT2046_Touchscreen](xpt2046-touchscreen-installation.md)

### Белый или чёрный TFT

Проверьте:

- установлена ли `TFT_eSPI`;
- применён ли именно [`config/tft_espi/User_Setup.h`](../config/tft_espi/User_Setup.h);
- включается ли подсветка GPIO 21;
- выбран ли контроллер ILI9341;
- используется ли проверенный профиль платы и Flash.

Сначала вернитесь к:

- [`01_display_test`](../examples/01_display_test/README.md)

### Цвета TFT выглядят неправильно

Проверяйте прежде всего конфигурацию `TFT_eSPI` и `TFT_RGB_ORDER`.

Фотография экрана не является точным колориметрическим эталоном: камера, баланс белого и экспозиция могут заметно менять видимые оттенки.

### Тачскрин зеркальный или оси перепутаны

Для raw-теста это допустимо. `02_touch_test` предназначен для подтверждения связи с XPT2046 и получения сырых X/Y/Z.

Калибровка и преобразование координат выполняются отдельным этапом.

### microSD не определяется

Проверьте:

```
MISO  GPIO 19
MOSI  GPIO 23
SCLK  GPIO 18
CS    GPIO 5
```

и сначала запустите:

- [`03_sd_test`](../examples/03_sd_test/README.md)

На проверенной карте получен размер:

```
Card size: 30250 MB
```

Также успешно подтверждены создание вложенных каталогов, запись, повторное чтение и append в:

- [`03_sd_rw_lab`](../examples/03_sd_rw_lab/README.md)

### Wi-Fi не подключается

Проверьте:

- локальный `secrets.h`;
- SSID и пароль;
- сеть 2,4 ГГц;
- доступ в интернет;
- отсутствие captive portal.

Подробности:

- [`04_wifi_clock`](../examples/04_wifi_clock/README.md)

### Ошибка загрузки или скетч не стартует после Upload

Верните проверенные параметры:

```
Board: ESP32 Dev Module
CPU Frequency: 240 MHz
Flash Frequency: 40 MHz
Flash Mode: DIO
Flash Size: 4 MB
Upload Speed: 115200
PSRAM: Disabled
```

При лабораторной чистой загрузке также используйте:

```
Erase All Flash Before Sketch Upload: Enabled
```

Не переходите к QIO или повышенной частоте Flash, пока базовый тест на DIO/40 MHz не работает стабильно.

## 11. Зафиксированный рабочий минимум

Если нужно быстро восстановить известную рабочую конфигурацию проекта, используйте следующую последовательность:

```
Board              ESP32 Dev Module
CPU Frequency      240 MHz
Flash Frequency    40 MHz
Flash Mode         DIO
Flash Size         4 MB
Partition Scheme   Default 4MB with spiffs
PSRAM               Disabled
Upload Speed        115200
Serial Monitor      115200 baud
```

Затем:

1. установить `CYD_Board`;
2. установить `TFT_eSPI` и применить проектный `User_Setup.h`;
3. установить `XPT2046_Touchscreen` перед touch-тестом;
4. начать с `00_board_test` и идти по последовательности лаборатории.
