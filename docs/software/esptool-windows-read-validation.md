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

## Следующая проверка: область bootloader

Считать область с адреса `0x1000` два раза:

```powershell
python -m esptool --chip esp32 --port COM12 read-flash 0x1000 0x1000 bootloader-read-1.bin
python -m esptool --chip esp32 --port COM12 read-flash 0x1000 0x1000 bootloader-read-2.bin
```

Сравнить SHA-256:

```powershell
Get-FileHash .\bootloader-read-1.bin -Algorithm SHA256
Get-FileHash .\bootloader-read-2.bin -Algorithm SHA256
```

Побайтовое сравнение:

```powershell
cmd /c fc /b bootloader-read-1.bin bootloader-read-2.bin
```

После совпадения можно переходить к двум полным дампам по `0x400000` байт.