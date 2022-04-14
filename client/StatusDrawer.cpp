#include "StatusDrawer.h"

#include "Screen.h"
#include "Logger.h"

StatusDrawer::StatusDrawer() 
    : mStatusBarHeight(22)
    , mStatusBarLineHeight(3)
    , mPadding(5)
{
}

void StatusDrawer::drawStatus(int16_t wifiStrength, const String &versionText)
{
    // Status bar order:
    // | version | ----- errors ---- | battery | wifi | time |
    //  -----------------------------------------------------
    Screen::drawString(mPadding, mPadding, versionText);
    const uint16_t batteryX = SCREEN_WIDTH - 200;
    drawBattery(batteryX, mPadding);
    drawWifi(batteryX + 120, mPadding, wifiStrength);

    Screen::mDisplay.fillRect(0, mStatusBarHeight, SCREEN_WIDTH, mStatusBarLineHeight, GxEPD_BLACK);
}

void StatusDrawer::drawBattery(int16_t x, int16_t y)
{
    /*
    he ADC value is a 12-bit number, so the maximum value is 4095 (counting from 0). 
    To convert the ADC integer value to a real voltage you’ll need to divide it by the maximum value of 4095, 
    then double it (note above that Adafruit halves the voltage), 
    then multiply that by the reference voltage of the ESP32 which is 3.3V and then vinally,
    multiply that again by the ADC Reference Voltage of 1100mV.
    */
    const float voltage = (analogRead(A13) / 4095.0) * 2 * 1.1 * 3.3;
    const bool invalidRead = voltage <= 1;
    const float percentage = invalidRead ? 0 : voltage / 4.2;
    DEBUG("Voltage = " + String(voltage));
    const uint16_t batteryHeight = mStatusBarHeight * 0.6;
    const uint16_t batteryWidth = 30;
    const uint16_t batteryHeadHeight = batteryHeight * 0.5;
    // outline
    Screen::mDisplay.drawRect(x, y, batteryWidth, batteryHeight, GxEPD_BLACK);
    // head
    Screen::mDisplay.fillRect(x + batteryWidth, y + batteryHeight * 0.5 - batteryHeadHeight * 0.5, batteryWidth * 0.1, batteryHeadHeight, GxEPD_BLACK);
    // fill
    const uint16_t fillPadding = batteryHeight * 0.2;
    const uint16_t fillWidth = (batteryWidth - fillPadding * 2) * percentage; 
    Screen::mDisplay.fillRect(x + fillPadding, y + fillPadding, fillWidth, batteryHeight - fillPadding * 2, GxEPD_BLACK);
    // texts
    const uint16_t textXPos = x + batteryWidth + 10;
    Screen::drawString(textXPos, y, String(percentage * 100, 0) + "%");
    Screen::drawString(textXPos + 35, y, String(voltage, 2) + "v");
}

void StatusDrawer::drawWifi(int16_t x, int16_t y, int16_t wifiStrength)
{
    const int barCount = 5;
    const float maxBarHeight = mStatusBarHeight * 0.6;
    const float barHeightStep = maxBarHeight / barCount;
    const float barWidth = 5;
    for (int i = 0 ; i < barCount ; i++) {
        const float barHeight = barHeightStep * (i + 1);
        const int strength = -100 + (i * 20);
        const float barX = x + i * 6;
        const float barY = y + maxBarHeight - barHeight;
        if (wifiStrength > strength) {
            Screen::mDisplay.fillRect(barX, barY, barWidth, barHeight, GxEPD_BLACK);
        } else {
            Screen::mDisplay.drawRect(barX, barY, barWidth, barHeight, GxEPD_BLACK);
        }
    }

    if (wifiStrength <= -1000) {
        // If not connected draw 'x' above first two bars
        const float lineLength = maxBarHeight * 0.5;
        const uint16_t lineWidth = barWidth * 0.7;
        Screen::drawLine(x, y, x + lineLength, y + lineLength, lineWidth, Screen::RIGHT, GxEPD_BLACK);
        Screen::drawLine(x, y + lineLength, x + lineLength, y, lineWidth, Screen::RIGHT, GxEPD_BLACK);
    }
}

void StatusDrawer::drawTime(const String &time)
{
    Screen::drawString(SCREEN_WIDTH - mPadding * 2, mPadding, time, Screen::RIGHT);
}

void StatusDrawer::drawTitle(const String &title)
{
    Screen::drawString(120, mPadding, title);
}

uint16_t StatusDrawer::statusBarHeight() const
{
    return mStatusBarHeight + mStatusBarLineHeight;
}