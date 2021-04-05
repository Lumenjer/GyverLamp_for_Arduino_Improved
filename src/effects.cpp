#include "effects.h"

EffectsRuntimeData d;

// ================================= ЭФФЕКТЫ ====================================
// --------------------------------- конфетти ------------------------------------

void fadePixel(byte i, byte j, byte step) {
    uint16_t pixelNum = getPixelNumber(i, j);
    if (getPixelColor(pixelNum) == 0) return;

    CRGB pixel = getPixelValue(pixelNum);
    if (pixel.r >= 30 || pixel.g >= 30 || pixel.b >= 30) {
        fadeToBlack(pixelNum, step);
    } else {
        drawPixel(pixelNum, 0);
    }
}

void fader(byte step) {
    for (byte i = 0; i < WIDTH; i++) {
        for (byte j = 0; j < HEIGHT; j++) {
            fadePixel(i, j, step);
        }
    }
}

void sparklesRoutine() {
    for (byte i = 0; i < SPARKLES_SCALE; i++) {
        byte x = random(0, WIDTH);
        byte y = random(0, HEIGHT);
        if (getPixelColorXY(x, y) == 0) {
            drawPixelXY(x, y, CHSV(random(0, 255), 255, 255));
        }
    }
    fader(70);
}

// -------------------------------------- огонь ---------------------------------------------
// эффект "огонь"

//these values are substracetd from the generated values to give a shape to the animation
const unsigned char valueMask[8][16] PROGMEM = {
        {32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 , 32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 },
        {64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 , 64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 },
        {96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 , 96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 },
        {128, 64 , 32 , 0  , 0  , 32 , 64 , 128, 128, 64 , 32 , 0  , 0  , 32 , 64 , 128},
        {160, 96 , 64 , 32 , 32 , 64 , 96 , 160, 160, 96 , 64 , 32 , 32 , 64 , 96 , 160},
        {192, 128, 96 , 64 , 64 , 96 , 128, 192, 192, 128, 96 , 64 , 64 , 96 , 128, 192},
        {255, 160, 128, 96 , 96 , 128, 160, 255, 255, 160, 128, 96 , 96 , 128, 160, 255},
        {255, 192, 160, 128, 128, 160, 192, 255, 255, 192, 160, 128, 128, 160, 192, 255}
};

//these are the hues for the fire,
//should be between 0 (red) to about 25 (yellow)
const unsigned char hueMask[8][16] PROGMEM = {
        {1 , 11, 19, 25, 25, 22, 11, 1 , 1 , 11, 19, 25, 25, 22, 11, 1 },
        {1 , 8 , 13, 19, 25, 19, 8 , 1 , 1 , 8 , 13, 19, 25, 19, 8 , 1 },
        {1 , 8 , 13, 16, 19, 16, 8 , 1 , 1 , 8 , 13, 16, 19, 16, 8 , 1 },
        {1 , 5 , 11, 13, 13, 13, 5 , 1 , 1 , 5 , 11, 13, 13, 13, 5 , 1 },
        {1 , 5 , 11, 11, 11, 11, 5 , 1 , 1 , 5 , 11, 11, 11, 11, 5 , 1 },
        {0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 , 0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 },
        {0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 , 0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 },
        {0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 }
};

// Randomly generate the next line (matrix row)

void generateLine() {
    for (uint8_t x = 0; x < WIDTH; x++) {
        d.fire.line[x] = random(64, 255);
    }
}

void shiftUp() {
    for (uint8_t y = HEIGHT - 1; y > 0; y--) {
        for (uint8_t x = 0; x < WIDTH; x++) {
            uint8_t newX = x;
            if (x > 15) {
                newX = x - 15;
            }
            if (y > 7) {
                continue;
            }
            d.fire.matrixValue[y][newX] = d.fire.matrixValue[y - 1][newX];
        }
    }

    for (uint8_t x = 0; x < WIDTH; x++) {
        uint8_t newX = x;
        if (x > 15) {
            newX = x - 15;
        }
        d.fire.matrixValue[0][newX] = d.fire.line[newX];
    }
}

// draw a frame, interpolating between 2 "key frames"
// @param pcnt percentage of interpolation

