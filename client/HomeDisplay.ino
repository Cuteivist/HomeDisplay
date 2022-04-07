#include "Screen.h"
#include "SleepController.h"

#include <vector>

#define BUILTIN_LED 13

// Connections for Adafruit ESP32 Feather
static const uint8_t EPD_BUSY = 32; // to EPD BUSY
static const uint8_t EPD_CS = 15; // to EPD CS
static const uint8_t EPD_RST = 27; // to EPD RST
static const uint8_t EPD_DC = 33; // to EPD DC
static const uint8_t EPD_SCK = 5; // to EPD CLK
static const uint8_t EPD_MISO = 19; // Master-In Slave-Out not used, as no data from mDisplay
static const uint8_t EPD_MOSI = 18; // to EPD DIN

//#########################################################################################
void setup()
{
    Serial.begin(115200);
    Screen screen;
    SleepController sleepController;
    
    changeLed(true);

    Screen::display();
    sleepController.sleep(3 * 60);
}

//#########################################################################################
void loop()
{ // this will never run!
}

void changeLed(bool show)
{
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, show ? HIGH : LOW);
}