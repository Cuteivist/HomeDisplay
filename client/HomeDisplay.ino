#include "Screen.h"
#include "BoardController.h"
#include "Logger.h"

#include "JsonParser.h"

int mStartTime;

void setup()
{
    mStartTime = millis();
    BoardController board;
    board.changeLed(true);
    // TODO add delay only for debug mode to get serial initialized
    delay(2000);
    Screen screen;
    if (!screen.updateScreenData()) {
        screen.statusDrawer()->drawTime("??:??");
    }
    screen.display();
    board.sleep(2 * 60);
}
//#########################################################################################
void loop()
{ // this will never run!
}