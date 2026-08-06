# Диагностика нестабильного `read-flash`

Проверено на ESP32-2432S028 с `ESP32-D0WD-V3`, Flash 4 MB и `esptool v5.3.1` под Windows.

## Ключевое исправление вывода

Один и тот же диапазон `0x2100–0x210F` сначала многократно давал `PermissionError(13)`, а затем был успешно считан той же командой:

```powershell
python -m esptool --chip esp32 --port COM12 read-flash 0x2100 0x10 test-2100-0010.bin
```

Успешный результат:

```text
Read 16 bytes from 0x00002100 ...
Hard resetting via RTS pin...
```

Поэтому прежний вывод об адресно-зависимой или содержательно-зависимой ошибке отменён.

```text
ADDRESS_SPECIFIC_FAILURE_NOT_CONFIRMED
INTERMITTENT_SERIAL_OR_RESET_FAILURE_CONFIRMED
```

## Что подтверждено

- Flash имеет объём 4 MB.
- `verify-flash` обращается к ранее «проблемным» областям и штатно возвращает `digest mismatch`.
- Один и тот же адрес может как завершиться ошибкой pySerial, так и успешно считаться.
- При ошибке плата иногда остаётся с погашенным дисплеем и слабым красным свечением RGB.
- После успешной команды с `Hard resetting via RTS pin...` пользовательская программа запускается снова.

## Наиболее осторожная интерпретация

Сбой относится не к конкретному адресу Flash, а к нестабильности цепочки:

```text
Windows / pySerial / USB-UART / auto-reset RTS-DTR / serial bootloader
```

После ошибки ESP32, вероятно, остаётся в serial bootloader или промежуточном состоянии, поэтому дисплей погашен и пользовательская программа не работает. Это не доказательство зависания заводской программы.

## Следующая проверка повторяемости

Запустить одну и ту же короткую команду 10 раз и посчитать успешные и неуспешные попытки:

```powershell
1..10 | ForEach-Object {
    $file = "repeat-2100-$_.bin"
    Remove-Item $file -ErrorAction SilentlyContinue
    python -m esptool --chip esp32 --port COM12 read-flash 0x2100 0x10 $file
    [PSCustomObject]@{
        Attempt = $_
        ExitCode = $LASTEXITCODE
        FileExists = Test-Path $file
        Length = if (Test-Path $file) { (Get-Item $file).Length } else { 0 }
    }
}
```

Затем повторить то же для контрольного адреса `0x2110`.

До получения статистики повторяемости не делать выводов о дефектном секторе, защищённом диапазоне или особом содержимом Flash.

## Статус

```text
FLASH_SIZE_CONFIRMED_4MB
REGION_0x2100_SUCCESSFULLY_READ
PREVIOUS_ADDRESS_DEPENDENCE_RETRACTED
INTERMITTENT_PYSERIAL_FAILURE
AUTO_RESET_OR_USB_SERIAL_PATH_SUSPECTED
FULL_BACKUP_NOT_YET_VALIDATED
ROOT_CAUSE_NOT_YET_ESTABLISHED
```

## Официальные источники

- <https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/basic-commands.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/advanced-options.html>
- <https://docs.espressif.com/projects/esptool/en/latest/esp32/troubleshooting.html>
