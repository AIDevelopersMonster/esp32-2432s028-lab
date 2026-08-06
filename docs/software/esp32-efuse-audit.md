# Аудит eFuse исследуемой ESP32-2432S028

Снимок получен командой:

```powershell
python -m espefuse --chip esp32 --port COM12 summary > esp32-efuse-summary.txt
```

Утилита: `espefuse v5.3.1`.

> Важно: этот документ описывает только чтение eFuse. Команды `burn-*` необратимо изменяют кристалл и без отдельной обоснованной задачи применяться не должны.

## Главный результат

Исследуемая плата сохраняет штатную возможность восстановления Flash через встроенный USB-UART и ROM download mode.

```text
UART_DOWNLOAD_DIS = False
FLASH_CRYPT_CNT = 0
ABS_DONE_0 = False
ABS_DONE_1 = False
DISABLE_DL_ENCRYPT = False
DISABLE_DL_DECRYPT = False
```

Это означает:

- UART download mode не отключён;
- Flash Encryption не включено;
- Secure Boot V1 и V2 не включены;
- UART-загрузчику не запрещены операции шифрования и дешифрования;
- полный подтверждённый образ Flash можно восстановить через `esptool` и USB-UART без внешнего программатора, пока исправны ESP32, USB-UART, питание, линии `EN/GPIO0` и сама SPI Flash.

## Защита и доступ

```text
WR_DIS = 0
RD_DIS = 0
JTAG_DISABLE = False
CONSOLE_DEBUG_DISABLE = True
DISABLE_DL_CACHE = False
```

Интерпретация:

- маска запрета записи eFuse не установлена;
- чтение блоков eFuse 1–3 не запрещено;
- JTAG аппаратно не отключён;
- ROM BASIC fallback отключён, что не мешает UART download mode;
- Flash cache в UART bootloader не отключён.

## Ключевые блоки

```text
BLOCK1 flash encryption key = all zero
BLOCK2 secure boot key      = all zero
BLOCK3 variable block       = all zero
SECURE_VERSION              = 0
KEY_STATUS                  = False
```

На момент чтения криптографические ключи Secure Boot и Flash Encryption не запрограммированы, anti-rollback не активирован.

## Идентификация кристалла

```text
ESP32 revision: 3
Package version: 1
CPU frequency rated: true
Maximum normal CPU frequency: 240 MHz
Coding scheme: NONE
ADC_VREF: 1142 mV
```

MAC-адрес в публичной документации маскируется:

```text
40:22:d8:xx:xx:xx
```

CRC заводского MAC в исходном отчёте прошёл проверку.

## Питание SPI Flash

В отчёте указано:

```text
XPD_SDIO_FORCE = False
```

Поэтому напряжение `VDD_SDIO` определяется состоянием `GPIO12` при сбросе:

```text
GPIO12 High  -> 1.8 V
GPIO12 Low/NC -> 3.3 V
```

Для исследуемой платы `flash-id` ранее определил Flash как 3.3 V. При собственных схемах и внешнем управлении GPIO12 нельзя случайно подтягивать его вверх во время сброса.

## Статус восстановления

```text
UART_ROM_DOWNLOAD_AVAILABLE
FLASH_ENCRYPTION_DISABLED
SECURE_BOOT_DISABLED
JTAG_AVAILABLE
EFUSE_READ_WRITE_PROTECTION_NOT_SET
FACTORY_FLASH_BACKUP_VALIDATED
EXTERNAL_PROGRAMMER_NOT_REQUIRED_FOR_NORMAL_RECOVERY
```

## Граница вывода

Отчёт eFuse подтверждает доступность ROM download mode и отсутствие включённых защитных механизмов. Он не проверяет физическое состояние USB-UART, контактов, разъёма, SPI Flash и линий автоматического сброса. При аппаратной неисправности может потребоваться внешний USB-UART адаптер, доступ к `EN/GPIO0/U0RXD/U0TXD` или непосредственный программатор SPI Flash.
