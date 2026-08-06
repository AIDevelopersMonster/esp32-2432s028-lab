# Диагностика адресно-зависимого сбоя `read-flash`

Проверено на ESP32-2432S028 с `ESP32-D0WD-V3`, Flash 4 MB и `esptool v5.3.1` под Windows.

## Наблюдаемые результаты

Одиночное чтение по 4096 байт:

| Адрес | `read-flash` |
|---:|---|
| `0x0000` | успешно |
| `0x1000` | успешно |
| `0x2000` | `PermissionError(13)` |
| `0x3000` | успешно |
| `0x7000` | `PermissionError(13)` |
| `0x8000` | успешно |
| `0x9000` | успешно |

Проверка тех же проблемных областей через `verify-flash` с локальным файлом из 4096 нулевых байт завершилась штатным результатом `digest mismatch` для:

- `0x2000`;
- `0x3000`;
- `0x7000`;
- `0x8000`.

## Локализация внутри сектора `0x2000–0x2FFF`

Чтение блоками по 256 байт:

| Диапазон | Результат |
|---|---|
| `0x2100–0x21FF` | `PermissionError(13)` |
| `0x2200–0x22FF` | успешно |
| `0x2300–0x23FF` | успешно |

Результат для `0x2000–0x20FF` пока не зафиксирован в журнале.

Последовательное деление блока `0x2100–0x21FF`:

| Диапазон | Размер | Результат |
|---|---:|---|
| `0x2100–0x217F` | 128 байт | `PermissionError(13)` |
| `0x2180–0x21FF` | 128 байт | успешно |
| `0x2100–0x213F` | 64 байта | `PermissionError(13)` |
| `0x2140–0x217F` | 64 байта | успешно |
| `0x2100–0x211F` | 32 байта | `PermissionError(13)` |
| `0x2120–0x213F` | 32 байта | успешно |
| `0x2100–0x210F` | 16 байт | `PermissionError(13)` |
| `0x2110–0x211F` | 16 байт | успешно, подтверждено дважды |

Сбой локализован до первых 16 байт диапазона `0x2100–0x210F`.

## Состояние платы после сбоя

После успешного чтения esptool выполняет `Hard resetting via RTS pin...`, после чего снова запускается пользовательская программа платы.

После сбоя наблюдаются:

- погасший дисплей;
- слабое красное свечение RGB-светодиода;
- отсутствие возврата к пользовательской программе до следующего успешного сброса или ручного RESET/EN.

Это согласуется не с зависанием уже работающей пользовательской программы, а с тем, что esptool перед операцией переводит ESP32 в serial bootloader, а после ошибки не удаётся надёжно вернуть плату в обычный режим загрузки. В таком состоянии пользовательская программа вообще не выполняется.

Слабое свечение RGB следует считать наблюдаемым состоянием GPIO во время serial bootloader/stub, а не диагностическим кодом ошибки, пока это отдельно не подтверждено.

## Вывод

`digest mismatch` означает, что esptool смог обратиться к указанной области Flash и сравнить её с локальным файлом. Следовательно:

```text
FLASH_REGION_ACCESSIBLE
RAW_DATA_TRANSFER_FAILURE
```

Проблема не является выходом за физический размер Flash и не похожа на запрет чтения отдельных секторов. Наблюдения указывают на зависимость сбоя от конкретного диапазона или содержащейся в нём последовательности байтов при передаче через `read-flash`.

Состояние дисплея и RGB после ошибки является, вероятнее всего, следствием неуспешного выхода из serial bootloader, а не причиной сбоя чтения.

Это пока рабочая гипотеза, а не установленная первопричина.

## Следующий решающий тест

Нужно отличить ошибку чтения от ошибки автоматического возврата из bootloader.

Сначала удалить старый файл, чтобы не принять его за новый результат:

```powershell
Remove-Item .\probe-2100.bin, .\trace-2100.txt -ErrorAction SilentlyContinue
```

Затем выполнить проблемное чтение без автоматического сброса после команды и с трассировкой:

```powershell
python -m esptool --trace --chip esp32 --port COM12 --after no-reset read-flash 0x2100 0x10 probe-2100.bin 2>&1 | Tee-Object .\trace-2100.txt
```

Проверить, появился ли файл:

```powershell
Get-Item .\probe-2100.bin -ErrorAction SilentlyContinue | Select-Object Name, Length
```

Интерпретация:

- файл отсутствует или имеет размер меньше 16 байт — сбой происходит в передаче данных чтения;
- файл имеет ровно 16 байт, а ошибка исчезла — основной сбой находится в пути автоматического hard reset через RTS;
- файл имеет 16 байт, но команда всё равно выдаёт pySerial-ошибку — нужно смотреть конец `trace-2100.txt`, чтобы определить точную операцию.

Поскольку `--after no-reset` намеренно оставляет ESP32 в загрузчике, после теста нужно нажать RESET/EN или переподключить питание.

После этого повторить тот же минимальный диапазон без flasher stub:

```powershell
Remove-Item .\probe-2100-rom.bin -ErrorAction SilentlyContinue
python -m esptool --trace --chip esp32 --port COM12 --after no-reset --no-stub read-flash --flash-size 4MB 0x2100 0x10 probe-2100-rom.bin
```

Полный дамп пока не запускать.

## Статус

```text
FLASH_SIZE_CONFIRMED_4MB
PROBLEM_REGIONS_VERIFY_SUCCESS
RAW_READ_FAILURE_NARROWED_TO_0x2100_0x210F
GOOD_CONTROL_0x2110_CONFIRMED_TWICE
BOARD_REMAINS_IN_SERIAL_BOOTLOADER_AFTER_FAILURE
RESET_PATH_VS_READ_PATH_TEST_PENDING
FULL_BACKUP_BLOCKED_BY_RAW_READ_FAILURE
ROOT_CAUSE_NOT_YET_ESTABLISHED
```

## Официальные источники

- <https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/basic-commands.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/advanced-options.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/advanced-topics/serial-protocol.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/troubleshooting.html>
