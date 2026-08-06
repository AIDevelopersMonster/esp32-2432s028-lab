# Проверка чтения flash в Windows

Проверено на реальной плате через `esptool v5.3.1`, порт `COM12`.

## Исходные данные

- MCU: `ESP32-D0WD-V3`, revision 3.0;
- flash: 4 MB;
- JEDEC manufacturer ID: `0xEF`;
- JEDEC device ID: `0x4016`;
- напряжение flash: 3,3 В.

MAC-адрес и имя пользователя Windows в документации не сохраняются.

## Два чтения области 0x0000–0x0FFF

```powershell
python -m esptool --chip esp32 --port COM12 read-flash 0x0 0x1000 test-read.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x0 0x1000 test-read1.bin
```

Оба чтения завершились успешно:

```text
Read 4096 bytes from 0x00000000 ...
Hard resetting via RTS pin...
```

Проверка загруженных файлов:

| Файл | Размер | SHA-256 |
|---|---:|---|
| `test-read.bin` | 4096 байт | `f47a8ec3e9aff2318d896942282ad4fe37d6391c82914f54a5da8a37de1300c6` |
| `test-read1.bin` | 4096 байт | `f47a8ec3e9aff2318d896942282ad4fe37d6391c82914f54a5da8a37de1300c6` |

Результат:

```text
FILES_IDENTICAL
READ_PATH_STABLE
```

Все 4096 байт в обоих файлах равны `0xFF`. Для типовой разметки классического ESP32 область `0x0000–0x0FFF` может быть незаполненной, поскольку bootloader обычно начинается с адреса `0x1000`.

Строка прогресса вида:

```text
Reading from 0x00001000 ...
```

не означает, что чтение началось с `0x1000`: это отображение достигнутой верхней границы. Итоговая строка правильно сообщает начало чтения `0x00000000`.

## Два чтения области bootloader 0x1000–0x1FFF

Выполнены команды:

```powershell
python -m esptool --chip esp32 --port COM12 read-flash 0x1000 0x1000 bootloader-read-1.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x1000 0x1000 bootloader-read-2.bin
```

Оба чтения завершились успешно:

```text
Read 4096 bytes from 0x00001000 in 0.4 seconds ...
Hard resetting via RTS pin...
```

PowerShell показал одинаковое начало SHA-256 для обоих файлов:

```text
05175BB485DB108064E390F9A172705ECF69B105F5314196DBD...
```

Поскольку табличный вывод PowerShell обрезал конец строки, для строгого подтверждения полного совпадения выполнить одну из команд:

```powershell
(Get-FileHash .\bootloader-read-1.bin -Algorithm SHA256).Hash
(Get-FileHash .\bootloader-read-2.bin -Algorithm SHA256).Hash
```

или сразу получить логический результат:

```powershell
(Get-FileHash .\bootloader-read-1.bin -Algorithm SHA256).Hash -eq (Get-FileHash .\bootloader-read-2.bin -Algorithm SHA256).Hash
```

Ожидаемый ответ:

```text
True
```

Дополнительная побайтовая проверка:

```powershell
cmd /c fc /b bootloader-read-1.bin bootloader-read-2.bin
```

Ожидаемый ответ:

```text
FC: no differences encountered
```

Текущий статус:

```text
BOOTLOADER_READ_SUCCESS
HASH_PREFIX_MATCH
FULL_HASH_CONFIRMATION_PENDING
```

## Следующий этап: полный дамп 4 MB

После получения `True` можно считать полный образ дважды:

```powershell
python -m esptool --chip esp32 --port COM12 --baud 460800 read-flash 0x0 0x400000 esp32-2432s028-full-1.bin
python -m esptool --chip esp32 --port COM12 --baud 460800 read-flash 0x0 0x400000 esp32-2432s028-full-2.bin
```

Если на скорости `460800` появляются ошибки, повторить на `115200`:

```powershell
python -m esptool --chip esp32 --port COM12 --baud 115200 read-flash 0x0 0x400000 esp32-2432s028-full-1.bin
python -m esptool --chip esp32 --port COM12 --baud 115200 read-flash 0x0 0x400000 esp32-2432s028-full-2.bin
```

Проверить размер и SHA-256:

```powershell
Get-Item .\esp32-2432s028-full-1.bin, .\esp32-2432s028-full-2.bin | Select-Object Name, Length
(Get-FileHash .\esp32-2432s028-full-1.bin -Algorithm SHA256).Hash
(Get-FileHash .\esp32-2432s028-full-2.bin -Algorithm SHA256).Hash
(Get-FileHash .\esp32-2432s028-full-1.bin -Algorithm SHA256).Hash -eq (Get-FileHash .\esp32-2432s028-full-2.bin -Algorithm SHA256).Hash
```

Ожидаемый размер каждого полного дампа:

```text
4194304 bytes
```

До получения двух совпадающих полных дампов не выполнять `erase-flash` и `write-flash`.