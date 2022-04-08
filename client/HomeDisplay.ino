//
#include "Screen.h"
#include "BoardController.h"
#include "NetworkManager.h"

#include "WiFiType.h"

#include <vector>

//#########################################################################################
void setup()
{
    BoardController board;
    board.changeLed(true);
    updateScreenData();
    board.sleep(2 * 60);
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