//
#define BUILTIN_LED 13

#include "Screen.h"
#include "SleepController.h"
#include "NetworkManager.h"

#include "WiFiType.h"

#include <vector>

//#########################################################################################
void setup()
{
    Serial.begin(115200);
    changeLed(true);
    SleepController sleepController;
    updateScreenData();
    sleepController.sleep(2 * 60);
}

void updateScreenData()
{
    Screen screen;
    NetworkManager networkManager;
    const bool connectedToWifi = networkManager.connectToWifi() != WL_CONNECTED;
    if (connectedToWifi) {
      // send request
    }

    screen.statusDrawer()->drawStatus(100, 100, networkManager.signalStrength(), "06:00:00");
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