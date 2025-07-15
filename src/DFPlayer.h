#ifndef DFPLAYER_H
#define DFPLAYER_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#define serial_TX 10
#define serial_RX 11

#define Start_Byte 0x7E
#define Version_Byte 0xFF
#define Command_Length 0x06
#define End_Byte 0xEF
#define Acknowledge 0x00 // Returns info with command 0x41 [0x01: info, 0x00: no info]

#define Command_Wait_Period 200

class DFPlayer
{
public:
    void init();
    void togglePause();
    void playNext();
    void playPrevious();
    void increaseVolume() { setVolume(volume + 1); }
    void decreaseVolume() { setVolume(volume - 1); }

private:
    void pause();
    void play();
    void setVolume(int volume);
    void execute_CMD(byte CMD, byte Par1, byte Par2);

    // SoftwareSerial dFPlayerSerial;
    unsigned long lastCommandTime = 0;
    boolean isPlaying = false;
    int volume = 10;
};

#endif // DFPLAYER_H