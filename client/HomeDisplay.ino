#include "Screen.h"
#include "BoardController.h"
#include "Logger.h"

#include "JsonParser.h"

int mStartTime;

void setup()
{
    mStartTime = millis();
    BoardController board;
    // TODO disable led for release
    board.changeLed(false);
    // TODO add delay only for debug mode to get serial initialized
    // delay(2000);
    Screen screen;
    screen.statusDrawer()->drawStatus(0, "Ver: 0.1 [debug]");
    if (!screen.updateScreenData()) {
        screen.statusDrawer()->drawTime("??:??");
    }
    screen.display();
    board.sleep(screen.nextSleepTimeout());
}
//#########################################################################################
void loop()
{ // this will never run!
}