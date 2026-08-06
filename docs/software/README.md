# Software / Firmware tools

Практический раздел по утилитам для чтения, проверки и записи прошивок ESP32.

## Windows CLI

- [`esptool-windows-cli.md`](esptool-windows-cli.md) — установка, обновление и базовые команды esptool в Windows CLI.
- [`esptool-windows-read-validation.md`](esptool-windows-read-validation.md) — проверенное двойное чтение flash и сравнение SHA-256.
- [`esptool-windows-full-backup.md`](esptool-windows-full-backup.md) — создание двух полных резервных дампов Flash.
- [`esptool-windows-recovery-validation.md`](esptool-windows-recovery-validation.md) — проверенное восстановление платы полным образом 4 MB через USB-UART.
- [`esptool-read-flash-diagnostics.md`](esptool-read-flash-diagnostics.md) — разбор ошибок pySerial, кабеля и подтверждённого полного дампа.
- [`esp32-efuse-audit.md`](esp32-efuse-audit.md) — аудит eFuse, доступность UART ROM download mode и условия восстановления без внешнего программатора.
- [`esp32-flash-partitions.md`](esp32-flash-partitions.md) — bootloader, таблица разделов, application, NVS, OTA и файловые разделы.

## Видео

- [ESP32-2432S028: установка esptool и проверка чтения Flash — YouTube Shorts](https://youtube.com/shorts/8WonksEFenA)

В видео показан путь от установки `esptool` до получения `True` при сравнении SHA-256 двух независимо считанных областей bootloader.

## В разработке

- расшифровка реальной таблицы разделов исследуемой платы;
- Linux CLI;
- Espressif Flash Download Tool;
- проверка BIN.
