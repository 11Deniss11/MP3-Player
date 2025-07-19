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
    screen.drawBufferToScreen();

    dFPlayer.init();

    int x = 5;
    while (x < SCREEN_WIDTH)
    {
        screen.drawString(Vector2(6, 24), true, "HELLO!", true, Vector2(x, 0), Vector2(x + 20, 31));
        screen.drawBufferToScreen();
        x += 30;
    }

    delay(500);

    while (digitalRead(button_previous) && digitalRead(button_next) && digitalRead(button_pause) && digitalRead(button_volume_up) && digitalRead(button_volume_down))
    {
        delay(100);
    }
    screen.clearBuffer();
    screen.drawBufferToScreen();
}

void loop()
{
    String trackNumStr = String(dFPlayer.getCurrentTrack());
    screen.clearBuffer();
    screen.drawString(Vector2(2, 11), false, trackNumStr.c_str());
    screen.drawBufferToScreen();

    if (!digitalRead(button_previous))
    {
        dFPlayer.playPrevious();
        delay(200); // Debounce delay
    }

    if (!digitalRead(button_next))
    {
        dFPlayer.playNext();
        delay(200);
    }

    if (!digitalRead(button_pause))
    {
        dFPlayer.togglePause();
        delay(500);
    }

    if (!digitalRead(button_volume_up))
    {
        dFPlayer.increaseVolume();
        delay(200);
    }

    if (!digitalRead(button_volume_down))
    {
        dFPlayer.decreaseVolume();
        delay(200);
    }
}