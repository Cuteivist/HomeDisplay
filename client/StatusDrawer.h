#ifndef STATUSDRAWER_H
#define STATUSDRAWER_H

#include <WString.h>

class StatusDrawer 
{
public:
    StatusDrawer();

    void drawStatus(int16_t x, int16_t y, int16_t wifiStrength, const String &time);

private:
    void drawBattery(int16_t x, int16_t y, int16_t height);
    void drawWifi(int16_t x, int16_t y, int16_t height, int16_t wifiStrength);
    void drawTime(int16_t x, int16_t y, int16_t height, const String &time);
};

#endif // STATUSDRAWER_H