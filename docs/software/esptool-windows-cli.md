# esptool в Windows CLI

Короткая практическая инструкция для установки, обновления и проверки `esptool` в Windows.

## 1. Установить Python

Скачать Python для Windows:

- <https://www.python.org/downloads/windows/>

При установке включите:

```text
Add python.exe to PATH
```

Проверка в PowerShell или CMD:

```bat
python --version
```

Если команда `python` не работает, попробуйте:

```bat
py --version
```

## 2. Обновить pip

```bat
python -m pip install --upgrade pip
```

или:

```bat
py -m pip install --upgrade pip
```

## 3. Установить esptool

```bat
python -m pip install esptool
```

## 4. Обновить esptool

Та же команда с параметром `--upgrade`:

```bat
python -m pip install --upgrade esptool
```

## 5. Проверить версию

```bat
python -m esptool version
```

Альтернативно:

```bat
esptool version
```

Для воспроизводимой работы в этом репозитории предпочтительна форма:

```bat
python -m esptool ...
```

Она явно использует `esptool`, установленный в выбранный Python.

## 6. Посмотреть общую справку

```bat
python -m esptool --help
```

Краткая форма:

```bat
python -m esptool -h
```

## 7. Посмотреть справку по отдельной команде

```bat
python -m esptool read-flash --help
python -m esptool write-flash --help
python -m esptool flash-id --help
python -m esptool chip-id --help
```

В esptool 5 используются команды с дефисами:

```text
read-flash
write-flash
flash-id
chip-id
erase-flash
verify-flash
image-info
```

В старых инструкциях для esptool 4 и ранее можно встретить форму с подчёркиванием:

```text
read_flash
write_flash
flash_id
chip_id
```

Для текущей версии сначала ориентируйтесь на вывод `--help`.

## 8. Найти COM-порт

Откройте:

```text
Диспетчер устройств → Порты (COM и LPT)
```

После подключения платы должен появиться порт вида:

```text
USB-SERIAL CH340 (COM5)
```

или:

```text
Silicon Labs CP210x USB to UART Bridge (COM6)
```

В примерах ниже используется `COM5`. Замените его на свой порт.

## 9. Проверить связь с ESP32

```bat
python -m esptool --chip esp32 --port COM5 chip-id
```

Затем определить flash:

```bat
python -m esptool --chip esp32 --port COM5 flash-id
```

Эти команды не стирают и не записывают flash.

### Реальный успешный пример: chip-id

Команда была выполнена для платы на `COM12`:

```bat
python -m esptool --chip esp32 --port COM12 chip-id
```

Полученный вывод:

```text
esptool v5.3.1
Connected to ESP32 on COM12:
Chip type:          ESP32-D0WD-V3 (revision v3.0)
Features:           Wi-Fi, BT, Dual Core + LP Core, 240MHz, Vref calibration in eFuse, Coding Scheme None
Crystal frequency:  40MHz
MAC:                40:22:d8:xx:xx:xx

Stub flasher running.

Warning: ESP32 has no chip ID. Reading MAC address instead.
MAC:                40:22:d8:xx:xx:xx

Hard resetting via RTS pin...
```

Имя пользователя Windows удалено, а часть MAC-адреса скрыта, поскольку она не нужна для повторения процедуры.

Этот результат означает:

- `esptool` установлен и запускается;
- COM-порт выбран правильно;
- USB–UART и ROM-загрузчик ESP32 работают;
- определён чип `ESP32-D0WD-V3`, ревизия 3.0;
- кварц определён как 40 МГц;
- загрузка временного flasher stub прошла успешно;
- автоматический аппаратный сброс через RTS работает.

Предупреждение `ESP32 has no chip ID` является нормальным для классического ESP32: команда выводит MAC-адрес вместо отдельного chip ID.

### Реальный успешный пример: flash-id

Команда:

```bat
python -m esptool --chip esp32 --port COM12 flash-id
```

Полученный вывод:

