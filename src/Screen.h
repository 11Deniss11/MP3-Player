#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>

#define SSD1306_I2C_ADDRESS 0x3C

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_PAGES (SCREEN_HEIGHT / 8)
#define RIGHT_OFFSET 0
#define UPPER_OFFSET 0

struct Vector2
{
    Vector2(uint8_t px, uint8_t py) : x(px), y(py) {}
    uint8_t x = 0;
    uint8_t y = 0;
};

struct Vector2f
{
    Vector2f(float px, float py) : x(px), y(py) {}
    float x = 0;
    float y = 0;
};

class Screen
{
public:
    uint8_t i2cAddress;
    Screen(uint8_t address);
    void init();
    void drawBufferToScreen();
    void clearBuffer();
    void fillBuffer();
    void moveCursor(uint8_t page, uint8_t col);
    void setPixel(Vector2 coordinate, bool colour);
    void drawChar(Vector2 coordinate, bool isBig, char c, bool occlude = false, Vector2 vert1 = Vector2(0, 0), Vector2 vert2 = Vector2(0, 0));
    void drawString(Vector2 coordinate, bool isBig, const char *str, bool occlude = false, Vector2 vert1 = Vector2(0, 0), Vector2 vert2 = Vector2(0, 0));
    void stop();
    bool edgeFunctionAboveZero(Vector2 a, Vector2 b, Vector2 c);

private:
    void sendCommand(uint8_t command);
    void sendData(uint8_t data);

    uint8_t screenBuffer[SCREEN_PAGES][SCREEN_WIDTH] = {0};
};

typedef struct
{
    uint16_t bitmapOffset; // Index into Pixellari16pt7bBitmaps[]
    uint8_t width;         // Bitmap width in pixels
    uint8_t height;        // Bitmap height in pixels
    uint8_t xAdvance;      // Distance to move cursor (x axis)
    int8_t xOffset;        // X dist from cursor pos to glyph bitmap
    int8_t yOffset;        // Y dist from cursor pos to glyph bitmap
} GFXglyph;

typedef struct
{
    const uint8_t *bitmap; // = Pixellari16pt7bBitmaps
    const GFXglyph *glyph; // = Pixellari16pt7bGlyphs
    uint8_t first;         // ASCII code of first char (usually 32)
    uint8_t last;          // ASCII code of last char (usually 126)
    uint8_t yAdvance;      // Line height
} GFXfont;

#include "Quantico_Regular16pt7b.h"
#include "Quantico_Regular7pt7b.h"

#endif // SCREEN_H