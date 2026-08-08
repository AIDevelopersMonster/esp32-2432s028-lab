# ESP32-2432S028R Lab

Практическая лаборатория для платы **ESP32-2432S028R**, известной как **Cheap Yellow Display (CYD)**: ESP32-WROOM-32, цветной TFT-дисплей 2,8 дюйма, резистивный тачскрин, microSD, RGB-светодиод, фоторезистор, аудиовыход, Wi-Fi и Bluetooth на одной плате.

Проект построен как последовательность проверяемых примеров: сначала базовое оборудование, затем дисплей, тачскрин, карта памяти, Wi-Fi, Bluetooth, аудио и разъёмы расширения. Корневой пример собирается через PlatformIO; скетчи из `examples/` можно запускать в Arduino IDE.

> Проект рассчитан прежде всего на распространённую ревизию **ESP32-2432S028R с ILI9341 и XPT2046**. Перед подключением внешних устройств сравните маркировку и разводку своей платы: у продавцов встречаются близкие ревизии с отличиями.

## Начать с библиотеки CYD_Board

Перед запуском примеров установите и проверьте локальную библиотеку проекта:

**[`libraries/CYD_Board/README.md`](libraries/CYD_Board/README.md)**

Первый проверочный скетч:

**[`examples/00_board_test/README.md`](examples/00_board_test/README.md)**

После его успешной работы переходите к проверке дисплея:

**[`examples/01_display_test/README.md`](examples/01_display_test/README.md)**

## B8 hardware acceptance — начать здесь

Для пошаговой аппаратной приёмки откройте:

**[`docs/HARDWARE-ACCEPTANCE-START.md`](docs/HARDWARE-ACCEPTANCE-START.md)**

Первый автоматизированный шаг HW-01 запускается из корня репозитория:

```
python tools/run_hw01_identity.py
```

Программа задаёт вопросы об экземпляре платы, выполняет сборку, вычисляет SHA-256 `firmware.bin`, копирует точный бинарный файл в evidence-каталог и создаёт JSON-протокол HW-01. Фотографии платы и итоговое решение PASS добавляются только после проверки оператором.

## Что уже подготовлено

- самотест дисплея, подсветки, RGB-светодиода, кнопки BOOT и LDR;
- тест ILI9341 с цветными полями;
- вывод необработанных координат XPT2046 для последующей калибровки;
- безопасный read-only тест microSD;
- отдельная лаборатория записи, readback, append и вложенных каталогов на microSD;
- простой сетевой экран часов через Wi-Fi и NTP;
- проверенный Bluetooth Classic SPP с двусторонним serial-обменом;
- проверенный BLE GATT-тест с advertising, READ/WRITE/NOTIFY;
- подготовленный тест аудиотракта GPIO26 / DAC2;
- подготовленный loopback-тест GPIO22/GPIO27 и входа GPIO35 на разъёмах расширения;
- общая библиотека `CYD_Board` с именованными выводами платы;
- конфигурация TFT_eSPI для Arduino IDE и PlatformIO;
- автоматическая проверка сборки через GitHub Actions.

## Основные характеристики

| Узел | Типовое исполнение |
|---|---|
| Микроконтроллер | ESP32-WROOM-32, два ядра Xtensa LX6, до 240 МГц |
| Беспроводная связь | Wi-Fi 2,4 ГГц, Bluetooth Classic/BLE |
| Дисплей | 2,8 дюйма, 240×320, ILI9341, SPI |
| Тачскрин | резистивный, XPT2046, SPI |
| Память программы | обычно 4 МБ Flash |
| Карта памяти | microSD, SPI |
| Дополнительно | RGB LED, LDR, BOOT/RESET, аудиовыход/усилитель, разъёмы расширения |
| Питание/прошивка | 5 В через USB, преобразователь USB–UART CH340C на распространённой ревизии |

## Быстрый старт: PlatformIO

1. Установите Visual Studio Code и расширение PlatformIO.
2. Откройте корневую папку репозитория.
3. Подключите плату по USB.
4. Выполните **PlatformIO: Upload**.
5. Откройте монитор порта на скорости **115200 бод**.

Команды терминала:

```
pio run
pio run -t upload
pio device monitor -b 115200
```

После запуска дисплей покажет состояние самотеста, RGB-светодиод будет перебирать цвета, а в монитор порта начнут выводиться показания LDR и состояние BOOT.

## Быстрый старт: Arduino IDE

Подробная инструкция находится в [`docs/arduino-ide.md`](docs/arduino-ide.md).

Кратко:

1. установите пакет плат **esp32 by Espressif Systems**;
2. установите библиотеки **TFT_eSPI** и, для тачскрина, **XPT2046_Touchscreen**;
3. скопируйте `libraries/CYD_Board` в каталог библиотек Arduino;
4. замените `TFT_eSPI/User_Setup.h` файлом `config/tft_espi/User_Setup.h`;
5. выберите плату **ESP32 Dev Module**;
6. используйте проверенный профиль Flash `40 MHz / DIO`, Upload Speed `115200`;
7. откройте нужный скетч из `examples/`.

Для Bluetooth Classic и BLE отдельные сторонние библиотеки не нужны: используемые Bluetooth API входят в Arduino core для ESP32.

## Примеры