void drawFrame(byte pcnt) {
    int nextv;

    //each row interpolates with the one before it
    for (uint8_t y = HEIGHT - 1; y > 0; y--) {
        for (uint8_t x = 0; x < WIDTH; x++) {
            uint8_t newX = x;
            if (x > 15) {
                newX = x - 15;
            }
            if (y < 8) {
                nextv = (((100.0 - pcnt) * d.fire.matrixValue[y][newX] + pcnt * d.fire.matrixValue[y - 1][newX]) / 100.0)
                        - pgm_read_byte(&(valueMask[y][newX]));

                CRGB color = CHSV(
                        FIRE_SCALE * 2.5 + pgm_read_byte(&(hueMask[y][newX])), // H
                        255, // S
                        (uint8_t)max(0, nextv) // V
                );

                drawPixelXY(x, y, color);
            } else if (y == 8 && FIRE_SPARKLES) {
                if (random(0, 20) == 0 && getPixelColorXY(x, y - 1) != 0) {
                    drawPixelXY(x, y, getPixelColorXY(x, y - 1));
                } else {
                    drawPixelXY(x, y, 0);
                }
            } else if (FIRE_SPARKLES) {
                // старая версия для яркости
                if (getPixelColorXY(x, y - 1) > 0) {
                    drawPixelXY(x, y, getPixelColorXY(x, y - 1));
                } else {
                    drawPixelXY(x, y, 0);
                }
            }
        }
    }

    //first row interpolates with the "next" line
    for (uint8_t x = 0; x < WIDTH; x++) {
        uint8_t newX = x;
        if (x > 15) {
            newX = x - 15;
        }
        CRGB color = CHSV(
                FIRE_SCALE * 2.5 + pgm_read_byte(&(hueMask[0][newX])), // H
                255,           // S
                (uint8_t)(((100.0 - pcnt) * d.fire.matrixValue[0][newX] + pcnt * d.fire.line[newX]) / 100.0) // V
        );
        drawPixelXY(newX, 0, color);
    }
}

void fireRoutine(bool initialise) {
    if (initialise) {
        generateLine();
    }
    if (d.fire.pcnt >= 100) {
        shiftUp();
        generateLine();
        d.fire.pcnt = 0;
    }
    drawFrame(d.fire.pcnt);
    d.fire.pcnt += 30;
}

// ---------------------------------------- радуга ------------------------------------------
byte hue;

void rainbowVertical() {
    hue += 2;
    for (byte j = 0; j < HEIGHT; j++) {
        CHSV thisColor = CHSV((byte)(hue + j * RAINBOW_VERTICAL_SCALE), 255, 255);
        for (byte i = 0; i < WIDTH; i++) {
            drawPixelXY(i, j, thisColor);
        }
    }
}

void rainbowHorizontal() {
    hue += 2;
    for (byte i = 0; i < WIDTH; i++) {
        CHSV thisColor = CHSV((byte)(hue + i * RAINBOW_HORIZONTAL_SCALE), 255, 255);
        for (byte j = 0; j < HEIGHT; j++) {
            drawPixelXY(i, j, thisColor);
        }
    }
}

// ---------------------------------------- ЦВЕТА ------------------------------------------
void colorsRoutine() {
    hue += 1;
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        drawPixel(i, CHSV(hue, 255, 255));
    }
}

// --------------------------------- ЦВЕТ ------------------------------------
void singleColorRoutine() {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        drawPixel(i, CHSV(COLOR_SCALE * 2.5, 255, 255));
    }
}

// ------------------------------ снегопад 2.0 --------------------------------
void snowRoutine() {
    // сдвигаем всё вниз
    for (byte x = 0; x < WIDTH; x++) {
        for (byte y = 0; y < HEIGHT - 1; y++) {
            drawPixelXY(x, y, getPixelColorXY(x, y + 1));
        }
    }

    for (byte x = 0; x < WIDTH; x++) {
        // заполняем случайно верхнюю строку
        // а также не даём двум блокам по вертикали вместе быть
        if (getPixelColorXY(x, HEIGHT - 2) == 0 && (random(0, SNOW_SCALE) == 0)) {
            drawPixelXY(x, HEIGHT - 1, 0xE0FFFF - 0x101010 * random(0, 4));
        } else {
            drawPixelXY(x, HEIGHT - 1, 0x000000);
        }
    }
}

