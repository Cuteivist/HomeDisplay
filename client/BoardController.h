#ifndef SLEEPCONTROLLER_H
#define SLEEPCONTROLLER_H

#include <WString.h>

class BoardController 
{
public:
    BoardController();

    void sleep(int16_t secs);
    void changeLed(bool show);
};

#endif //SLEEPCONTROLLER_H
