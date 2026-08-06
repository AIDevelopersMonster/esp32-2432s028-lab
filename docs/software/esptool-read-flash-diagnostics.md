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

Разделение блока `0x2100–0x21FF`:

| Диапазон | Размер | Результат |
|---|---:|---|
| `0x2100–0x217F` | 128 байт | `PermissionError(13)` |
| `0x2180–0x21FF` | 128 байт | успешно |
| `0x2100–0x213F` | 64 байта | `PermissionError(13)` |
| `0x2140–0x217F` | 64 байта | успешно |

Сбой локализован до диапазона `0x2100–0x213F` длиной 64 байта.

## Вывод

`digest mismatch` означает, что esptool смог обратиться к указанной области Flash и сравнить её с локальным файлом. Следовательно:

```text
FLASH_REGION_ACCESSIBLE
RAW_DATA_TRANSFER_FAILURE
```

Проблема не является выходом за физический размер Flash и не похожа на запрет чтения отдельных секторов. Наблюдения указывают на зависимость сбоя от конкретного диапазона или содержащейся в нём последовательности байтов при передаче через `read-flash`.

Это пока рабочая гипотеза, а не установленная первопричина.

## Следующая локализация

Разделить проблемные 64 байта на две половины:

```powershell
python -m esptool --chip esp32 --port COM12 read-flash 0x2100 0x20 test-2100-0020.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x2120 0x20 test-2120-0020.bin
```

Если одна половина падает, разделить её на блоки по 16 байт. Например, если проблемной окажется `0x2100–0x211F`:

```powershell
python -m esptool --chip esp32 --port COM12 read-flash 0x2100 0x10 test-2100-0010.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x2110 0x10 test-2110-0010.bin
```

После определения минимального проблемного диапазона повторить точно такой же адрес и размер с `--no-stub`, чтобы проверить, воспроизводится ли сбой в ROM-загрузчике без flasher stub.

Полный дамп пока не запускать.

## Статус

```text
FLASH_SIZE_CONFIRMED_4MB
PROBLEM_REGIONS_VERIFY_SUCCESS
RAW_READ_FAILURE_NARROWED_TO_0x2100_0x213F
FULL_BACKUP_BLOCKED_BY_RAW_READ_FAILURE
ROOT_CAUSE_NOT_YET_ESTABLISHED
```

## Официальные источники

- <https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/basic-commands.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/advanced-topics/serial-protocol.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/troubleshooting.html>
