# 06 — Bluetooth Low Energy (BLE)

## Статус

**ПРОВЕРЕНО: РАБОТАЕТ**

Дата проверки: **08.08.2026**

Фактический запуск на ESP32-2432S028R выполнен успешно. Для проверки использовалось приложение **nRF Connect**.

По представленному Serial-логу непосредственно подтверждаются:

- запуск BLE и публикация устройства `CYD-BLE`;
- создание GATT service и characteristic;
- подключение BLE-клиента;
- длительная стабильная передача notifications от ESP32 к клиенту;
- приём записи `hello` от клиента;
- отправка ACK через notify;
- приём второй записи `test ble`;
- продолжение notifications после операций записи.

Фрагмент фактического результата:

```
ESP32-2432S028R BLE GATT test
Starting BLE device: CYD-BLE
CYD_BLE_READY
BLE_CLIENT_CONNECTED
BLE TX: CYD BLE counter=1
...
BLE RX: hello
BLE_WRITE_ACK_NOTIFIED
...
BLE RX: test ble
BLE_WRITE_ACK_NOTIFIED
```

Счётчик notifications в предоставленном логе дошёл как минимум до:

```
BLE TX: CYD BLE counter=111
```

Это подтверждает не только единичную передачу, но и устойчивую работу канала в течение продолжительного теста.

Сообщение:

```
BLE_WRITE_EMPTY
```

не является ошибкой BLE-стека: оно означает, что клиент выполнил запись нулевой длины. После этого соединение продолжило нормально работать, notifications продолжились, а следующие записи `hello` и `test ble` были успешно приняты.

Итоговый статус лабораторного теста:

```
TEST_06_BLE_PASS
BLE_START_CONFIRMED
BLE_ADVERTISING_CONFIRMED
BLE_DEVICE_DISCOVERED
BLE_CONNECTION_CONFIRMED
BLE_WRITE_CONFIRMED
BLE_NOTIFY_CONFIRMED
BLE_ACK_NOTIFY_CONFIRMED
BLE_LONG_RUNNING_TX_CONFIRMED
```

READ и переподключение не создают обязательных строк в данном фрагменте Serial-лога; полный тест в nRF Connect подтверждён пользователем как успешно пройденный.

Файл программы: [`06_ble_test.ino`](06_ble_test.ino)

## Что проверяет пример

Тест использует встроенный BLE оригинального ESP32 и создаёт простейший GATT-сервер.

Проверяются:

- запуск BLE;
- advertising устройства `CYD-BLE`;
- обнаружение платы телефоном/ПК;
- подключение BLE-клиента;
- чтение characteristic;
- запись данных клиентом в characteristic;
- получение этих данных ESP32 и вывод в USB Serial;
- отправка ACK-ответа через notify;
- периодические notifications от ESP32 к клиенту.

Никакой внешний BLE-модуль и GPIO-подключения не требуются.

## Требования

- установлен пакет `esp32 by Espressif Systems`;
- выбрана плата **ESP32 Dev Module**;
- используются проверенные настройки из [`docs/arduino-ide.md`](../../docs/arduino-ide.md);
- отдельную BLE-библиотеку устанавливать не нужно: используемые `BLEDevice`, `BLEServer`, `BLEUtils`, `BLE2902` входят в ESP32 Arduino core;
- нужен телефон или компьютер с BLE/GATT-клиентом.

Для фактической проверки использовалось приложение **nRF Connect**.

Официальная документация Arduino ESP32 по BLE:

- <https://docs.espressif.com/projects/arduino-esp32/en/latest/api/ble.html>

## Проверенные настройки Arduino IDE для лаборатории

```
Board: ESP32 Dev Module
CPU Frequency: 240 MHz
Flash Frequency: 40 MHz
Flash Mode: DIO
Flash Size: 4 MB
PSRAM: Disabled
Upload Speed: 115200
Serial Monitor: 115200 baud
```

## Запуск

1. Откройте `06_ble_test.ino`.
2. Нажмите **Verify**.
3. Нажмите **Upload**.
4. Откройте Serial Monitor на `115200` бод.
5. Запустите **nRF Connect** или другой BLE/GATT-клиент.
6. Выполните BLE scan.
7. Найдите устройство:

```
CYD-BLE
```

После старта в Serial Monitor ожидается:

```
ESP32-2432S028R BLE GATT test
Starting BLE device: CYD-BLE
CYD_BLE_READY
```

Также скетч выводит UUID сервиса и характеристики.

## UUID теста

Service UUID:

```
4fafc201-1fb5-459e-8fcc-c5c9c331914b
```

Characteristic UUID:

```
beb5483e-36e1-4688-b7f5-ea07361b26a8
```

Characteristic поддерживает:

```
READ
WRITE
NOTIFY
```

## Проверка чтения

После подключения откройте characteristic и выполните READ.

Начальное значение:

```
CYD BLE ready
```

## Проверка ESP32 -> телефон/ПК

В BLE-клиенте включите notifications для characteristic.

Примерно каждые 5 секунд должны приходить сообщения:

```
CYD BLE counter=1
CYD BLE counter=2
CYD BLE counter=3
```

В Serial Monitor одновременно выводится:

```
BLE TX: CYD BLE counter=1
```

В фактическом тесте notifications устойчиво работали как минимум до значения счётчика 111.

## Проверка телефон/ПК -> ESP32

Запишите в characteristic, например:

```
hello
```

В Serial Monitor должно появиться:

```
BLE RX: hello
BLE_WRITE_ACK_NOTIFIED
```

В фактическом тесте успешно приняты как минимум две строки:

```
hello
test ble
```

После каждой записи скетч сформировал ACK и вызвал notify.

## Переподключение

После отключения клиента скетч снова запускает advertising. В Serial Monitor ожидается:

```
BLE_CLIENT_DISCONNECTED
BLE_ADVERTISING_RESTARTED
```

После этого к `CYD-BLE` можно подключиться снова без перезагрузки платы.

## Критерий PASS

Для базового подтверждения BLE/GATT должны быть выполнены:

```
BLE_START_CONFIRMED
BLE_ADVERTISING_CONFIRMED
BLE_DEVICE_DISCOVERED
BLE_CONNECTION_CONFIRMED
BLE_WRITE_CONFIRMED
BLE_NOTIFY_CONFIRMED
BLE_ACK_NOTIFY_CONFIRMED
```

Для текущей ESP32-2432S028R эти условия выполнены.

READ и повторное подключение остаются отдельными функциональными проверками интерфейса клиента; пользователь сообщил, что тест в nRF Connect пройден полностью.

## Типичные проблемы

### `CYD-BLE` не видно

- перезапустите ESP32;
- убедитесь, что приложение выполняет именно BLE scan;
- выключите/включите Bluetooth на телефоне;
- попробуйте другой BLE-клиент.

### Устройство видно, но не удаётся найти characteristic

Убедитесь, что открыт сервис с UUID:

```
4fafc201-1fb5-459e-8fcc-c5c9c331914b
```

### Notifications не приходят

В BLE/GATT-приложении отдельно включите notifications/subscribe для characteristic. Само подключение к устройству обычно не включает notify автоматически.

## Рекомендуемое приложение

Фактически проверено:

```
nRF Connect
```

Подойдёт и другой BLE/GATT-клиент, который умеет scan, connect, read, write и subscribe/notify.

## Предыдущий пример

Для проверки второго Bluetooth-режима ESP32 сначала можно пройти [`05_bluetooth_classic_test`](../05_bluetooth_classic_test/README.md).
