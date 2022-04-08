#include "StatusDrawer.h"

#include "Screen.h"
#include "BoardController.h"

StatusDrawer::StatusDrawer() 
{

}

void StatusDrawer::drawStatus(int16_t x, int16_t y, int16_t wifiStrength, const String &time)
{
    const int16_t height = 30;
    drawBattery(x, y, height);
    x += height * 2;
    drawWifi(x, y, height, wifiStrength);
    drawTime(x, y, height, time);
}

void StatusDrawer::drawBattery(int16_t x, int16_t y, int16_t height)
{
    const float voltage = (analogRead(A13) / 4095.0) * 2 * 1.1 * 3.3;
    if (voltage <= 1 ) { 
        // Only display if there is a valid reading
        return;
    }
    const float percentage = voltage / 4.2;
    BoardController::debug("Voltage = " + String(voltage));
    const float batteryIconWidth = height / 3;
    const float batteryIconYOffset = height * 0.1;
    const float batteryIconHeight = height - batteryIconYOffset;
    const float batteryIconFillOffset = batteryIconWidth * 0.2;
    const float batteryIconYPos = y + batteryIconYOffset;
    Screen::mDisplay.drawRect(x, batteryIconYPos, batteryIconWidth, batteryIconHeight, GxEPD_BLACK);
    Screen::mDisplay.fillRect(x + batteryIconWidth * 0.25, y, batteryIconWidth * 0.5, batteryIconYOffset, GxEPD_BLACK);
    const float batteryFillHeight = (batteryIconHeight - batteryIconFillOffset * 2) * percentage;
    Screen::mDisplay.fillRect(x + batteryIconFillOffset, y + height - batteryIconFillOffset - batteryFillHeight, 
                              batteryIconWidth - batteryIconFillOffset * 2, batteryFillHeight, GxEPD_BLACK);
    const float textXPos = x + batteryIconWidth * 1.5;
    Screen::drawString(textXPos, y, String(percentage * 100, 0) + "%", Screen::LEFT, Screen::TOP);
    Screen::drawString(textXPos, y + height, String(voltage, 2) + "v", Screen::LEFT, Screen::BOTTOM);
}

void StatusDrawer::drawWifi(int16_t x, int16_t y, int16_t height, int16_t wifiStrength)
{
    const int barCount = 5;
    const float maxBarHeight = height * 0.5;
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
        Screen::drawLine(x, y, x + lineLength, y + lineLength, lineWidth, Screen::RIGHT , GxEPD_BLACK);
        Screen::drawLine(x, y + lineLength, x + lineLength, y, lineWidth, Screen::RIGHT, GxEPD_BLACK);
    }
}

void StatusDrawer::drawTime(int16_t x, int16_t y, int16_t height, const String &time)
{
    Screen::drawString(x, y + height, time, Screen::LEFT, Screen::BOTTOM);
}