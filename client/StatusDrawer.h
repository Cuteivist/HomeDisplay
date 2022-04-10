#ifndef STATUSDRAWER_H
#define STATUSDRAWER_H

#include <WString.h>

class StatusDrawer 
{
public:
    StatusDrawer();

    void drawStatus(int16_t wifiStrength, const String &versionText);
    void drawTime(const String &time);
    void drawTitle(const String &title);
    uint16_t statusBarHeight() const;

private:
    const uint16_t mStatusBarHeight;
    const uint16_t mStatusBarLineHeight;
    const uint16_t mPadding;
    void drawBattery(int16_t x, int16_t y);
    void drawWifi(int16_t x, int16_t y, int16_t wifiStrength);
};

#endif // STATUSDRAWER_H