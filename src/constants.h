#ifndef GYVERLAMP_ARDUINO_COMMON_H
#define GYVERLAMP_ARDUINO_COMMON_H

#include <Arduino.h>
#include <FastLED.h>

// ============= НАСТРОЙКИ =============

// ---------- МАТРИЦА ---------
#define DEFAULT_BRIGHTNESS_LEVEL 4 // стандартный маскимальный уровень яркости (0-7)
#define DEFAULT_SPEED 32     // стандартный уровень скорости (0-255)

#define CURRENT_LIMIT 2500    // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит

#define WIDTH 24              // ширина матрицы
#define HEIGHT 16             // высота матрицы

#define COLOR_ORDER GRB       // порядок цветов на ленте. Если цвет отображается некорректно - меняйте. Начать можно с RGB

#define MATRIX_TYPE 0         // тип матрицы: 0 - зигзаг, 1 - параллельная
#define CONNECTION_ANGLE 1    // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
#define STRIP_DIRECTION 3     // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
// при неправильной настройке матрицы вы получите предупреждение "Wrong matrix parameters! Set to default"
// шпаргалка по настройке матрицы здесь! https://alexgyver.ru/matrix_guide/

// ============= ДЛЯ РАЗРАБОТЧИКОВ =============
#define LED_TYPE WS2811       // тип ленты (WS2811, WS2812B)
#define LED_PIN 4             // пин ленты
#define BTN_PIN 2             // пин кнопки

#define NUM_LEDS WIDTH * HEIGHT
#define SEGMENTS 1            // диодов в одном "пикселе" (для создания матрицы из кусков ленты)


// ---------------- БИБЛИОТЕКИ -----------------
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ALLOW_INTERRUPTS 0

#endif
