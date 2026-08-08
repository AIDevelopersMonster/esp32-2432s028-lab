# 05 — Bluetooth Classic SPP

## Статус

**ПРОВЕРЕНО: РАБОТАЕТ**

Дата проверки: **08.08.2026**

Видеоотчёт:

- https://youtube.com/shorts/fpp9V5QYG7o

Фактический запуск на ESP32-2432S028R подтверждён пользователем.

По фото- и видеоотчёту подтверждаются:

- устройство `CYD-BT-CLASSIC` обнаружено и подключено в Bluetooth SPP terminal;
- ESP32 регулярно передаёт heartbeat-сообщение;
- период отправки примерно 5 секунд;
- сообщение `[CYD] Bluetooth Classic SPP alive` стабильно приходит на телефон;
- отправленный с телефона символ `t` был принят ESP32 и возвращён обратно как echo;
- двусторонний SPP-канал фактически работает.

Итоговый статус лабораторного теста:

```
TEST_05_BLUETOOTH_CLASSIC_PASS
BT_CLASSIC_START_CONFIRMED
BT_CLASSIC_DEVICE_DISCOVERED
BT_CLASSIC_SPP_CONNECTION_CONFIRMED
BT_CLASSIC_TX_CONFIRMED
BT_CLASSIC_RX_CONFIRMED
BT_CLASSIC_ECHO_CONFIRMED
```

## Фото результата

![Результат теста 05 — Bluetooth Classic SPP](test-05-bluetooth-classic-result.svg)

Файл `test-05-bluetooth-classic-result.svg` — минимальная заглушка. Его можно заменить локально реальной фотографией результата, сохранив то же имя файла; ссылка в README останется рабочей.

Файл программы: [`05_bluetooth_classic_test.ino`](05_bluetooth_classic_test.ino)

## Что проверяет пример

Тест использует встроенный Bluetooth Classic оригинального ESP32 и профиль SPP (Serial Port Profile).

Он проверяет:

- запуск Bluetooth Classic;
- публикацию устройства с именем `CYD-BT-CLASSIC`;
- подключение телефона или компьютера по SPP;
- передачу данных из Arduino Serial Monitor в Bluetooth;
- передачу данных из Bluetooth в USB Serial;
- возврат полученных Bluetooth-данных обратно клиенту;
- периодическую передачу heartbeat-сообщения.

Никакой внешний Bluetooth-модуль и GPIO-подключения не требуются.

## Требования

- установлен пакет `esp32 by Espressif Systems`;
- выбрана плата **ESP32 Dev Module**;
- используются проверенные настройки из [`docs/arduino-ide.md`](../../docs/arduino-ide.md);
- отдельную библиотеку устанавливать не нужно: `BluetoothSerial` входит в ESP32 Arduino core;
- нужен телефон или компьютер с поддержкой Bluetooth Classic SPP и терминальное приложение.

Официальная документация Arduino ESP32 по Bluetooth Classic:

- <https://docs.espressif.com/projects/arduino-esp32/en/latest/api/bluetooth.html>

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

1. Откройте `05_bluetooth_classic_test.ino`.
2. Нажмите **Verify**.
3. Нажмите **Upload**.
4. Откройте Serial Monitor на `115200` бод.
5. На телефоне или компьютере найдите Bluetooth-устройство:

```
CYD-BT-CLASSIC
```

6. Подключитесь к нему через приложение, поддерживающее Bluetooth Classic SPP.

После старта в Serial Monitor ожидается:

```
ESP32-2432S028R Bluetooth Classic SPP test
Starting Bluetooth device: CYD-BT-CLASSIC
CYD_BT_CLASSIC_READY
```

## Проверка направления ESP32 -> телефон/ПК

После подключения клиент должен примерно раз в 5 секунд получать:

```
[CYD] Bluetooth Classic SPP alive
```

На реальной плате это подтверждено фото- и видеоотчётом.

## Проверка направления телефон/ПК -> ESP32

В Bluetooth-терминале отправьте, например:

```
hello from bluetooth
```

Та же строка должна:

- появиться в Arduino Serial Monitor;
- вернуться в Bluetooth-терминал как echo.

В фактическом тесте был отправлен символ:

```
t
```

и терминал показал его возврат, что подтверждает RX + echo.

## Проверка USB Serial -> Bluetooth

Введите в Arduino Serial Monitor, например:

```
hello from usb serial
```

Строка должна появиться в подключённом Bluetooth SPP-терминале.

## Критерий PASS

Пример считается полностью подтверждённым, если одновременно выполнены условия:

```
BT_CLASSIC_START_CONFIRMED
BT_CLASSIC_DEVICE_DISCOVERED
BT_CLASSIC_SPP_CONNECTION_CONFIRMED
BT_CLASSIC_TX_CONFIRMED
BT_CLASSIC_RX_CONFIRMED
BT_CLASSIC_ECHO_CONFIRMED
```

Для проверенной ESP32-2432S028R эти условия выполнены.

## Важное ограничение

Bluetooth Classic SPP поддерживается не всеми телефонами одинаково. На некоторых мобильных ОС доступ к произвольному SPP-клиенту может быть ограничен. В таком случае удобнее использовать компьютер или Android-устройство с SPP-терминалом.

Это не является проблемой ESP32, если устройство видно и другой совместимый SPP-клиент подключается успешно.

## Типичные ошибки

### `Bluetooth is not enabled...`

Проверьте, что выбрана именно обычная ESP32-совместимая цель, а не ESP32-C3/S3/C6, где Bluetooth Classic SPP недоступен.

### Устройство не видно

- перезапустите ESP32;
- выключите/включите Bluetooth на телефоне;
- убедитесь, что ищете именно `CYD-BT-CLASSIC`;
- попробуйте другой телефон или компьютер.

### Устройство видно, но терминал не подключается

Проверьте, что приложение действительно поддерживает Bluetooth Classic SPP, а не только BLE.

## Следующий пример

После успешного прохождения этого теста переходите к [`06_ble_test`](../06_ble_test/README.md).
