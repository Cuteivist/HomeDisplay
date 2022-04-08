#include "Screen.h"
#include "BoardController.h"

void setup()
{
    BoardController board;
    board.changeLed(true);
    Screen screen;
    if (!screen.updateScreenData()) {
        screen.statusDrawer()->drawTime("?:?:?");
    }
    screen.display();
    board.sleep(2 * 60);
}
//#########################################################################################
void loop()
{ // this will never run!
}