```text
esptool v5.3.1
Connected to ESP32 on COM12:
Chip type:          ESP32-D0WD-V3 (revision v3.0)
Features:           Wi-Fi, BT, Dual Core + LP Core, 240MHz, Vref calibration in eFuse, Coding Scheme None
Crystal frequency:  40MHz
MAC:                40:22:d8:xx:xx:xx

Stub flasher running.

Flash Memory Information:
=========================
Manufacturer: ef
Device: 4016
Detected flash size: 4MB
Flash voltage set by a strapping pin: 3.3V

Hard resetting via RTS pin...
```

Результат подтверждает:

- flash доступна для чтения;
- JEDEC manufacturer ID: `0xEF`;
- JEDEC device ID: `0x4016`;
- обнаруженный объём: `4 MB` (`0x400000` байт);
- рабочее напряжение flash: `3,3 В`;
- связь и автоматический сброс продолжают работать штатно.

Команда `flash-id` ничего не стирает и не записывает.

## 10. Основной синтаксис

Общая форма:

```text
python -m esptool [общие параметры] команда [параметры команды]
```

Пример:

```bat
python -m esptool --chip esp32 --port COM5 --baud 460800 read-flash 0x0 0x1000 test.bin
```

Здесь:

- `--chip esp32` — семейство микроконтроллера;
- `--port COM5` — последовательный порт;
- `--baud 460800` — скорость обмена;
- `read-flash` — команда;
- `0x0` — начальный адрес;
- `0x1000` — количество байт;
- `test.bin` — имя выходного файла.

## 11. Безопасное тестовое чтение

Считать первые 4096 байт два раза:

```bat
python -m esptool --chip esp32 --port COM12 read-flash 0x0 0x1000 test-read-1.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x0 0x1000 test-read-2.bin
```

Проверить SHA-256 в PowerShell:

```powershell
Get-FileHash .\test-read-1.bin -Algorithm SHA256
Get-FileHash .\test-read-2.bin -Algorithm SHA256
```

Хэши должны совпасть. Дополнительное побайтовое сравнение:

```powershell
cmd /c fc /b test-read-1.bin test-read-2.bin
```

Ожидаемый результат:

```text
FC: no differences encountered
```

Эти команды только читают flash и подходят для первичной проверки стабильности чтения.

## 12. Где хранить файлы

Рекомендуемый рабочий каталог:

```text
C:\ESP32\firmware\
├── downloads\
├── backups\
├── logs\
└── hashes\
```

Перед выполнением команд перейти в каталог:

```bat
cd C:\ESP32\firmware
```

## 13. Типичные ошибки

### `python` не найден

Переустановите Python с включённым `Add python.exe to PATH` или используйте:

```bat
py -m esptool version
```

### `No module named esptool`

Установите esptool в тот же Python:

```bat
python -m pip install --upgrade esptool
```

### COM-порт занят

Закройте:

- Arduino Serial Monitor;
- PlatformIO Monitor;
- PuTTY;
- терминалы, использующие этот COM-порт.

### Ошибка `Failed to connect`

1. Проверьте номер COM-порта.
2. Удерживайте кнопку `BOOT`.
3. Кратко нажмите `RESET` или `EN`.
4. Запустите команду.
5. Отпустите `BOOT` после начала соединения.

### Ошибки на большой скорости

Уменьшите скорость:

```bat
python -m esptool --chip esp32 --port COM5 --baud 115200 flash-id
```

## 14. Официальные ссылки

- Документация esptool: <https://docs.espressif.com/projects/esptool/en/latest/esp32/>
- Основные команды: <https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/basic-commands.html>
- Установка: <https://docs.espressif.com/projects/esptool/en/latest/esp32/installation.html>
- GitHub: <https://github.com/espressif/esptool>
- Releases и готовые Windows-бинарники: <https://github.com/espressif/esptool/releases>
- Python для Windows: <https://www.python.org/downloads/windows/>

## 15. Минимальная проверка готовности

```text
[ ] python --version работает
[ ] pip обновлён
[ ] esptool установлен
[ ] python -m esptool version работает
[ ] python -m esptool --help работает
[ ] COM-порт определён
[x] chip-id выполнен
[x] flash-id выполнен
[ ] test-read-1.bin успешно считан
[ ] test-read-2.bin успешно считан
[ ] SHA-256 двух чтений совпадает
```

После прохождения этого списка можно переходить к полному резервному копированию заводской прошивки.