# Установка XPT2046_Touchscreen для ESP32-2432S028

Эта инструкция нужна перед запуском примера:

```text
examples/02_touch_test/02_touch_test.ino
```

В проекте используется библиотека **XPT2046_Touchscreen by Paul Stoffregen** для резистивного контроллера XPT2046.

Официальный репозиторий библиотеки:

- <https://github.com/PaulStoffregen/XPT2046_Touchscreen>

Страница библиотеки в Arduino Documentation:

- <https://docs.arduino.cc/libraries/xpt2046_touchscreen/>

## 1. Установить через Arduino Library Manager

В Arduino IDE 2.x откройте **Library Manager** в левой панели либо:

```text
Sketch → Include Library → Manage Libraries…
```

В строке поиска введите:

```text
XPT2046_Touchscreen
```

Найдите библиотеку:

```text
XPT2046_Touchscreen by Paul Stoffregen
```

и нажмите **Install**.

Для этого проекта предпочтителен именно оригинальный пакет Paul Stoffregen, а не случайный одноимённый fork.

## 2. Проверить установку

Обычно библиотека устанавливается в каталог sketchbook Arduino:

```text
C:\Users\CHUWI\Documents\Arduino\libraries\XPT2046_Touchscreen
```

Проверка в PowerShell:

```powershell
Test-Path "C:\Users\CHUWI\Documents\Arduino\libraries\XPT2046_Touchscreen\XPT2046_Touchscreen.h"
```

Ожидаемый результат:

```text
True
```

После установки полностью закройте и снова запустите Arduino IDE.

## 3. Что подключает скетч

В `02_touch_test.ino` используются:

```cpp
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <CYD_Board.h>
```

Если библиотека не установлена, компилятор обычно сообщает:

```text
XPT2046_Touchscreen.h: No such file or directory
```

## 4. Отдельный SPI для тачскрина на нашей плате

Для проверенной ESP32-2432S028R линии XPT2046 заданы библиотекой `CYD_Board`:

| Сигнал | GPIO |
|---|---:|
| T_IRQ | 36 |
| MOSI | 32 |
| MISO | 39 |
| CLK | 25 |
| CS | 33 |

Они отличаются от SPI-линий TFT.

Скетч создаёт отдельный объект SPI:

```cpp
SPIClass touchSpi(VSPI);
```

и запускает его на линиях тачскрина:

```cpp
touchSpi.begin(cyd::TOUCH_CLK_PIN,
               cyd::TOUCH_MISO_PIN,
               cyd::TOUCH_MOSI_PIN,
               cyd::TOUCH_CS_PIN);

touch.begin(touchSpi);
```

Официальная библиотека поддерживает передачу другого SPI-порта в `begin(...)`, поэтому для нашего теста не требуется модифицировать исходный код `XPT2046_Touchscreen`.

## 5. Другие обязательные компоненты теста 02

До запуска touch-теста должны быть выполнены предыдущие шаги:

1. Установить и проверить нашу библиотеку [`CYD_Board`](../libraries/CYD_Board/README.md).
2. Установить и настроить [`TFT_eSPI`](tft-espi-installation.md).
3. Успешно выполнить [`01_display_test`](../examples/01_display_test/README.md).

## 6. Проверить компиляцию

Откройте:

```text
examples\02_touch_test\02_touch_test.ino
```

Выберите проверенный профиль Arduino IDE:

```text
Board: ESP32 Dev Module
CPU Frequency: 240 MHz
Flash Frequency: 40 MHz
Flash Mode: DIO
Flash Size: 4 MB
Upload Speed: 115200
```

Сначала нажмите **Verify**.

Не должно быть ошибок:

```text
XPT2046_Touchscreen.h: No such file or directory
CYD_Board.h: No such file or directory
TFT_eSPI.h: No such file or directory
```

После успешной компиляции загрузите скетч на плату.

## 7. Ожидаемый результат

Откройте Serial Monitor на:

```text
115200 baud
```

При касании должны появляться сырые значения:

```text
x=1234 y=2870 z=560
```

Те же X/Y/Z выводятся на TFT.

Это пока **raw test**: значения не преобразованы в экранные координаты 0…319 и 0…239 и не откалиброваны.

Подробный критерий проверки:

- [`examples/02_touch_test/README.md`](../examples/02_touch_test/README.md)

## Ручная установка — только как резервный вариант

Если Library Manager недоступен, библиотеку можно скачать из официального репозитория Paul Stoffregen и установить ZIP через Arduino IDE:

```text
Sketch → Include Library → Add .ZIP Library…
```

Но для обычной установки в этом проекте используем Library Manager: так проще повторить процедуру и получить корректную структуру библиотеки.
