#include "leds.h"

CRGB leds[NUM_LEDS];

// залить все
void fillAll(CRGB color) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        leds[i] = color;
    }
}

void fadeToBlack(uint16_t pixNum, uint8_t step) {
    uint16_t thisPixel = pixNum * SEGMENTS;
    for (byte i = 0; i < SEGMENTS; i++) {
        leds[thisPixel + i].fadeToBlackBy(step);
    }
}

// функция отрисовки точки по номеру пикселя
void drawPixel(uint16_t pixNum, CRGB color) {
    uint16_t thisPixel = pixNum * SEGMENTS;
    for (byte i = 0; i < SEGMENTS; i++) {
        leds[thisPixel + i] = color;
    }
}

// функция отрисовки точки по координатам X Y
void drawPixelXY(uint8_t x, uint8_t y, CRGB color) {
    if (x < 0 || x > WIDTH - 1 || y < 0 || y > HEIGHT - 1) {
        return;
    }
    uint16_t thisPixel = getPixelNumber(x, y) * SEGMENTS;
    for (byte i = 0; i < SEGMENTS; i++) {
        leds[thisPixel + i] = color;
    }
}

CRGB getPixelValue(uint16_t pixNum) {
    uint16_t thisPixel = pixNum * SEGMENTS;
    if (thisPixel < 0 || thisPixel > NUM_LEDS - 1) {
        return 0;
    }
    return leds[thisPixel];
}

// функция получения цвета пикселя по его номеру
uint32_t getPixelColor(uint16_t pixNum) {
    uint16_t thisPixel = pixNum * SEGMENTS;
    if (thisPixel < 0 || thisPixel > NUM_LEDS - 1) {
        return 0;
    }
    return (((uint32_t)leds[thisPixel].r << 16) | ((long)leds[thisPixel].g << 8 ) | (long)leds[thisPixel].b);
}

// функция получения цвета пикселя в матрице по его координатам
uint32_t getPixelColorXY(uint8_t x, uint8_t y) {
    return getPixelColor(getPixelNumber(x, y));
}

// **************** НАСТРОЙКА МАТРИЦЫ ****************
#if (CONNECTION_ANGLE == 0 && STRIP_DIRECTION == 0)
#define _WIDTH WIDTH
#define THIS_X x
#define THIS_Y y

#elif (CONNECTION_ANGLE == 0 && STRIP_DIRECTION == 1)
#define _WIDTH HEIGHT
#define THIS_X y
#define THIS_Y x

#elif (CONNECTION_ANGLE == 1 && STRIP_DIRECTION == 0)
#define _WIDTH WIDTH
#define THIS_X x
#define THIS_Y (HEIGHT - y - 1)

#elif (CONNECTION_ANGLE == 1 && STRIP_DIRECTION == 3)
#define _WIDTH HEIGHT
#define THIS_X (HEIGHT - y - 1)
#define THIS_Y x

#elif (CONNECTION_ANGLE == 2 && STRIP_DIRECTION == 2)
#define _WIDTH WIDTH
#define THIS_X (WIDTH - x - 1)
#define THIS_Y (HEIGHT - y - 1)

#elif (CONNECTION_ANGLE == 2 && STRIP_DIRECTION == 3)
#define _WIDTH HEIGHT
#define THIS_X (HEIGHT - y - 1)
#define THIS_Y (WIDTH - x - 1)

#elif (CONNECTION_ANGLE == 3 && STRIP_DIRECTION == 2)
#define _WIDTH WIDTH
#define THIS_X (WIDTH - x - 1)
#define THIS_Y y

#elif (CONNECTION_ANGLE == 3 && STRIP_DIRECTION == 1)
#define _WIDTH HEIGHT
#define THIS_X y
#define THIS_Y (WIDTH - x - 1)

#else
#define _WIDTH WIDTH
#define THIS_X x
#define THIS_Y y
#pragma message "Wrong matrix parameters! Set to default"

#endif

// получить номер пикселя в ленте по координатам
uint16_t getPixelNumber(uint8_t x, uint8_t y) {
#ifndef XY_EXTERN
    if ((THIS_Y % 2 == 0) || MATRIX_TYPE) {               // если чётная строка
        return (THIS_Y * _WIDTH + THIS_X);
    } else {                                              // если нечётная строка
        return (THIS_Y * _WIDTH + _WIDTH - THIS_X - 1);
    }
#else
    uint16_t i = (y * WIDTH) + x;
    uint16_t j = pgm_read_dword(&XYTable[i]);
    return j;
#endif

}

inline void clearPixels() {
    FastLED.clear();
}

void flash(byte num, unsigned int interval) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        drawPixel(i, CRGB(0xFFFFFF));
    }
    for (byte n = 0; n < num; n++) {
        FastLED.setBrightness(16);
        FastLED.show();
        delay(interval);
        FastLED.setBrightness(0);
        FastLED.show();
        delay(interval);
    }
    clearPixels();
}
