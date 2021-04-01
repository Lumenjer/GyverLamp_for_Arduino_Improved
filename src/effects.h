#ifndef GYVERLAMP_ARDUINO_EFFECTS_H
#define GYVERLAMP_ARDUINO_EFFECTS_H

#include <Arduino.h>
#include <FastLED.h>
#include <colorpalettes.h>

#include "leds.h"


// --------------------------------- конфетти ------------------------------------

#define SPARKLES_SCALE 40

void sparklesRoutine();

// -------------------------------------- огонь ---------------------------------------------
// эффект "огонь"
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
void colorRoutine();

// ------------------------------ снегопад 2.0 --------------------------------
#define SNOW_SCALE 40
void snowRoutine();

// ------------------------------ МАТРИЦА ------------------------------
#define MATRIX_SCALE 40
void matrixRoutine();

// ----------------------------- СВЕТЛЯКИ ------------------------------
#define LIGHTERS_SCALE 40

#ifndef LIGHTERS_AM
#define LIGHTERS_AM 50       // количество светляков
#endif

void lightersRoutine(bool initialise);

// ------------------------------ ШУМ ------------------------------

// ******************* НАСТРОЙКИ *****************
// "масштаб" эффектов. Чем меньше, тем крупнее!
#define MADNESS_SCALE 100
#define CLOUD_SCALE 30
#define LAVA_SCALE 50
#define PLASMA_SCALE 30
#define RAINBOW_SCALE 30
#define RAINBOW_S_SCALE 20
#define ZEBRA_SCALE 30
#define FOREST_SCALE 120
#define OCEAN_SCALE 90

void madnessNoise(bool initialise);
void rainbowNoise(bool initialise);
void rainbowStripeNoise(bool initialise);
void zebraNoise(bool initialise);
void forestNoise(bool initialise);
void oceanNoise(bool initialise);
void plasmaNoise(bool initialise);
void cloudNoise(bool initialise);
void lavaNoise(bool initialise);

#define MAX_DIMENSION (max(WIDTH, HEIGHT))

// ------------------------------ runtime-data для эффектов ------------------------------

union EffectsRuntimeData {
    byte hue = 0; // Радуга, Цвета

    struct FireData {
        unsigned char line[WIDTH];
        unsigned char matrixValue[8][16];
        byte pcnt;

        FireData(): pcnt(0) {
            memset(line, 0, sizeof(line));
            memset(matrixValue, 0, sizeof(matrixValue));
        }
    } fire; // Огонь

    struct LightersData {
        uint16_t lightersPos[2][LIGHTERS_AM];
        int8_t lightersSpeed[2][LIGHTERS_AM];
        CHSV lightersColor[LIGHTERS_AM];
        byte loopCounter;

        LightersData() : loopCounter(0) {
            memset(lightersPos, 0, sizeof(lightersPos));
            memset(lightersSpeed, 0, sizeof(lightersSpeed));
            memset(lightersColor, 0, sizeof(lightersColor));
        }
    } lighters; // Светлячки

    struct NoiseData {
        uint8_t data[MAX_DIMENSION][MAX_DIMENSION];

        CRGBPalette16 currentPalette;
        uint8_t colorLoop;
        uint8_t ihue = 0;

        uint16_t x;
        uint16_t y;
        uint16_t z;

        NoiseData() : currentPalette(PartyColors_p), colorLoop(1), ihue(0), x(0), y(0), z(0) {
            memset(data, 0, sizeof(data));
        }
    } noise; // все шумовые эффекты

    EffectsRuntimeData() {
    }
};

#endif
