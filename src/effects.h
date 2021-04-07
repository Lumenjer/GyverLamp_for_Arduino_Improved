#ifndef GYVERLAMP_ARDUINO_EFFECTS_H
#define GYVERLAMP_ARDUINO_EFFECTS_H

#include <Arduino.h>
#include <FastLED.h>
#include <colorpalettes.h>

#include "leds.h"

#define MAX_DIMENSION (max(WIDTH, HEIGHT))
#define MIN_DIMENSION (min(WIDTH, HEIGHT))


// --------------------------------- конфетти ------------------------------------

#define SPARKLES_SCALE 40

void sparklesRoutine();

// -------------------------------------- огонь ---------------------------------------------
// эффект "огонь"

struct FireData {
    uint8_t line[WIDTH] {};
    uint8_t matrixValue[8][16] {};
    byte pcnt;

    FireData(): pcnt(0) {
        memset(line, 0, sizeof(line));
        memset(matrixValue, 0, sizeof(matrixValue));
    }
};

#define FIRE_SPARKLES 1        // вылетающие угольки вкл выкл
#define FIRE_SCALE 0.1

void fireRoutine(bool initialise);

// ---------------------------------------- радуга ------------------------------------------
#define RAINBOW_VERTICAL_SCALE 40
void rainbowVertical();

#define RAINBOW_HORIZONTAL_SCALE 40
void rainbowHorizontal();

// ---------------------------------------- ЦВЕТА ------------------------------------------

#define COLORS_SCALE 40
void colorsRoutine();

// --------------------------------- ЦВЕТ ------------------------------------

#define COLOR_SCALE 60
void singleColorRoutine();

// ------------------------------ снегопад 2.0 --------------------------------
#define SNOW_SCALE 40
void snowRoutine();

// ------------------------------ МАТРИЦА ------------------------------
#define MATRIX_SCALE 40
void matrixRoutine();

// ----------------------------- СВЕТЛЯКИ ------------------------------

#ifndef LIGHTERS_AM
#define LIGHTERS_AM 50       // количество светляков , настраивается так, чтобы потребление памяти не выходило за лимиты ардуино
#endif

struct LightersData {
    uint16_t lightersPos[2][LIGHTERS_AM] {};
    int8_t lightersSpeed[2][LIGHTERS_AM] {};
    CHSV lightersColor[LIGHTERS_AM] {};
    byte loopCounter;

    LightersData() : loopCounter(0) {
        memset(lightersPos, 0, sizeof(lightersPos));
        memset(lightersSpeed, 0, sizeof(lightersSpeed));
        memset(lightersColor, 0, sizeof(lightersColor));
    }
};

#define LIGHTERS_SCALE 40

void lightersRoutine(bool initialise);

// ------------------------------ ШУМ ------------------------------

struct NoiseData { // структура данных для хранения всех необходимых параметров для эффекта

    // В оригинальной имплементации используется один большой квадратный массив MAX_DIMENSION х MAX_DIMENSION
    // Однако, для x >= MIN_DIMENSION && y >= MIN_DIMENSION данные из массива никак не используются — нет смысла на них тратить память.
    // Я разбил квадратный массив на 2: MAX_DIMENSION x MIN_DIMENSION и MIN_DIMENSION x MAX_DIMENSION - MIN_DIMENSION таким образом,
    // чтобы они суммарно образовали используемую часть большого квадратного массива
//    uint8_t data[MAX_DIMENSION][MAX_DIMENSION] {};
    uint8_t data[MAX_DIMENSION][MIN_DIMENSION] {};
    uint8_t data2[MIN_DIMENSION][MAX_DIMENSION - MIN_DIMENSION] {};

    uint16_t speed = 20; // speed is set dynamically once we've started up
    uint16_t scale = 30; // scale is set dynamically once we've started up

    uint8_t colorLoop;
    uint8_t ihue = 0;

    uint16_t x;
    uint16_t y;
    uint16_t z;

    CRGBPalette16 currentPalette;

    NoiseData() : colorLoop(1), ihue(0), x(0), y(0), z(0), currentPalette(PartyColors_p) {
        memset(data, 0, sizeof(data));
    }
};


// ******************* НАСТРОЙКИ *****************
// XXX_SCALE - "масштаб" эффектов. Чем меньше, тем крупнее!
// XXX_SPEED - "скорость" эффектов. Чем меньше, тем медленнее!

#ifndef MADNESS_SCALE
#define MADNESS_SCALE 100
#endif

#ifndef MADNESS_SPEED
#define MADNESS_SPEED 30
#endif
void madnessNoise(bool initialise);


#ifndef RAINBOW_SCALE
#define RAINBOW_SCALE 30
#endif

#ifndef RAINBOW_SPEED
#define RAINBOW_SPEED 30
#endif

void rainbowNoise(bool initialise);


#ifndef RAINBOW_STRIPE_SCALE
#define RAINBOW_STRIPE_SCALE 20
#endif

#ifndef RAINBOW_STRIPE_SPEED
#define RAINBOW_STRIPE_SPEED 40
#endif
void rainbowStripeNoise(bool initialise);


#ifndef ZEBRA_SCALE
#define ZEBRA_SCALE 20
#endif

#ifndef ZEBRA_SPEED
#define ZEBRA_SPEED 5
#endif

void zebraNoise(bool initialise);


#ifndef FOREST_SCALE
#define FOREST_SCALE 120
#endif

#ifndef FOREST_SPEED
#define FOREST_SPEED 40
#endif

void forestNoise(bool initialise);


#ifndef OCEAN_SCALE
#define OCEAN_SCALE 90
#endif


#ifndef OCEAN_SPEED
#define OCEAN_SPEED 10
#endif

void oceanNoise(bool initialise);

#ifndef PLASMA_SCALE
#define PLASMA_SCALE 30
#endif

#ifndef PLASMA_SPEED
#define PLASMA_SPEED 20
#endif

void plasmaNoise(bool initialise);


#ifndef CLOUD_SCALE
#define CLOUD_SCALE 30
#endif

#ifndef CLOUD_SPEED
#define CLOUD_SPEED 10
#endif

void cloudNoise(bool initialise);


#ifndef LAVA_SCALE
#define LAVA_SCALE 20
#endif

#ifndef LAVA_SPEED
#define LAVA_SPEED 10
#endif

void lavaNoise(bool initialise);


// ------------------------------ runtime-data для эффектов ------------------------------

union EffectsRuntimeData {
    byte hue = 0; // Радуга, Цвета
    struct FireData fire; // Огонь
    struct LightersData lighters; // Светлячки
    struct NoiseData noise; // все шумовые эффекты

    EffectsRuntimeData() {
    };
};

#endif
