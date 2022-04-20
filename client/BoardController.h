#ifndef SLEEPCONTROLLER_H
#define SLEEPCONTROLLER_H

#include <WString.h>

class BoardController 
{
public:
    BoardController();

    void sleep(uint64_t secs);
    void changeLed(bool show);
};

#endif //SLEEPCONTROLLER_H
