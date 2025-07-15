#include <Arduino.h>
#include <Wire.h>
#include "screen.h"
#include <avr/pgmspace.h> // Add this at the top if not already present

// Constructor
Screen::Screen(uint8_t address) : i2cAddress(address)
{
    Wire.begin();
}

// Send command to SSD1306
void Screen::sendCommand(uint8_t command)
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x00); // Co = 0, D/C = 0
    Wire.write(command);
    Wire.endTransmission();
}

// Send data to SSD1306
void Screen::sendData(uint8_t data)
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x40); // Co = 0, D/C = 1
    Wire.write(data);
    Wire.endTransmission();
}

// Initialize SSD1306
void Screen::init()
{
    // Initialize SSD1306
    sendCommand(0xAE);              // Display off
    sendCommand(0xD5);              // Set display clock divide ratio/oscillator frequency
    sendCommand(0x80);              // Set divide ratio
    sendCommand(0xA8);              // Set multiplex ratio
    sendCommand(SCREEN_HEIGHT - 1); // Multiplex ratio for the screen height
    sendCommand(0xD3);              // Set display offset (this is vertical offset)
    sendCommand(UPPER_OFFSET);      // Offset Value (no vertical offset)
    sendCommand(0x40);              // Set start line address (start from line 0)
    sendCommand(0x8D);              // Charge pump setting
    sendCommand(0x14);              // Enable charge pump
    sendCommand(0x20);              // Memory addressing mode
    sendCommand(0x00);              // Horizontal addressing mode
    sendCommand(0xA1);              // Set segment re-map (column address 127 is mapped to SEG0)
    sendCommand(0xC8);              // Set COM output scan direction (remapped mode)
    sendCommand(0xDA);              // Set COM pins hardware configuration
    sendCommand(0x02);              // Alternative COM pin configuration, disable COM left/right remap (apparently 0x12 for keychain pet but 0x02 for this)
    sendCommand(0x81);              // Set contrast control
    sendCommand(0xFF);              // Contrast value
    sendCommand(0xD9);              // Set pre-charge period
    sendCommand(0xF1);              // Phase 1 period: 15 DCLKs, Phase 2 period: 1 DCLK
    sendCommand(0xDB);              // Set VCOMH deselect level
    sendCommand(0x40);              // VCOMH deselect level ~0.77*Vcc
    sendCommand(0xA4);              // Entire display on (resume to RAM content display)
    sendCommand(0xA6);              // Set normal display (0xA6 for normal, 0xA7 for inverse)
    sendCommand(0xAF);              // Display on
}

// Move cursor to a specific page and column
void Screen::moveCursor(uint8_t page, uint8_t col)
{
    sendCommand(0xB0 + page);                          // Set page address
    sendCommand(0x00 | ((RIGHT_OFFSET + col) & 0x0F)); // Set lower column address (32 & 0x0F extracts the lower 4 bits of 32)
    sendCommand(0x10 | ((RIGHT_OFFSET + col) >> 4));   // Set higher column address (32 >> 4 extracts the higher 4 bits of 32)
}

// Draw the buffer to the screen
void Screen::drawBufferToScreen()
{
    for (uint8_t page = 0; page < SCREEN_PAGES; page++)
    {
        moveCursor(page, 0);
        for (uint8_t col = 0; col < SCREEN_WIDTH; col++)
        {
            sendData(screenBuffer[page][col]); // Each byte represents 8 vertical pixels
        }
    }
}

// Clear the screen buffer
void Screen::clearBuffer()
{
    memset(screenBuffer, 0x00, sizeof screenBuffer);
}

// Fill the screen buffer with full white
void Screen::fillBuffer()
{
    memset(screenBuffer, 0xff, sizeof screenBuffer);
}

// Set colour of pixel
void Screen::setPixel(uint8_t x, uint8_t y, bool colour)
{
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)
    {
        return;
    }
    if (colour)
    {
        screenBuffer[y / 8][x] |= (1 << (y % 8));
    }
    else
    {
        screenBuffer[y / 8][x] &= ~(1 << (y % 8));
    }
}

void Screen::drawString(uint8_t x, uint8_t y, bool isBig, const char *str)
{
    const GFXfont *font = isBig ? (const GFXfont *)&Quantico_Regular16pt7b : (const GFXfont *)&Quantico_Regular7pt7b;

    uint8_t first = pgm_read_byte(&font->first);
    uint8_t last = pgm_read_byte(&font->last);

    while (*str)
    {
        char c = *str++;
        if (c < first || c > last)
        {
            continue; // Skip characters not in font
        }
        if (x >= SCREEN_WIDTH - 20 && !isBig)
        {
            for (int i = 0; i < 3; i++)
            {
                drawChar(x, y, isBig, '.');
                x += pgm_read_byte(&font->glyph['.' - first].xAdvance);
            }
            return;
        }
        drawChar(x, y, isBig, c);
        x += pgm_read_byte(&font->glyph[c - first].xAdvance) * 1.2; // Move cursor to the right by the width of the character
    }
}

// https://rop.nl/truetype2gfx/ my beloved
void Screen::drawChar(uint8_t x, uint8_t y, bool isBig, char c)
{
    const GFXfont *font = isBig ? (const GFXfont *)&Quantico_Regular16pt7b : (const GFXfont *)&Quantico_Regular7pt7b;

    uint8_t first = pgm_read_byte(&font->first);
    uint8_t last = pgm_read_byte(&font->last);

    if (c < first || c > last)
    {
        return; // Character not in font
    }

    // Read glyph pointer from PROGMEM
    const GFXglyph *glyphs = (const GFXglyph *)pgm_read_ptr(&font->glyph);
    const GFXglyph *glyph = &glyphs[c - first];

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t w = pgm_read_byte(&glyph->width);
    uint8_t h = pgm_read_byte(&glyph->height);
    int8_t xo = pgm_read_byte(&glyph->xOffset);
    int8_t yo = pgm_read_byte(&glyph->yOffset);

    if (h == 0)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        return;
    }

    const uint8_t *bitmap = (const uint8_t *)pgm_read_ptr(&font->bitmap);
    uint8_t bits = 0, bit = 0;
    for (uint8_t yy = 0; yy < h; yy++)
    {
        for (uint8_t xx = 0; xx < w; xx++)
        {
            if (!(bit++ & 7))
                bits = pgm_read_byte(&bitmap[bo++]);
            if (bits & 0x80)
            {
                int16_t px = x + xo + xx;
                int16_t py = y + yo + yy;
                if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT)
                    setPixel(px, py, true);
            }
            bits <<= 1;
        }
    }
}

// Stop the screen
void Screen::stop()
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x00); // Command mode
    Wire.write(0xAE); // Display off
    Wire.write(0x8D); // Charge pump setting
    Wire.write(0x10); // Disable charge pump
    Wire.write(0xA5); // Entire display ON, ignore RAM content
    Wire.endTransmission();
}