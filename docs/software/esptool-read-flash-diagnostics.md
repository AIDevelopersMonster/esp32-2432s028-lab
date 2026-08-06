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

## Вывод

`digest mismatch` означает, что esptool смог обратиться к указанной области Flash и сравнить её с локальным файлом. Следовательно:

```text
FLASH_REGION_ACCESSIBLE
RAW_DATA_TRANSFER_FAILURE
```

Проблема не является выходом за физический размер Flash и не похожа на запрет чтения отдельных секторов. Сбой проявляется при передаче полного содержимого некоторых областей на компьютер через `read-flash`.

## Следующая локализация

Не запускать полный дамп. Сначала разделить проблемный сектор `0x2000–0x2FFF` на меньшие части:

```powershell
python -m esptool --chip esp32 --port COM12 read-flash 0x2000 0x100 test-2000-0100.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x2100 0x100 test-2100-0100.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x2200 0x100 test-2200-0100.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x2300 0x100 test-2300-0100.bin
```

Если эти чтения успешны, увеличить размер последовательно:

```powershell
python -m esptool --chip esp32 --port COM12 read-flash 0x2000 0x200 test-2000-0200.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x2000 0x400 test-2000-0400.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x2000 0x800 test-2000-0800.bin
```

Цель — установить, зависит ли сбой от конкретного поддиапазона, размера ответа или содержимого передаваемых данных.

## Статус

```text
FLASH_SIZE_CONFIRMED_4MB
PROBLEM_REGIONS_VERIFY_SUCCESS
FULL_BACKUP_BLOCKED_BY_RAW_READ_FAILURE
ROOT_CAUSE_NOT_YET_ESTABLISHED
```

## Официальные источники

- <https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/basic-commands.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/advanced-topics/serial-protocol.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/troubleshooting.html>
