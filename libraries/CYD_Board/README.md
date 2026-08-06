# CYD_Board — установка и первая проверка

`CYD_Board` — локальная библиотека проекта для платы ESP32-2432S028 / Cheap Yellow Display. Она содержит именованную распиновку и небольшие функции для подсветки TFT, RGB-светодиода, фоторезистора LDR и кнопки BOOT.

Библиотека не устанавливается через Arduino Library Manager. Её нужно скопировать из репозитория вручную.

## Где находится библиотека в проекте

```text
esp32-2432s028-lab/
└── libraries/
    └── CYD_Board/
        ├── library.properties
        └── src/
            └── CYD_Board.h
```

## Установка в Arduino IDE под Windows

Исходный каталог:

```text
C:\Users\CHUWI\Documents\GitHub\esp32-2432s028-lab\libraries\CYD_Board
```

Скопируйте весь каталог `CYD_Board` в:

```text
C:\Users\CHUWI\Documents\Arduino\libraries\CYD_Board
```

В PowerShell это можно сделать так:

```powershell
$source = "C:\Users\CHUWI\Documents\GitHub\esp32-2432s028-lab\libraries\CYD_Board"
$target = "C:\Users\CHUWI\Documents\Arduino\libraries\CYD_Board"

Remove-Item $target -Recurse -Force -ErrorAction SilentlyContinue
Copy-Item $source $target -Recurse -Force
```

После копирования закройте и снова запустите Arduino IDE.

Правильная структура установленной библиотеки:

```text
C:\Users\CHUWI\Documents\Arduino\libraries\CYD_Board\library.properties
C:\Users\CHUWI\Documents\Arduino\libraries\CYD_Board\src\CYD_Board.h
```

Не должно получиться лишнего вложенного каталога вида:

```text
...\libraries\CYD_Board\CYD_Board\src\CYD_Board.h
```

## Проверка установки

Откройте в Arduino IDE основной тест библиотеки:

```text
examples\00_board_test\00_board_test.ino
```

Он подключает библиотеку строкой:

```cpp
#include <CYD_Board.h>
```

и вызывает:

```cpp
cyd::beginBasicHardware();
```

Если Arduino IDE компилирует скетч без ошибки:

```text
CYD_Board.h: No such file or directory
```

то библиотека найдена правильно.

## Проверенный результат теста 00

При корректной установке и загрузке `00_board_test`:

- RGB-светодиод перебирает красный, зелёный и синий цвета;
- подсветка TFT включена;
- в Serial Monitor на 115200 бод выводятся значения LDR и состояние кнопки BOOT.

Полная инструкция теста:

- [`examples/00_board_test/README.md`](../../examples/00_board_test/README.md)

После успешной проверки библиотеки переходите к дисплею:

- [`examples/01_display_test/README.md`](../../examples/01_display_test/README.md)

## Используемый интерфейс

```cpp
cyd::beginBasicHardware();
cyd::setBacklight(true);
cyd::setRgb(true, false, false);
cyd::rgbOff();
uint16_t light = cyd::readLightRaw();
```

## Обновление локальной копии

После изменений библиотеки в GitHub-репозитории повторите копирование каталога `libraries\CYD_Board` в каталог библиотек Arduino. Arduino IDE не обновляет эту локальную библиотеку автоматически.
