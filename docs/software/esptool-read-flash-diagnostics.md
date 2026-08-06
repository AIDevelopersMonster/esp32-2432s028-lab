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

Чтение блоками по 256 байт дало:

| Диапазон | Результат |
|---|---|
| `0x2100–0x21FF` | `PermissionError(13)` |
| `0x2200–0x22FF` | успешно |
| `0x2300–0x23FF` | успешно |

Результат для `0x2000–0x20FF` пока не зафиксирован в журнале.

Разделение блока `0x2100–0x21FF` на половины по 128 байт:

| Диапазон | Результат |
|---|---|
| `0x2100–0x217F` | `PermissionError(13)` |
| `0x2180–0x21FF` | результат пока не получен |

Таким образом, сбой уже нельзя объяснить только размером операции: соседние блоки одинаковой длины ведут себя по-разному.

## Вывод

`digest mismatch` означает, что esptool смог обратиться к указанной области Flash и сравнить её с локальным файлом. Следовательно:

```text
FLASH_REGION_ACCESSIBLE
RAW_DATA_TRANSFER_FAILURE
```

Проблема не является выходом за физический размер Flash и не похожа на запрет чтения отдельных секторов. Наблюдения указывают на зависимость сбоя от конкретного диапазона или содержащейся в нём последовательности байтов при передаче через `read-flash`.

Это пока рабочая гипотеза, а не установленная первопричина.

## Следующая локализация

Сначала получить результат второй половины:

```powershell
python -m esptool --chip esp32 --port COM12 read-flash 0x2180 0x80 test-2180-0080.bin
```

Если `0x2180–0x21FF` читается, делить только проблемную половину `0x2100–0x217F`:

```powershell
python -m esptool --chip esp32 --port COM12 read-flash 0x2100 0x40 test-2100-0040.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x2140 0x40 test-2140-0040.bin
```

Если обе половины по `0x80` падают, перейти к блокам по 16 байт:

```powershell
python -m esptool --chip esp32 --port COM12 read-flash 0x2100 0x10 test-2100-0010.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x2110 0x10 test-2110-0010.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x2120 0x10 test-2120-0010.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x2130 0x10 test-2130-0010.bin
```

Полный дамп пока не запускать.

## Статус

```text
FLASH_SIZE_CONFIRMED_4MB
PROBLEM_REGIONS_VERIFY_SUCCESS
RAW_READ_FAILURE_NARROWED_TO_0x2100_0x217F
SECOND_HALF_0x2180_PENDING
FULL_BACKUP_BLOCKED_BY_RAW_READ_FAILURE
ROOT_CAUSE_NOT_YET_ESTABLISHED
```

## Официальные источники

- <https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/basic-commands.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/advanced-topics/serial-protocol.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/troubleshooting.html>
