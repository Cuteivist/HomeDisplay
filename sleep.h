int CurrentHour = 0, CurrentMin = 0, CurrentSec = 0;
long StartTime = 0;

long SleepDuration = 2; // Sleep time in minutes, aligned to the nearest minute boundary, so if 30 will always update at 00 or 30 past the hour
int WakeupTime = 7;     // Don't wakeup until after 07:00 to save battery power
int SleepTime = 23;     // Sleep after (23+1) 00:00 to save battery power

void beginSleep() {
    //  display.powerOff();
    long SleepTimer = (SleepDuration * 60 - ((CurrentMin % SleepDuration) * 60 + CurrentSec)); // Some ESP32 are too fast to maintain accurate time
    esp_sleep_enable_timer_wakeup((SleepTimer + 20) * 1000000LL);                              // Added extra 20-secs of sleep to allow for slow ESP32 RTC timers
    digitalWrite(BUILTIN_LED, LOW);
    Serial.println("Entering " + String(SleepTimer) + "-secs of sleep time");
    Serial.println("Awake for : " + String((millis() - StartTime) / 1000.0, 3) + "-secs");
    Serial.println("Starting deep-sleep period...");
    esp_deep_sleep_start(); // Sleep for e.g. 30 minutes
}