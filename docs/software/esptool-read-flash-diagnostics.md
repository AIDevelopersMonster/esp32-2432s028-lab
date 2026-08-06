# Диагностика нестабильного `read-flash`

Проверено на ESP32-2432S028 с `ESP32-D0WD-V3`, Flash 4 MB и `esptool v5.3.1` под Windows.

## Итог эксперимента

До замены USB-кабеля чтение регулярно обрывалось после первых 8192 байт с ошибками pySerial:

```text
GetOverlappedResult failed (PermissionError 13)
Cannot configure port (PermissionError 13)
WriteFile failed (PermissionError 13)
```

Сбой воспроизводился:

- на `115200`, `57600` и `460800`;
- с flasher stub и с `--no-stub`;
- при чтении разных адресов;
- при отключённом завершающем сбросе `--after no-reset`.

Один и тот же адрес мог сначала завершиться ошибкой, а затем успешно считаться. Поэтому гипотеза об особом или недоступном диапазоне Flash была отклонена.

После замены USB-кабеля успешно выполнено непрерывное чтение 65536 байт:

```powershell
python -m esptool --chip esp32 --port COM12 --baud 57600 `
  --after no-reset `
  read-flash 0x0 0x10000 slow-64k-57600.bin
```

Результат:

```text
Read 65536 bytes from 0x00000000 in 11.8 seconds (44.3 kbit/s)
Staying in bootloader.
```

## Вывод

Наиболее сильное экспериментальное объяснение — нестабильный старый USB-кабель или контакт в USB-тракте.

```text
FLASH_ADDRESS_FAILURE_RETRACTED
FLASH_SIZE_CONFIRMED_4MB
64KB_READ_SUCCESS_AFTER_CABLE_REPLACEMENT
USB_CABLE_OR_CONNECTION_PATH_WAS_PRIMARY_SUSPECT
FULL_BACKUP_VALIDATION_PENDING
```

Один успешный тест 64 KB ещё не доказывает устойчивость полного чтения 4 MB. Перед любыми операциями записи нужны два полных совпадающих дампа.

## Следующий шаг

Поскольку команда использовала `--after no-reset`, сначала нажать `RESET/EN` или переподключить питание.

Затем выполнить первый полный дамп на проверенной скорости `57600`:

```powershell
Remove-Item .\esp32-2432s028-full-1.bin -ErrorAction SilentlyContinue

python -m esptool --chip esp32 --port COM12 --baud 57600 `
  read-flash 0x0 0x400000 esp32-2432s028-full-1.bin
```

После завершения проверить размер:

```powershell
Get-Item .\esp32-2432s028-full-1.bin | Select-Object Name, Length, LastWriteTime
```

Ожидается:

```text
4194304
```

После первого успешного полного чтения выполнить второй независимый дамп и сравнить SHA-256.

## Практический урок

При повторяющихся `PermissionError(13)` и обрыве на одном и том же объёме данных сначала следует заменить USB-кабель на короткий заведомо исправный кабель передачи данных, а уже затем исследовать драйвер, скорость, адреса Flash и USB-UART.

## Официальные источники

- <https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/basic-commands.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/advanced-options.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/troubleshooting.html>
