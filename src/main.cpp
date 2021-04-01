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
#include <GyverButton.h>

#include "common.h"
#include "leds.h"
#include "effects.h"

GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);

boolean initialiseEffect = true;

byte brightness = 200;

boolean brightDirection;
uint32_t effTimer;
boolean ONflag = true;

int8_t currentMode = 0;

#define MODE_AMOUNT 18

void effectsTick() {
    if (ONflag && millis() - effTimer >= DEFAULT_SPEED) {
        effTimer = millis();
        switch (currentMode) {
            case 0: sparklesRoutine();
                break;
            case 1: fireRoutine(initialiseEffect);
                break;
            case 2: rainbowVertical();
                break;
            case 3: rainbowHorizontal();
                break;
            case 4: colorsRoutine();
                break;
            case 5: colorRoutine();
                break;
            case 6: snowRoutine();
                break;
            case 7: matrixRoutine();
                break;
            case 8: lightersRoutine(initialiseEffect);
                break;
            case 9: madnessNoise(initialiseEffect);
                break;
            case 10: rainbowNoise(initialiseEffect);
                break;
            case 11: rainbowStripeNoise(initialiseEffect);
                break;
            case 12: zebraNoise(initialiseEffect);
                break;
            case 13: forestNoise(initialiseEffect);
                break;
            case 14: oceanNoise(initialiseEffect);
                break;
            case 15: plasmaNoise(initialiseEffect);
                break;
            case 16: cloudNoise(initialiseEffect);
                break;
            case 17: lavaNoise(initialiseEffect);
                break;
        }
        FastLED.show();
    }
    if (initialiseEffect) {
        initialiseEffect = false;
    }
}

void changePower() {
    if (ONflag) {
        effectsTick();
        for (int i = 0; i < brightness; i += 8) {
            FastLED.setBrightness(i);
            delay(1);
            FastLED.show();
        }
        FastLED.setBrightness(brightness);
        delay(2);
        FastLED.show();
    } else {
        effectsTick();
        for (int i = brightness; i > 8; i -= 8) {
            FastLED.setBrightness(i);
            delay(1);
            FastLED.show();
        }
        FastLED.clear();
        delay(2);
        FastLED.show();
    }
}

void buttonTick() {
    touch.tick();
    if (touch.isSingle()) {
        if (ONflag) {
            ONflag = false;
            changePower();
        } else {
            ONflag = true;
            changePower();
        }
    }

    if (ONflag && touch.isDouble()) {
        if (++currentMode >= MODE_AMOUNT) currentMode = 0;
        FastLED.setBrightness(brightness);
        initialiseEffect = true;
        FastLED.clear();
        delay(1);
    }
    if (ONflag && touch.isTriple()) {
        if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
        FastLED.setBrightness(brightness);
        initialiseEffect = true;
        FastLED.clear();
        delay(1);
    }

    if (ONflag && touch.isHolded()) {
        brightDirection = !brightDirection;
    }
    if (ONflag && touch.isStep()) {
        if (brightDirection) {
            if (brightness < 10) brightness += 1;
            else if (brightness < 250) brightness += 5;
            else brightness = 255;
        } else {
            if (brightness > 15) brightness -= 5;
            else if (brightness > 1) brightness -= 1;
            else brightness = 1;
        }
        FastLED.setBrightness(brightness);
    }
}


void setup() {
    // ЛЕНТА

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.show();

    if (CURRENT_LIMIT > 0) {
        FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
    }

    touch.setStepTimeout(100);
    touch.setClickTimeout(500);
    randomSeed(micros());

}

void loop() {
    effectsTick();
    buttonTick();
}
