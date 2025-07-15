#include <Arduino.h>

#include "DFPlayer.h"
#include "Screen.h"

#define button_previous 3
#define button_next 4
#define button_pause 5
#define button_volume_down 6
#define button_volume_up 7

DFPlayer dFPlayer;
Screen screen(SSD1306_I2C_ADDRESS);

void setup()
{
    pinMode(button_previous, INPUT_PULLUP);
    pinMode(button_next, INPUT_PULLUP);
    pinMode(button_pause, INPUT_PULLUP);
    pinMode(button_volume_up, INPUT_PULLUP);
    pinMode(button_volume_down, INPUT_PULLUP);

    pinMode(LED_BUILTIN, OUTPUT);

    screen.init();
    screen.clearBuffer();
    screen.drawString(6, 24, true, "HELLO!");
    screen.drawBufferToScreen();

    dFPlayer.init();
}

void loop()
{
    if (!digitalRead(button_previous))
    {
        dFPlayer.playPrevious();
        delay(200); // Debounce delay
    }

    if (!digitalRead(button_next))
    {
        dFPlayer.playNext();
        delay(200); // Debounce delay
    }

    if (!digitalRead(button_pause))
    {
        dFPlayer.togglePause();
        delay(500); // Debounce delay
    }

    if (!digitalRead(button_volume_up))
    {
        dFPlayer.increaseVolume();
        delay(200); // Debounce delay
    }

    if (!digitalRead(button_volume_down))
    {
        dFPlayer.decreaseVolume();
        delay(200); // Debounce delay
    }
}