// ------------------------------ МАТРИЦА ------------------------------
void matrixRoutine() {
    for (byte x = 0; x < WIDTH; x++) {
        // заполняем случайно верхнюю строку
        uint32_t thisColor = getPixelColorXY(x, HEIGHT - 1);
        if (thisColor == 0) {
            drawPixelXY(x, HEIGHT - 1, 0x00FF00 * (random(0, MATRIX_SCALE) == 0));
        } else if (thisColor < 0x002000) {
            drawPixelXY(x, HEIGHT - 1, 0);
        } else {
            drawPixelXY(x, HEIGHT - 1, thisColor - 0x002000);
        }
    }

    // сдвигаем всё вниз
    for (byte x = 0; x < WIDTH; x++) {
        for (byte y = 0; y < HEIGHT - 1; y++) {
            drawPixelXY(x, y, getPixelColorXY(x, y + 1));
        }
    }
}

// ----------------------------- СВЕТЛЯКИ ------------------------------

void lightersRoutine(bool initialise) {
    if (initialise) {
        randomSeed(millis());
        for (byte i = 0; i < LIGHTERS_AM; i++) {
            d.lighters.lightersPos[0][i] = random(0, WIDTH * 10);
            d.lighters.lightersPos[1][i] = random(0, HEIGHT * 10);
            d.lighters.lightersSpeed[0][i] = random(-10, 10);
            d.lighters.lightersSpeed[1][i] = random(-10, 10);
            d.lighters.lightersColor[i] = CHSV(random(0, 255), 255, 255);
        }
    }
    clearPixels();
    if (++d.lighters.loopCounter > 20) {
        d.lighters.loopCounter = 0;
    }
    for (byte i = 0; i < LIGHTERS_SCALE; i++) {
        if (d.lighters.loopCounter == 0) {     // меняем скорость каждые 255 отрисовок
            d.lighters.lightersSpeed[0][i] += random(-3, 4);
            d.lighters.lightersSpeed[1][i] += random(-3, 4);
            d.lighters.lightersSpeed[0][i] = constrain(d.lighters.lightersSpeed[0][i], -20, 20);
            d.lighters.lightersSpeed[1][i] = constrain(d.lighters.lightersSpeed[1][i], -20, 20);
        }

        d.lighters.lightersPos[0][i] += d.lighters.lightersSpeed[0][i];
        d.lighters.lightersPos[1][i] += d.lighters.lightersSpeed[1][i];

        if (d.lighters.lightersPos[0][i] < 0) {
            d.lighters.lightersPos[0][i] = (WIDTH - 1) * 10;
        }
        if (d.lighters.lightersPos[0][i] >= WIDTH * 10) {
            d.lighters.lightersPos[0][i] = 0;
        }

        if (d.lighters.lightersPos[1][i] < 0) {
            d.lighters.lightersPos[1][i] = 0;
            d.lighters.lightersSpeed[1][i] = -d.lighters.lightersSpeed[1][i];
        }
        if (d.lighters.lightersPos[1][i] >= (HEIGHT - 1) * 10) {
            d.lighters.lightersPos[1][i] = (HEIGHT - 1) * 10;
            d.lighters.lightersSpeed[1][i] = -d.lighters.lightersSpeed[1][i];
        }
        drawPixelXY(d.lighters.lightersPos[0][i] / 10, d.lighters.lightersPos[1][i] / 10, d.lighters.lightersColor[i]);
    }
}

// ================================= ШУМОВЫЕ ЭФФЕКТЫ ====================================

// The 16 bit version of our coordinates

uint16_t speed = 20; // speed is set dynamically once we've started up
uint16_t scale = 30; // scale is set dynamically once we've started up

// ******************* СЛУЖЕБНЫЕ *******************
void fillNoiseLED() {
    uint8_t dataSmoothing = 0;
    if (speed < 50) {
        dataSmoothing = 200 - (speed * 4);
    }
    for (uint16_t i = 0; i < MAX_DIMENSION; i++) {
        int ioffset = scale * i;
        for (uint16_t j = 0; j < MAX_DIMENSION; j++) {
            int joffset = scale * j;

            uint8_t data = inoise8(d.noise.x + ioffset, d.noise.y + joffset, d.noise.z);

            data = qsub8(data, 16);
            data = qadd8(data, scale8(data, 39));

            if (dataSmoothing) {
                uint8_t olddata = d.noise.data[i][j];
                uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
                data = newdata;
            }

            d.noise.data[i][j] = data;
        }
    }
    d.noise.z += speed;

    // apply slow drift to X and Y, just for visual variation.
    d.noise.x += speed / 8;
    d.noise.y -= speed / 16;

    for (uint8_t i = 0; i < WIDTH; i++) {
        for (uint8_t j = 0; j < HEIGHT; j++) {
            uint8_t index = d.noise.data[j][i];
            uint8_t bri =   d.noise.data[i][j];
            // if this palette is a 'loop', add a slowly-changing base value
            if (d.noise.colorLoop) {
                index += d.noise.ihue;
            }
            // brighten up, as the color palette itself often contains the
            // light/dark dynamic range desired
            if (bri > 127) {
                bri = 255;
            } else {
                bri = dim8_raw(bri * 2);
            }
            CRGB color = ColorFromPalette(d.noise.currentPalette, index, bri);
            drawPixelXY(i, j, color);   //leds[getPixelNumber(i, j)] = color;
        }
    }
    d.noise.ihue += 1;
}

