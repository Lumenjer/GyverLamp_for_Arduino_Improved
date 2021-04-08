/*
 * Облегчённая и оптимизированная прошивка GyverLamp для Arduino
 * Изменено и доработано: NayaVu 2021, https://naya.vu/
 *
 * Это форк прошивки от x-plora (https://github.com/x-plora/GyverLamp_for_Arduino),
 * который в свою очередь является форком прошивки от Norovl (https://github.com/Norovl/GyverLamp_for_Arduino),
 * которая является форком оригинальной прошивки от  AlexGyver (https://github.com/AlexGyver/GyverLamp/)
 *
 * Автор: AlexGyver, AlexGyver Technologies, 2019
 * https://AlexGyver.ru/
 */

// Все констаны и настройки перенесены в common.h

#include <Arduino.h>
#include <FastLED.h>
#include <EEPROM.h>
#include <InterruptDrivenButton.h>

#include "constants.h"
#include "effects.h"
#include "leds.h"
#include "common.h"

#define SPARKLES_EFFECT 0
#define FIRE_EFFECT 1
#define RAINBOW_VERTICAL_EFFECT 2
#define RAINBOW_HORIZONTAL_EFFECT 3
#define COLORS_EFFECT 4
#define SINGLE_COLOR_EFFECT 5
#define SNOW_EFFECT 6
#define MATRIX_EFFECT 7
#define LIGHTERS_EFFECT 8
#define MADNESS_NOISE_EFFECT 9
#define RAINBOW_NOISE_EFFECT 10
#define RAINBOW_STRIPE_NOISE_EFFECT 11
#define ZEBRA_NOISE_EFFECT 12
#define FOREST_NOISE_EFFECT 13
#define OCEAN_NOISE_EFFECT 14
#define PLASMA_NOISE_EFFECT 15
#define CLOUD_NOISE_EFFECT 16
#define LAVA_NOISE_EFFECT 17

#define NUM_EFFECTS 18

static uint32_t effTimer;

static byte currentEffect = SPARKLES_EFFECT;
static boolean initialiseEffect = true; // shows if it is a first iteration
static boolean brightnessDirection;
static boolean powerOn = false;


// 1, 2, 3 bits - brightness (possible values: 1-8)
// 4, 5, 6, 7 bits - effect runtime value (mostly speed, 1-16)
// 8th bit - 1 if the whole parameter is loaded from EEPROM, 0 if default

static uint8_t brightnessLevels[NUM_EFFECTS] = {};

static InterruptDrivenButton button(BTN_PIN);
DEFINE_IDB_ISR(button)

#define NUM_LEVELS 8
static const uint8_t brightnessLevelMapping[NUM_LEVELS] PROGMEM = { 8, 16, 32, 64, 80, 128, 192, 255 };

inline uint8_t getBrightness() {
    uint8_t level = brightnessLevels[currentEffect];
    return pgm_read_byte(&brightnessLevelMapping[level < NUM_LEVELS ? level : DEFAULT_BRIGHTNESS_LEVEL]);
}

inline void setLedBrightness() {
    FastLED.setBrightness(getBrightness());
}

void effectsTick() {
    if (!powerOn) {
        return;
    }

    if (initialiseEffect) {
        clearPixels();
        setLedBrightness();
        delay(100);
    }
    uint32_t now = millis();
    if (timeDiff(now, effTimer) >= DEFAULT_SPEED) {
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
        uint8_t brightness = getBrightness();
        for (int i = 0; i < brightness; i += 8) {
            FastLED.setBrightness(i);
            delay(1);
            FastLED.show();
        }
        FastLED.setBrightness(brightness);
    } else {
        uint8_t brightness = getBrightness();
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


void loadConfig() {
    if (EEPROM.read(0) != 0xAA) {
        // если настройки не были сохранены, заполняем дефолтами яркость
        memset(&brightnessLevels, DEFAULT_BRIGHTNESS_LEVEL, NUM_EFFECTS);

        currentEffect = SPARKLES_EFFECT;
        return;
    }

    currentEffect = EEPROM.read(1);
    if (currentEffect >= NUM_EFFECTS) {
        currentEffect = 0;
    }

    for (byte i = 0; i < NUM_EFFECTS; i++) {
        brightnessLevels[i] = EEPROM.read(i + 11); // (2-10 байт - резерв)
    }
}

void saveConfig() {
    if (EEPROM.read(0) != 0xAA) {
        EEPROM.write(0, 0xAA);
    }

    if (EEPROM.read(1) != currentEffect) {
        EEPROM.write(1, currentEffect);
    }

    for (byte i = 0; i < NUM_EFFECTS; i++) {
        if (EEPROM.read(i + 11) != brightnessLevels[i]) {
            EEPROM.write(i + 11, brightnessLevels[i]);
        }
    }
}

void resetConfig() {
    EEPROM.write(0, 0);
}

void(* resetFunc) () = nullptr; //declare reset function @ address 0

void buttonTick() {
    if (!button.hasEvent()) {
        return;
    }

    InterruptDrivenButtonEvent event = button.pollEvent();
    if (!event.type) {
        return;
    }

    if (event.type == IDB_EVENT_CLICKS) {
        switch (event.clicks) {
            case 1:
                // включить/выключить
                changePower();
                break;

            case 2:
                if (!powerOn) {
                    return;
                }

                // следующий эффект
                if (currentEffect == NUM_EFFECTS - 1) {
                    currentEffect = 0;
                } else {
                    currentEffect++;
                }
                initialiseEffect = true;

                break;

            case 3:
                if (!powerOn) {
                    return;
                }

                // предыдущий эффект
                if (currentEffect == 0) {
                    currentEffect = NUM_EFFECTS - 1;
                } else {
                    currentEffect--;
                }
                initialiseEffect = true;

                break;

            case 4:
                if (!powerOn) {
                    return;
                }

                // сохранить настройки яркости в энергонезависимой памяти
                saveConfig();
                flash(3, 100);

                setLedBrightness();
                break;

            default:
                return;
        }
    } else if (event.type == IDB_EVENT_HOLD) {
        if (!powerOn) {
            return;
        }

        // задержка кнопки - регулирование яркости
        if (event.holdTicks == 1) {
            brightnessDirection = !brightnessDirection;
        }

        int8_t level = brightnessLevels[currentEffect];
        level += brightnessDirection ? 1 : -1;

        if (level >= NUM_LEVELS) {
            level = NUM_LEVELS - 1;
            flash(2, 100);
        } else if (level < 0) {
            level = 0;
            flash(2, 100);
        }

        brightnessLevels[currentEffect] = level;

        setLedBrightness();
    } else if (event.type == IDB_EVENT_BOOT_HOLD) {
        // кнопка удерживалась при включении лампы - сброс настроек яркости
        resetConfig();
        flash(5, 100);
        resetFunc();
    }
}

void setup() {
    loadConfig();
    initialiseEffect = true;

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);

    if (CURRENT_LIMIT > 0) {
        FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
    }

    setLedBrightness();
    FastLED.show();

    button.setup(IDB_ISR(button));

    randomSeed(micros());
}

void loop() {
    effectsTick();
    button.loop();
    buttonTick();
}