| Каталог | Назначение | Дополнительные библиотеки |
|---|---|---|
| `00_board_test` | RGB LED, LDR, BOOT | CYD_Board |
| `01_display_test` | первичная проверка ILI9341 | CYD_Board, TFT_eSPI |
| `02_touch_test` | необработанные X/Y/Z тачскрина | CYD_Board, TFT_eSPI, XPT2046_Touchscreen |
| `03_sd_test` | безопасная инициализация microSD и список файлов | CYD_Board, стандартные SPI/SD |
| `03_sd_rw_lab` | каталоги, запись, readback и append | CYD_Board, стандартные SPI/SD |
| `04_wifi_clock` | Wi-Fi и часы NTP | CYD_Board, TFT_eSPI, стандартные WiFi/time |
| `05_bluetooth_classic_test` | Bluetooth Classic SPP, передача в обе стороны | стандартный BluetoothSerial |
| `06_ble_test` | BLE advertising и GATT READ/WRITE/NOTIFY | стандартный BLE API ESP32 |
| `07_audio_test` | GPIO26 / DAC2 и штатный аудиотракт | CYD_Board |
| `08_expansion_io_test` | loopback GPIO22↔GPIO27 и вход/ADC GPIO35 | нет |

Для `04_wifi_clock` скопируйте `secrets.example.h` в `secrets.h` и укажите данные своей сети. Файл `secrets.h` не должен публиковаться в Git.

Bluetooth Classic и BLE уже проверены на реальной плате. Примеры `07_audio_test` и `08_expansion_io_test` подготовлены, но должны оставаться в статусе `READY FOR HARDWARE TEST` до фактического прогона.

## Распиновка

Полная таблица и замечания находятся в [`docs/pinout.md`](docs/pinout.md).

Ключевые подключения распространённой ревизии:

- TFT: MISO 12, MOSI 13, SCLK 14, CS 15, DC 2, подсветка 21;
- Touch: IRQ 36, MOSI 32, MISO 39, CLK 25, CS 33;
- microSD: MISO 19, MOSI 23, SCLK 18, CS 5;
- RGB LED: R 4, G 16, B 17, логика инверсная;
- LDR: GPIO 34;
- аудио: GPIO 26;
- BOOT: GPIO 0;
- expansion lab: GPIO22, GPIO27, GPIO35 input-only.

## Важные ограничения

- Большинство GPIO уже занято встроенными устройствами.
- GPIO 34, 35, 36 и 39 работают только как входы.
- RGB-светодиод включается уровнем `LOW`.
- GPIO 0 является загрузочным выводом; не удерживайте его в `LOW` во время сброса без необходимости.
- GPIO 21 управляет подсветкой и одновременно выведен на одном из разъёмов некоторых ревизий.
- Тачскрин и microSD разведены на разные группы выводов. При объединении их в одном приложении заранее продумайте распределение аппаратных SPI-контроллеров либо программный SPI.
- Цвета, поворот и калибровка тачскрина могут отличаться между партиями.
- Отдельные Bluetooth-тесты не доказывают устойчивую совместную работу Wi-Fi + Bluetooth Classic + BLE под высокой нагрузкой.
- Расположение контактов P3/CN1 может отличаться между ревизиями; перед loopback-тестом идентифицируйте реальные GPIO конкретной платы.

## Структура репозитория

```
esp32-2432s028-lab/
├── src/                      # основной PlatformIO-самотест
├── libraries/CYD_Board/      # библиотека распиновки и помощников
├── config/tft_espi/          # User_Setup.h для TFT_eSPI
├── examples/                 # последовательные Arduino-примеры
├── docs/                     # распиновка, установка, идеи проектов
├── hardware/images/          # собственные фотографии и схемы
└── .github/workflows/        # CI-сборка PlatformIO
```

## План развития

- калибратор тачскрина с сохранением коэффициентов;
- демонстрация LVGL 9;
- меню диагностики всех встроенных узлов;
- работа дисплея, тачскрина и microSD в одном приложении;
- совместный radio coexistence-тест Wi-Fi/Bluetooth после отдельных PASS;
- веб-панель управления;
- интеграция с MQTT и Home Assistant;
- дополнительные I2C-примеры с внешними датчиками через свободные GPIO;
- корпуса и файлы 3D-печати.

## Источники и полезные материалы

- Arduino ESP32 DAC: <https://docs.espressif.com/projects/arduino-esp32/en/latest/api/dac.html>
- Arduino ESP32 Bluetooth Classic: <https://docs.espressif.com/projects/arduino-esp32/en/latest/api/bluetooth.html>
- ESP-IDF GPIO: <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html>
- ESP-IDF Bluetooth API: <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/index.html>
- NuttX board documentation: <https://nuttx.apache.org/docs/latest/platforms/xtensa/esp32/boards/esp32-2432S028/index.html>
- Random Nerd Tutorials pinout: <https://randomnerdtutorials.com/esp32-cheap-yellow-display-cyd-pinout-esp32-2432s028r/>
- LVGL Arduino integration: <https://docs.lvgl.io/master/integration/frameworks/arduino.html>
- TFT_eSPI: <https://github.com/Bodmer/TFT_eSPI>
- XPT2046_Touchscreen: <https://github.com/PaulStoffregen/XPT2046_Touchscreen>

## Публикация на GitHub

Рекомендуемое имя, описание, topics и команды первой публикации приведены в [`docs/github-publication.md`](docs/github-publication.md).

## Лицензия

Код распространяется по лицензии [MIT](LICENSE). Фотографии, схемы продавцов и сторонние материалы не становятся MIT-лицензированными автоматически; проверяйте их исходные условия отдельно.
