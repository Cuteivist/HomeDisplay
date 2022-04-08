#ifndef SLEEPCONTROLLER_H
#define SLEEPCONTROLLER_H

#include <cstdint>

class SleepController 
{
public:
    SleepController();

    void sleep(int16_t secs);

private:
    int mStartTime = 0;
};

#endif //SLEEPCONTROLLER_H
