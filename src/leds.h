#ifndef GYVERLAMP_ARDUINO_LEDS_H
#define GYVERLAMP_ARDUINO_LEDS_H

#include <Arduino.h>
#include <FastLED.h>
#include "common.h"

// функции для работы с LED-матрицей

extern CRGB leds[NUM_LEDS];

// залить все
void fillAll(CRGB color);

void fadeToBlack(uint16_t pixNum, uint8_t step);

// функция отрисовки точки по номеру пикселя
void drawPixel(uint16_t pixNum, CRGB color);

// функция отрисовки точки по координатам X Y
void drawPixelXY(uint8_t x, uint8_t y, CRGB color);

// получить значение пикселя по его номеру
CRGB getPixelValue(uint16_t pixNum);

// функция получения цвета пикселя по его номеру
uint32_t getPixelColor(uint8_t pixNum);

// функция получения цвета пикселя в матрице по его координатам
uint32_t getPixelColorXY(uint8_t x, uint8_t y);

// получить номер пикселя в ленте по координатам
uint16_t getPixelNumber(uint8_t x, uint8_t y);

// очистить матрицу пикселей
void clearPixels();

#endif //GYVERLAMP_ARDUINO_LEDS_H
