#ifndef STATUSDRAWER_H
#define STATUSDRAWER_H

#include <WString.h>

class StatusDrawer 
{
public:
    StatusDrawer();

    void drawStatus(int16_t wifiStrength);
    void drawTime(const String &time);

private:
    uint16_t mX, mY, mHeight;
    uint16_t rightSectionXPos() const;
    void drawBattery(int16_t x, int16_t y, int16_t height);
    void drawWifi(int16_t x, int16_t y, int16_t height, int16_t wifiStrength);
};

#endif // STATUSDRAWER_H