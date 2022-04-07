#include "SleepController.h"

#include "Screen.h"
#include "time.h"

SleepController::SleepController()
    : mStartTime(milis())
{

}

void SleepContoller::sleep(int16_t secs) 
{
    Screen::mDisplay.powerOff();
    esp_sleep_enable_timer_wakeup((secs+20) * 1000000LL); // Added extra 20-secs of sleep to allow for slow ESP32 RTC timers
#ifdef BUILTIN_LED
    pinMode(BUILTIN_LED, INPUT); // If it's On, turn it off and some boards use GPIO-5 for SPI-SS, which remains low after screen use
    digitalWrite(BUILTIN_LED, HIGH);
#endif
    Serial.println("Entering " + String(secs) + "-secs of sleep time");
    Serial.println("Awake for : " + String((millis() - mStartTime) / 1000.0, 3) + "-secs");
    Serial.println("Starting deep-sleep period...");
    esp_deep_sleep_start();      // Sleep for e.g. 30 minutes
}