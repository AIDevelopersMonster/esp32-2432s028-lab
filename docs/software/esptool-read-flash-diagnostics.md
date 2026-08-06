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

После замены USB-кабеля сначала успешно выполнено непрерывное чтение 65536 байт на `57600`, а затем два полных чтения всей Flash на `115200`.

Первый полный дамп:

```text
Read 4194304 bytes from 0x00000000 in 382.5 seconds (87.7 kbit/s)
esp32-2432s028-full-1.bin
```

Второй полный дамп:

```text
Read 4194304 bytes from 0x00000000 in 387.8 seconds (86.5 kbit/s)
esp32-2432s028-full-2.bin
```

Размер каждого файла подтверждён:

```text
4194304 bytes
```

## Вывод

Наиболее сильное экспериментальное объяснение — неисправный или неподходящий старый USB-кабель либо нестабильный контакт в USB-тракте.

```text
FLASH_ADDRESS_FAILURE_RETRACTED
FLASH_SIZE_CONFIRMED_4MB
TWO_COMPLETE_4MB_READS_SUCCEEDED_AFTER_CABLE_REPLACEMENT
USB_CABLE_OR_CONNECTION_PATH_CONFIRMED_AS_PRIMARY_CAUSE
HASH_COMPARISON_PENDING
```

Успешные полные чтения на `115200` показывают, что снижать скорость до `57600` для этой связки после замены кабеля не требуется.

## Финальная проверка дампов

Выполнить:

```powershell
Get-FileHash .\esp32-2432s028-full-1.bin -Algorithm SHA256
Get-FileHash .\esp32-2432s028-full-2.bin -Algorithm SHA256

(Get-FileHash .\esp32-2432s028-full-1.bin -Algorithm SHA256).Hash -eq `
(Get-FileHash .\esp32-2432s028-full-2.bin -Algorithm SHA256).Hash
```

Ожидаемый логический результат:

```text
True
```

Дополнительная побайтовая проверка:

```powershell
cmd /c fc /b esp32-2432s028-full-1.bin esp32-2432s028-full-2.bin
```

До совпадения SHA-256 статус резервной копии остаётся незавершённым.

## Практический урок

При повторяющихся `PermissionError(13)` и обрыве на одном и том же объёме данных сначала следует заменить USB-кабель на короткий заведомо исправный кабель передачи данных. Только после этого имеет смысл исследовать драйвер, скорость, адреса Flash и USB-UART.

## Официальные источники

- <https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/basic-commands.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/advanced-options.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/troubleshooting.html>
