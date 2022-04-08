#include "BoardController.h"

#include "Screen.h"

#define BUILTIN_LED 13

int BoardController::mStartTime = 0;

BoardController::BoardController()
{
    mStartTime = millis();
    Serial.begin(115200);
}

void BoardController::debug(const String &str)
{
    Serial.println(String(" | ") + String(millis() - mStartTime) + " - " + str);
}

void BoardController::debug(char* str)
{
    debug(String(str));
}

void BoardController::sleep(int16_t secs) 
{
    Screen::mDisplay.powerOff();
    esp_sleep_enable_timer_wakeup((secs+20) * 1000000LL); // Added extra 20-secs of sleep to allow for slow ESP32 RTC timers
#ifdef BUILTIN_LED
    pinMode(BUILTIN_LED, INPUT); // If it's On, turn it off and some boards use GPIO-5 for SPI-SS, which remains low after screen use
    digitalWrite(BUILTIN_LED, LOW);
#endif
    debug("Entering " + String(secs) + "-secs of sleep time");
    debug("Awake for : " + String((millis() - mStartTime) / 1000.0, 3) + "-secs");
    debug("Starting deep-sleep period...");
    esp_deep_sleep_start();      // Sleep for e.g. 30 minutes
}

void BoardController::changeLed(bool show)
{
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, show ? HIGH : LOW);
}