void fillnoise8() {
    for (uint16_t i = 0; i < MAX_DIMENSION; i++) {
        uint16_t ioffset = scale * i;
        for (uint16_t j = 0; j < MAX_DIMENSION; j++) {
            uint16_t joffset = scale * j;
            d.noise.data[i][j] = inoise8(d.noise.x + ioffset, d.noise.y + joffset, d.noise.z);
        }
    }
    d.noise.z += speed;
}

void madnessNoise(bool initialise) {
    if (initialise) {
        scale = MADNESS_SCALE;
        speed = DEFAULT_SPEED;
    }
    fillnoise8();
    for (uint8_t i = 0; i < WIDTH; i++) {
        for (uint8_t j = 0; j < HEIGHT; j++) {
            CRGB thisColor = CHSV(d.noise.data[j][i], 255, d.noise.data[i][j]);
            drawPixelXY(i, j, thisColor);
        }
    }
    d.noise.ihue += 1;
}
void rainbowNoise(bool initialise) {
    if (initialise) {
        d.noise.currentPalette = RainbowColors_p;
        scale = RAINBOW_SCALE;
        speed = DEFAULT_SPEED;
        d.noise.colorLoop = 1;
    }
    fillNoiseLED();
}
void rainbowStripeNoise(bool initialise) {
    if (initialise) {
        d.noise.currentPalette = RainbowStripeColors_p;
        scale = RAINBOW_S_SCALE;
        speed = DEFAULT_SPEED;
        d.noise.colorLoop = 1;
    }
    fillNoiseLED();
}
void zebraNoise(bool initialise) {
    if (initialise) {
        // 'black out' all 16 palette entries...
        fill_solid(d.noise.currentPalette, 16, CRGB::Black);
        // and set every fourth one to white.
        d.noise.currentPalette[0] = CRGB::White;
        d.noise.currentPalette[4] = CRGB::White;
        d.noise.currentPalette[8] = CRGB::White;
        d.noise.currentPalette[12] = CRGB::White;
        scale = ZEBRA_SCALE;
        speed = DEFAULT_SPEED;
        d.noise.colorLoop = 1;
    }
    fillNoiseLED();
}
void forestNoise(bool initialise) {
    if (initialise) {
        d.noise.currentPalette = ForestColors_p;
        scale = FOREST_SCALE;
        speed = DEFAULT_SPEED;
        d.noise.colorLoop = 0;
    }
    fillNoiseLED();
}
void oceanNoise(bool initialise) {
    if (initialise) {
        d.noise.currentPalette = OceanColors_p;
        scale = OCEAN_SCALE;
        speed = DEFAULT_SPEED;
        d.noise.colorLoop = 0;
    }

    fillNoiseLED();
}
void plasmaNoise(bool initialise) {
    if (initialise) {
        d.noise.currentPalette = PartyColors_p;
        scale = PLASMA_SCALE;
        speed = DEFAULT_SPEED;
        d.noise.colorLoop = 1;
    }
    fillNoiseLED();
}
void cloudNoise(bool initialise) {
    if (initialise) {
        d.noise.currentPalette = CloudColors_p;
        scale = CLOUD_SCALE;
        speed = DEFAULT_SPEED;
        d.noise.colorLoop = 0;
    }
    fillNoiseLED();
}
void lavaNoise(bool initialise) {
    if (initialise) {
        d.noise.currentPalette = LavaColors_p;
        scale = LAVA_SCALE;
        speed = DEFAULT_SPEED;
        d.noise.colorLoop = 0;
    }
    fillNoiseLED();
}
