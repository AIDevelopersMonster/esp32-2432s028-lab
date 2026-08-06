# Установка и настройка TFT_eSPI для ESP32-2432S028

Эта инструкция нужна перед запуском примера:

```text
examples/01_display_test/01_display_test.ino
```

Одной установки библиотеки недостаточно: после установки нужно применить конфигурацию дисплея из этого репозитория.

## 1. Установить TFT_eSPI через Arduino IDE

В Arduino IDE откройте:

```text
Sketch → Include Library → Manage Libraries…
```

Либо нажмите значок **Library Manager** в левой панели Arduino IDE 2.x.

В строке поиска введите:

```text
TFT_eSPI
```

Найдите библиотеку:

```text
TFT_eSPI by Bodmer
```

Нажмите **Install**.

После установки библиотека обычно находится в каталоге sketchbook Arduino:

```text
C:\Users\CHUWI\Documents\Arduino\libraries\TFT_eSPI
```

Проверить наличие можно в PowerShell:

```powershell
Test-Path "C:\Users\CHUWI\Documents\Arduino\libraries\TFT_eSPI\TFT_eSPI.h"
```

Ожидаемый результат:

```text
True
```

## 2. Сохранить исходный User_Setup.h

TFT_eSPI хранит параметры контроллера и выводов внутри своей папки. Перед заменой конфигурации сохраните исходный файл:

```powershell
$library = "C:\Users\CHUWI\Documents\Arduino\libraries\TFT_eSPI"
Copy-Item "$library\User_Setup.h" "$library\User_Setup.h.backup" -Force
```

## 3. Применить конфигурацию нашей платы

В репозитории подготовлен файл:

```text
config\tft_espi\User_Setup.h
```

Скопируйте его поверх установленного `User_Setup.h`:

```powershell
$repo = "C:\Users\CHUWI\Documents\GitHub\esp32-2432s028-lab"
$library = "C:\Users\CHUWI\Documents\Arduino\libraries\TFT_eSPI"

Copy-Item "$repo\config\tft_espi\User_Setup.h" `
          "$library\User_Setup.h" -Force
```

Проверка:

```powershell
Get-Item "$library\User_Setup.h" |
    Select-Object FullName, Length, LastWriteTime
```

## 4. Что задаёт наш User_Setup.h

Для проверенной ESP32-2432S028 используются:

```text
Display driver: ILI9341
MISO: GPIO 12
MOSI: GPIO 13
SCLK: GPIO 14
CS: GPIO 15
DC: GPIO 2
RST: -1
Backlight: GPIO 21, управляется библиотекой CYD_Board
```

## 5. Перезапустить Arduino IDE

После установки TFT_eSPI и замены `User_Setup.h` полностью закройте Arduino IDE и запустите её снова.

## 6. Проверить компиляцию

Откройте:

```text
examples\01_display_test\01_display_test.ino
```

Выберите:

```text
Board: ESP32 Dev Module
Flash Frequency: 40 MHz
Flash Mode: DIO
Flash Size: 4 MB
Upload Speed: 115200
```

Сначала нажмите **Verify**. Ошибок вида:

```text
TFT_eSPI.h: No such file or directory
```

быть не должно.

Затем загрузите скетч на плату.

## 7. Ожидаемый результат

После загрузки на дисплее должны появиться:

- заголовок `ESP32-2432S028R`;
- текст `ILI9341 display test`;
- цветные поля;
- рамка;
- сообщение об успешной инициализации.

Подробный критерий проверки:

- [`../examples/01_display_test/README.md`](../examples/01_display_test/README.md)

## Важно при обновлении TFT_eSPI

Обновление TFT_eSPI через Library Manager может заменить `User_Setup.h`. После каждого обновления снова скопируйте:

```text
config\tft_espi\User_Setup.h
```

в каталог установленной библиотеки.

Официальный репозиторий TFT_eSPI также рекомендует хранить собственную конфигурацию отдельно, чтобы обновления библиотеки её не уничтожали. Для первого теста проекта используется простой и наглядный способ с заменой `User_Setup.h`.
