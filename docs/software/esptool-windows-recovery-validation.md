# Проверенное восстановление полного образа Flash

Проверено на ESP32-2432S028 с `ESP32-D0WD-V3`, Flash 4 MB, встроенным USB-UART и `esptool v5.3.1` под Windows.

## Исходное аварийное состояние

После неудачной записи плата запускала bootloader, но не находила исправного приложения:

```text
Checksum failed
OTA app partition slot 0 is not bootable
image at 0x150000 has invalid magic byte
OTA app partition slot 1 is not bootable
No bootable app partitions in the partition table
```

Это подтвердило, что ROM download mode и bootloader доступны, но application-раздел повреждён или записан неполно.

## Использованный резервный образ

Для восстановления использован один из двух ранее считанных и побайтно совпавших полных образов:

```text
esp32-2432s028-full-1.bin
esp32-2432s028-full-2.bin
```

Параметры каждого файла:

```text
Размер: 4194304 байта
SHA-256: A8A670376F6526B64214B3CAF798A0EEB7E8B9C46C5C2C442E02FBD519391021
```

## Команда восстановления

Полный образ записывается с адреса `0x0`:

```powershell
python -m esptool --chip esp32 --port COM12 --baud 115200 `
  write-flash 0x0 .\esp32-2432s028-full-1.bin
```

Отдельная команда `erase-flash` перед этим не требуется: `write-flash` стирает необходимые сектора перед записью.

## Подтверждённый результат

После записи полного образа и аппаратного сброса исходная прошивка снова загрузилась и работа платы была восстановлена.

```text
FULL_4MB_FLASH_IMAGE_WRITTEN_FROM_OFFSET_0
ROM_UART_RECOVERY_CONFIRMED
FACTORY_FIRMWARE_BOOT_CONFIRMED
DISPLAY_AND_APPLICATION_OPERATION_CONFIRMED
EXTERNAL_PROGRAMMER_NOT_REQUIRED
RECOVERY_PROCEDURE_VALIDATED
```

Это экспериментально подтверждает, что для данной платы исправный полный дамп 4 MB можно использовать для восстановления через встроенный USB-UART даже после повреждения application-разделов.

## Ограничения

Этот результат относится к исследованной плате и её собственному дампу. Полный образ другой платы может содержать отличающиеся NVS, OTA data, настройки, идентификаторы или другую таблицу разделов.

Не публиковать полный дамп без проверки на сетевые настройки, пароли, токены и другие уникальные данные.
