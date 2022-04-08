#ifndef SLEEPCONTROLLER_H
#define SLEEPCONTROLLER_H

#include <WString.h>

class BoardController 
{
public:
    BoardController();

    void sleep(int16_t secs);
    void changeLed(bool show);
    static void debug(const String &str);
    static void debug(char* str);

private:
    int mStartTime = 0;
};

#endif //SLEEPCONTROLLER_H
