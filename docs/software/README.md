# Software / Firmware tools

Практический раздел по утилитам для чтения, проверки и записи прошивок ESP32.

## Windows CLI

- [`esptool-windows-cli.md`](esptool-windows-cli.md) — установка, обновление и базовые команды esptool в Windows CLI.
- [`esptool-windows-read-validation.md`](esptool-windows-read-validation.md) — проверенное двойное чтение flash и сравнение SHA-256.

## Видео

- [ESP32-2432S028: установка esptool и проверка чтения Flash — YouTube Shorts](https://youtube.com/shorts/8WonksEFenA)

В видео показан путь от установки `esptool` до получения `True` при сравнении SHA-256 двух независимо считанных областей bootloader.

## В разработке

- полный резервный дамп flash;
- Linux CLI;
- Espressif Flash Download Tool;
- проверка BIN;
- восстановление заводской прошивки.