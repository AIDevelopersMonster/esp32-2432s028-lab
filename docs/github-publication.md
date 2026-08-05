# Публикация на GitHub

## Рекомендуемые параметры

- Repository name: `esp32-2432s028-lab`
- Description: `Examples, pinout and practical projects for the ESP32-2432S028R Cheap Yellow Display (CYD)`
- Visibility: public, если проект предназначен для публикации; private — для предварительной проверки
- Default branch: `main`
- Licence: MIT

## Topics

```text
esp32
esp32-2432s028
esp32-2432s028r
cheap-yellow-display
cyd
ili9341
xpt2046
arduino
platformio
tft-espi
embedded
iot
touchscreen
```

## Первая публикация

```bash
git init -b main
git add .
git commit -m "Initial ESP32-2432S028R lab"
git remote add origin https://github.com/AIDevelopersMonster/esp32-2432s028-lab.git
git push -u origin main
```

Если репозиторий создаётся через GitHub CLI:

```bash
gh repo create AIDevelopersMonster/esp32-2432s028-lab \
  --public \
  --source=. \
  --remote=origin \
  --push
```

Параметр `--public` можно заменить на `--private`.

## Рекомендуемый первый релиз

- Tag: `v0.1.0`
- Title: `ESP32-2432S028R Lab v0.1.0`
- Notes: базовая структура, самотест платы, TFT, touch, microSD и Wi-Fi/NTP.
