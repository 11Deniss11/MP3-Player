#include "DFPlayer.h"

// add a thing to save both volume and track number to flash

void DFPlayer::init()
{
    Serial.begin(9600);
    delay(500);
    execute_CMD(0x09, 0, 2); // set output to headphones
    execute_CMD(0x07, 0, 0); // set equalizer to normal
    execute_CMD(0x3F, 0, 0); // reset playback
    setVolume(volume);
    execute_CMD(0x11, 0, 1); // play first track
    isPlaying = true;
}

void DFPlayer::togglePause()
{
    if (isPlaying)
    {
        pause();
        isPlaying = false;
    }
    else
    {
        play();
        isPlaying = true;
    }
}

void DFPlayer::pause()
{
    execute_CMD(0x0E, 0, 0);
}

void DFPlayer::play()
{
    execute_CMD(0x0D, 0, 1);
}

void DFPlayer::playNext()
{
    execute_CMD(0x01, 0, 1);
}

void DFPlayer::playPrevious()
{
    execute_CMD(0x02, 0, 1);
}

void DFPlayer::setVolume(int pVolume)
{
    pVolume = constrain(pVolume, 0, 30); // between 0 and 30
    execute_CMD(0x06, 0, pVolume);
    volume = pVolume;
}

int DFPlayer::getVolume()
{
    return volume;
}

int DFPlayer::getCurrentTrack()
{
    execute_CMD(0x4B, 0, 0); // Query current track
    if (waitUntilAvailable())
    {
        delay(50);
        byte response[10];
        for (byte i = 0; i < 10; i++)
        {
            response[i] = Serial.read();
        }
        // Check if the response is valid
        if (response[0] == Start_Byte && response[1] == Version_Byte && response[2] == Command_Length &&
            response[3] == 0x4B && response[4] == Acknowledge && response[9] == End_Byte)
        {
            return (response[6] << 8) | response[7]; // Combine high and low byte
        }
    }
}

bool DFPlayer::waitUntilAvailable()
{
    unsigned long startTime = millis();
    while (!Serial.available() && millis() - startTime < 300)
    {
        // Wait for data to be available
    }
    return Serial.available() > 0;
}

void DFPlayer::execute_CMD(byte CMD, byte Par1, byte Par2)
{
    unsigned long timeSinceLastCommand = millis() - lastCommandTime;
    if (timeSinceLastCommand < Command_Wait_Period)
    {
        // digitalWrite(LED_BUILTIN, HIGH);
        delay(Command_Wait_Period - timeSinceLastCommand);
    }
    // digitalWrite(LED_BUILTIN, LOW);
    lastCommandTime = millis();

    // Calculate the checksum (2 bytes)
    word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
    // Build the command line
    byte Command_line[10] = {Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge,
                             Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte};
    // Send the command line to the module
    for (byte k = 0; k < 10; k++)
    {
        Serial.write(Command_line[k]);
    }
}
