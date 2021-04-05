/*
  Скетч к проекту "Многофункциональный RGB светильник"
  Страница проекта (схемы, описания): https://alexgyver.ru/GyverLamp/
  Исходники на GitHub: https://github.com/AlexGyver/GyverLamp/
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver, AlexGyver Technologies, 2019
  https://AlexGyver.ru/
*/

/*
  Версия 1.5.1
  - Оптимизировано обращение к серверу времени (нет подвисаний при отсутствии интернета)
  - Оптимизация под пины NodeMCU

  Версия 1.5.2
  - Исправлен незначительный баг с таймером
  - Исправлено падение по WDT при выводе IP
  - Исправлен баг с переназначением времени будильника
  - Исправлено переключение с первого на последний режимы
  - Приложение автоматически получает настройки с кнопки
  - Бегущая строка с текущим временем во время рассвета

  Версия 1.5.3
  - Увеличена плавность рассвета
  - Поправлен баг с отображением времени рассвета

  Версия 1.5.4
  - Поправлены глюки во время рассвета
  
  Версия 1.5.5
  - Поправлено невыключение света во время рассвета

*/

/*
  Forked by naya.vu
 */

// Все констаны и настройки перенесены в common.h

#include <Arduino.h>
#include <FastLED.h>

#include "constants.h"
#include "effects.h"
#include "leds.h"
#include "button.h"
#include "common.h"

boolean brightnessDirection;
int8_t brightnessLevel = 5; // 0-7

uint32_t effTimer;
boolean powerOn = false;

enum Effects {
    SPARKLES_EFFECT,
    FIRE_EFFECT,
    RAINBOW_VERTICAL_EFFECT,
    RAINBOW_HORIZONTAL_EFFECT,
    COLORS_EFFECT,
    SINGLE_COLOR_EFFECT,
    SNOW_EFFECT,
    MATRIX_EFFECT,
    LIGHTERS_EFFECT,
    MADNESS_NOISE_EFFECT,
    RAINBOW_NOISE_EFFECT,
    RAINBOW_STRIPE_NOISE_EFFECT,
    ZEBRA_NOISE_EFFECT,
    FOREST_NOISE_EFFECT,
    OCEAN_NOISE_EFFECT,
    PLASMA_NOISE_EFFECT,
    CLOUD_NOISE_EFFECT,
    LAVA_NOISE_EFFECT
};
#define NUM_EFFECTS 18

byte currentEffect = SPARKLES_EFFECT;
boolean initialiseEffect = true; // shows if it is a first iteration

inline void setBrightness() {
    FastLED.setBrightness(brightnessLevel * 32 + 31);
}

void effectsTick() {
    uint32_t now = millis();
    if (powerOn && timeDiff(now, effTimer) >= DEFAULT_SPEED) {
        effTimer = now;

        switch (currentEffect) {
            case SPARKLES_EFFECT:
                sparklesRoutine();
                break;
            case FIRE_EFFECT:
                fireRoutine(initialiseEffect);
                break;
            case RAINBOW_VERTICAL_EFFECT:
                rainbowVertical();
                break;
            case RAINBOW_HORIZONTAL_EFFECT:
                rainbowHorizontal();
                break;
            case COLORS_EFFECT:
                colorsRoutine();
                break;
            case SINGLE_COLOR_EFFECT:
                singleColorRoutine();
                break;
            case SNOW_EFFECT:
                snowRoutine();
                break;
            case MATRIX_EFFECT:
                matrixRoutine();
                break;
            case LIGHTERS_EFFECT:
                lightersRoutine(initialiseEffect);
                break;
            case MADNESS_NOISE_EFFECT:
                madnessNoise(initialiseEffect);
                break;
            case RAINBOW_NOISE_EFFECT:
                rainbowNoise(initialiseEffect);
                break;
            case RAINBOW_STRIPE_NOISE_EFFECT:
                rainbowStripeNoise(initialiseEffect);
                break;
            case ZEBRA_NOISE_EFFECT:
                zebraNoise(initialiseEffect);
                break;
            case FOREST_NOISE_EFFECT:
                forestNoise(initialiseEffect);
                break;
            case OCEAN_NOISE_EFFECT:
                oceanNoise(initialiseEffect);
                break;
            case PLASMA_NOISE_EFFECT:
                plasmaNoise(initialiseEffect);
                break;
            case CLOUD_NOISE_EFFECT:
                cloudNoise(initialiseEffect);
                break;
            case LAVA_NOISE_EFFECT:
                lavaNoise(initialiseEffect);
                break;
        }
        FastLED.show();
    }
    if (initialiseEffect) {
        initialiseEffect = false;
    }
}

void changePower() {
    powerOn = !powerOn;

    effectsTick();

    if (powerOn) {
        uint8_t brightness = brightnessLevel * 32 + 31;
        for (int i = 0; i < brightnessLevel; i += 8) {
            FastLED.setBrightness(i);
            delay(1);
            FastLED.show();
        }
        FastLED.setBrightness(brightness);
    } else {
        uint8_t brightness = brightnessLevel * 32 + 31;
        for (int i = brightness; i > 8; i -= 8) {
            FastLED.setBrightness(i);
            delay(1);
            FastLED.show();
        }
        FastLED.clear();
    }

    delay(2);
    FastLED.show();
}

void buttonTick() {
    if (!Button::hasEvent()) {
        return;
    }

    Button::Event event = Button::pullEvent();
    if (!event.type) {
        return;
    }

    if (event.type == Button::EVENT_CLICKS) {
        switch (event.value) {
            case 1:
                changePower();
                break;

            case 2:
                if (currentEffect == NUM_EFFECTS - 1) {
                    currentEffect = 0;
                } else {
                    currentEffect++;
                }
                initialiseEffect = true;
                FastLED.clear();
                delay(1);
                setBrightness();

                break;

            case 3:
                if (currentEffect == 0) {
                    currentEffect = NUM_EFFECTS - 1;
                } else {
                    currentEffect--;
                }
                initialiseEffect = true;
                FastLED.clear();
                delay(1);
                setBrightness();

                break;

            case 4:
                // save settings
                break;

            default:
                return;
        }
    } else if (event.type == Button::EVENT_HOLDING) {
        if (event.value == 1) {
            brightnessDirection = !brightnessDirection;
        }

        brightnessLevel += brightnessDirection ? 1 : -1;

        if (brightnessLevel >= 8) {
            brightnessLevel = 7;
            flash(2, 100);
        } else if (brightnessLevel < 0) {
            brightnessLevel = 0;
            flash(2, 100);
        }

        setBrightness();
    } else if (event.type == Button::EVENT_BOOT_HELD) {
        // reset EEPROM
        flash(5, 100);
    }
}

void setup() {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(DEFAULT_BRIGHTNESS);
    FastLED.show();

    if (CURRENT_LIMIT > 0) {
        FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
    }

    Button::setup(BTN_PIN);

    randomSeed(micros());
}

void loop() {
    effectsTick();

    Button::loop();

    buttonTick();
}
