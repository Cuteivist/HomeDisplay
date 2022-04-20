#include "WeatherDrawer.h"

#include "Screen.h"
#include <map>
#include "Logger.h"

WeatherDrawer::WeatherDrawer() 
{

}

void WeatherDrawer::drawWeather(int16_t x, int16_t y, int16_t height, Weather weather)
{
    switch (weather) {
    case Windy:
        drawWind(x, y, height);
        break;
    case Fog:
        drawFog(x, y, height);
        break;
    case Sunny:
        drawSun(x, y, height);
        break;
    case LowCloudy: {
        const int16_t cloudOffset = height * 0.4;
        drawSun(x, y, height);
        drawCloud(x + cloudOffset * 0.5, y + cloudOffset, height - cloudOffset, weather);
        break;
    }
    case FullCloudy: {
        // Top left cloud
        drawCloud(x + height * 0.05, y + height * 0.1, height * 0.5, weather, true);
        // Top right cloud
        drawCloud(x + height * 0.75, y + height * 0.0, height * 0.5, weather, true);
        drawCloud(x, y + height * 0.1, height * 0.8, weather, true);
        break;
    }
    case SnowWithRain:
    case Storm:
    case Rainstorm:
    case Snow:
    case WindyCloud:
    case Rain: {
        const int16_t cloudHeight = height * 0.6;
        drawCloud(x, y, cloudHeight, weather, true);
        drawWeatherUnderCloud(x, y + cloudHeight, height - cloudHeight, weather);
        break;
    }
    default:
        // TODO write error message
        return;
    }
}

void WeatherDrawer::drawMoon(int16_t x, int16_t y, int16_t height, MoonPhase phase, int16_t linesize)
{
    const int16_t radius = height * 0.5;
    const int16_t moonX = x + radius;
    const int16_t moonY = y + radius;
    Screen::mDisplay.fillCircle(moonX, moonY, radius, GxEPD_BLACK);
    bool isLeftSide = true;
    switch (phase) {
    case NewMoon:
        break;
    case WaningCrescent: // Fallthrough
        isLeftSide = false;
    case WaxingCrescent: {
        const int16_t biggerRadius = radius * 1.1;
        for (int16_t i = 0; i < height * 0.3; i++) {
            Screen::mDisplay.drawCircle(moonX + i, moonY, biggerRadius, GxEPD_WHITE);
        }
        for (int16_t i = 0; i < linesize; i++) {
            Screen::mDisplay.drawCircle(moonX, moonY, radius - i, GxEPD_BLACK);
        }
        break;
    }
    case LastQuarter: // Fallthrough
        isLeftSide = false;
    case FirstQuarter: {
        const int16_t rectX = isLeftSide ? x + radius : x;
        const int16_t rectWidth = isLeftSide ? radius + linesize : radius;
        Screen::mDisplay.fillRect(rectX, y, rectWidth, height + linesize, GxEPD_WHITE);
        for (int16_t i = 0; i < linesize; i++) {
            Screen::mDisplay.drawCircle(moonX, moonY, radius - i, GxEPD_BLACK);
        }
        break;
    }
    case WaxingGibbous:
        isLeftSide = false;
    case WaningGibbous: {
        const int16_t secondCircleCenterX = moonX + radius * 0.6 * (isLeftSide ? -1 : 1);
        Screen::mDisplay.fillCircle(secondCircleCenterX, moonY, radius - linesize, GxEPD_WHITE);
        for (int16_t i = 0; i < linesize; i++) {
            Screen::mDisplay.drawCircle(moonX, moonY, radius - i, GxEPD_BLACK);
        }
        break;
    }
    case FullMoon:
        Screen::mDisplay.fillCircle(moonX, moonY, radius - linesize, GxEPD_WHITE);
        break;
    }
}

void WeatherDrawer::drawCloud(int16_t x, int16_t y, int16_t height, Weather weather, bool clearInside, int16_t linesize)
{
    // NOTE: width is around height * 2
    const int16_t size = height * 0.25;
    const int16_t leftBottomCircleX = x + size;
    const int16_t leftBottomCircleY = y + size * 3;
    const int16_t rightBottomCircleX = x + size * 7;
    const int16_t rightBottomCircleY = leftBottomCircleY;
    const int16_t leftUpperCircleX = x + size * 3;
    const int16_t leftUpperCircleY = y + size * 2;
    const int16_t rightUpperCircleX = x + size * 5;
    const int16_t rightUpperCircleY = y + size * 1.7 + 1;
    const int16_t rectLinesX = x + size - 1;
    const int16_t rectLinesY = y + size * 2;

    Screen::mDisplay.fillCircle(leftBottomCircleX, leftBottomCircleY, size, GxEPD_BLACK);
    Screen::mDisplay.fillCircle(rightBottomCircleX, rightBottomCircleY, size, GxEPD_BLACK);
    Screen::mDisplay.fillCircle(leftUpperCircleX, leftUpperCircleY, size * 1.4, GxEPD_BLACK);
    Screen::mDisplay.fillCircle(rightUpperCircleX, rightUpperCircleY, size * 1.75, GxEPD_BLACK);
    Screen::mDisplay.fillRect(rectLinesX, rectLinesY, size * 6, size * 2 + 1, GxEPD_BLACK);
    if (clearInside) {
        Screen::mDisplay.fillCircle(leftBottomCircleX, leftBottomCircleY, size - linesize, GxEPD_WHITE);
        Screen::mDisplay.fillCircle(rightBottomCircleX, rightBottomCircleY, size - linesize, GxEPD_WHITE);
        Screen::mDisplay.fillCircle(leftUpperCircleX, leftUpperCircleY, size * 1.4 - linesize, GxEPD_WHITE);
        Screen::mDisplay.fillCircle(rightUpperCircleX, rightUpperCircleY, size * 1.75 - linesize, GxEPD_WHITE);
        Screen::mDisplay.fillRect(rectLinesX + 1, rectLinesY + linesize, size * 5.9, size * 2 - linesize * 2 + 1, GxEPD_WHITE);
    }
}

void WeatherDrawer::drawWeatherUnderCloud(int16_t x, int16_t y, int16_t height, Weather weather, int16_t linesize)
{
    switch (weather) {
    case Rain: {
        const int16_t rainDropHeight = height * 0.35;
        const int16_t topRowStartingX = x + rainDropHeight * 1.1;
        const int16_t bottomRowStartingX = x + rainDropHeight * 0.5;
        for (int16_t i = 0; i < 5; i++) {
            drawRainDrop(topRowStartingX + i * rainDropHeight * 1.7, y, rainDropHeight, linesize);
            drawRainDrop(bottomRowStartingX + i * rainDropHeight * 1.65, y + rainDropHeight * 1.5, rainDropHeight, linesize);
        }
        break;
    }
    case Rainstorm: {
        const int16_t stormWidth = height * 1.2;
        const int16_t leftX = x + (height * 2 - stormWidth);
        Screen::mDisplay.fillRect(leftX, y - linesize, stormWidth, linesize * 2, GxEPD_WHITE);
        drawThunder(leftX + stormWidth * 0.2, y - height * 0.5, height * 0.7, linesize);
        drawThunder(leftX + stormWidth * 0.5, y - height * 0.55, height * 1, linesize);
        const int16_t rainDropHeight = height * 0.35;
        const int16_t topRowStartingX = x + rainDropHeight;
        for (int16_t i = 0; i < 5; i++) {
            drawRainDrop(topRowStartingX + i * rainDropHeight * 1.5, y + rainDropHeight, rainDropHeight, linesize);
        }
        break;
    }
    case Storm: {
        const int16_t stormWidth = height * 1.2;
        const int16_t leftX = x + (height * 2 - stormWidth);
        Screen::mDisplay.fillRect(leftX, y - linesize, stormWidth, linesize * 2, GxEPD_WHITE);
        drawThunder(leftX + stormWidth * 0.1, y - height * 0.5, height, linesize);
        drawThunder(leftX + stormWidth * 0.5, y - height * 0.5, height * 1.35, linesize);
        break;
    }
    case Snow: {
        const int16_t snowWidth = height * 0.8;
        const int16_t leftX = x + (height * 2.5 - snowWidth);
        Screen::mDisplay.fillRect(leftX, y - linesize, snowWidth, linesize * 2, GxEPD_WHITE);

        const int16_t bigSnowFlakeLinesize = max((int16_t)1, (int16_t)(linesize - 1));
        const int16_t smallSnowFlakeLinesize = max((int16_t)1, (int16_t)(linesize - 2));
        drawSnowFlake(leftX + height * 0.1, y - height * 0.3, height * 0.5, bigSnowFlakeLinesize);
        drawSnowFlake(leftX - height * 0.6, y + height * 0.1, height * 0.5, bigSnowFlakeLinesize);
        drawSnowFlake(leftX, y + height * 0.3, height * 0.45, smallSnowFlakeLinesize);
        drawSnowFlake(leftX - height * 1.2, y + height * 0.3, height * 0.45, smallSnowFlakeLinesize);
        break;
    }
    case SnowWithRain: {
        const int16_t leftX = x + height * 0.2;
        const int16_t snowFlakeLinesize = max((int16_t)1, (int16_t)(linesize - 2));
        const int16_t rainDropHeight = height * 0.35;
        const int16_t rowY = y + rainDropHeight;
        for (int16_t i = 0; i < 3; i++) {
            const int16_t currentLeftX = leftX + height * 1.1 * i;
            drawRainDrop(currentLeftX, rowY - rainDropHeight * 0.3, rainDropHeight * 1.2, linesize);
            drawSnowFlake(currentLeftX + height * 0.2, rowY, rainDropHeight * 1.5, snowFlakeLinesize);
        }
        break;
    }
    case WindyCloud: {
        const int16_t windWidth = height * 0.5;
        const int16_t leftX = x + (height * 2);
        Screen::mDisplay.fillRect(leftX, y - height * 0.75, windWidth + height * 0.7, height * 0.9, GxEPD_WHITE);
        drawWind(leftX - height * 0.2, y - height * 0.75, height * 2, linesize);
        break;
    }
    default:
        break;
    }
}

void WeatherDrawer::drawThunder(int16_t x, int16_t y, int16_t height, int16_t linesize)
{
    const int16_t middleLineWidth = height * 0.3;
    const int16_t middleLineY = y + height * 0.45;
    const int16_t middleLineRightX = x + middleLineWidth;
    for (int i = 0; i < linesize; i++) {
        Screen::mDisplay.drawLine(x + middleLineWidth * 0.5 + i, y, x + i, middleLineY, GxEPD_BLACK);
        Screen::mDisplay.drawLine(x + i, middleLineY - i, middleLineRightX, middleLineY - i, GxEPD_BLACK);
        Screen::mDisplay.drawLine(middleLineRightX + i, middleLineY - i, x + middleLineWidth * 0.3 + i, y + height, GxEPD_BLACK);
    }
}

void WeatherDrawer::drawRainDrop(int16_t x, int16_t y, int16_t height, int16_t linesize)
{
    for (int16_t i = 0; i < linesize; i++) {
        Screen::mDisplay.drawLine(x + i, y + height * 0.5, x - height * 0.5 + i, y + height * 1.5, GxEPD_BLACK);
    }
}

void WeatherDrawer::drawSnowFlake(int16_t x, int16_t y, int16_t height, int16_t linesize)
{
    const int16_t halfSize = height * 0.5;
    const int16_t middleX = x + halfSize;
    const int16_t middleY = y + halfSize;
    // Values for diagonal lines
    const int16_t diagonalLineSize = halfSize / 1.4;
    const int16_t diagonalLeftX = (middleX - diagonalLineSize);
    const int16_t diagonalRightX = (middleX + diagonalLineSize);
    const int16_t diagonalTopY = (middleY - diagonalLineSize);
    const int16_t diagonalBottomY = (middleY + diagonalLineSize);
    for (int i = 0; i < linesize; i++) {
        // left and right lines
        Screen::mDisplay.fillRect(x, middleY, height, linesize, GxEPD_BLACK);
        // top and bottom lines
        Screen::mDisplay.fillRect(middleX, y, linesize, height, GxEPD_BLACK);
        for (int16_t i = 0; i < linesize + 1; i++) {
            // top left to bottom right lines
            Screen::mDisplay.drawLine(i + diagonalLeftX, diagonalTopY, i + diagonalRightX, diagonalBottomY, GxEPD_BLACK);
            // bottom left to top right lines
            Screen::mDisplay.drawLine(i + diagonalLeftX, diagonalBottomY, i + diagonalRightX, diagonalTopY, GxEPD_BLACK);
        }
    }
}

void WeatherDrawer::drawSun(int16_t x, int16_t y, int16_t height, int16_t linesize)
{
    const int16_t halfSize = height * 0.5;
    const int16_t middleX = x + halfSize;
    const int16_t middleY = y + halfSize;
    const int16_t circleRadius = height * 0.2;
    const int16_t offsetBetweenCenterAndLines = circleRadius * 1.5;

    drawSnowFlake(x, y, height, linesize);

    // Create offset between sun circle and lines
    Screen::mDisplay.fillCircle(middleX, middleY, offsetBetweenCenterAndLines, GxEPD_WHITE);
    // Drawing circle and clearing inside
    Screen::mDisplay.fillCircle(middleX, middleY, circleRadius, GxEPD_BLACK);
    Screen::mDisplay.fillCircle(middleX, middleY, circleRadius - linesize, GxEPD_WHITE);
}

void WeatherDrawer::drawFog(int16_t x, int16_t y, int16_t height, int16_t linesize)
{
    const int16_t lineCount = 8;
    // NOTE!! make sure that arrays has same length as lineCount
    const float widthOffsets[8] = { 0.05, 0.1, 0.2, 0.3, 0.4, 0.35, 0.2, 0.25 };
    const float widthMultipliers[8] = { 0.9, 0.75, 0.7, 0.65, 0.45, 0.35, 0.25, 0.15 };
    const int16_t spacing = (height - (lineCount * linesize)) / lineCount;
    int16_t width = 0;

    for (int i = 0 ; i < lineCount ; i++) {
        const int16_t xPos = x + height * widthOffsets[i];
        y += spacing;
        width = height * widthMultipliers[i];
        Screen::mDisplay.fillRect(xPos, y, width, linesize, GxEPD_BLACK);
    }
}

void WeatherDrawer::drawWind(int16_t x, int16_t y, int16_t height, int16_t linesize) 
{
    const int16_t spacing = (height - (4 * linesize)) / 8;
    // Initial offset
    x += height * 0.1;
    y += spacing;
    // First small line
    Screen::mDisplay.fillRect(x + height * 0.05, y, height * 0.2, linesize, GxEPD_BLACK);
    y += spacing;
    // Second Line with arc to the top
    int16_t radius = height * 0.08;
    int16_t circleXPos = x + height * 0.6;
    int16_t circleYPos = y - radius;
    Screen::mDisplay.fillCircle(circleXPos, circleYPos, radius, GxEPD_BLACK);
    Screen::mDisplay.fillCircle(circleXPos, circleYPos, radius - linesize, GxEPD_WHITE);
    // Clear part of the circle
    Screen::mDisplay.fillRect(circleXPos - radius, circleYPos, radius, radius, GxEPD_WHITE);
    Screen::mDisplay.fillRect(x + height * 0.1, y, height * 0.5, linesize, GxEPD_BLACK);
    y += spacing;
    // Two small ines
    Screen::mDisplay.fillRect(x + height * 0.15, y, height * 0.25, linesize, GxEPD_BLACK);
    Screen::mDisplay.fillRect(x + height * 0.45, y, height * 0.1, linesize, GxEPD_BLACK);
    y += spacing;
    // Line with arc to bottom
    circleXPos = x + height * 0.5;
    circleYPos = y + radius;
    Screen::mDisplay.fillCircle(circleXPos, circleYPos, radius, GxEPD_BLACK);
    Screen::mDisplay.fillCircle(circleXPos, circleYPos, radius - linesize, GxEPD_WHITE);
    // Clear part of the circle
    Screen::mDisplay.fillRect(circleXPos - radius, circleYPos - radius, radius, radius * 2, GxEPD_WHITE);
    Screen::mDisplay.fillRect(x, y, height * 0.5, linesize, GxEPD_BLACK);
    y += spacing;
    // Small line
    Screen::mDisplay.fillRect(x + height * 0.25, y, height * 0.15, linesize, GxEPD_BLACK);

}

String WeatherDrawer::weatherToString(Weather weather) const
{
    switch (weather) {
    case Sunny:
        return "Sunny";
    case LowCloudy:
        return "LowCloudy";
    case FullCloudy:
        return "FullCloudy";
    case Rain:
        return "Rain";
    case Storm:
        return "Storm";
    case Rainstorm:
        return "Rainstorm";
    case Snow:
        return "Snow";
    case SnowWithRain:
        return "SnowWithRain";
    case Fog:
        return "Fog";
    case Windy:
        return "Windy";
    case WindyCloud:
        return "WindyCloud";
    }
    return "UNKNOWN";
}

String WeatherDrawer::moonToString(MoonPhase moon) const
{
    switch (moon) {
    case NewMoon:
        return "NewMoon";
    case WaxingCrescent:
        return "WaxingCrescent";
    case FirstQuarter:
        return "FirstQuarter";
    case WaxingGibbous:
        return "WaxingGibbous";
    case FullMoon:
        return "FullMoon";
    case WaningGibbous:
        return "WaningGibbous";
    case LastQuarter:
        return "LastQuarter";
    case WaningCrescent:
        return "LastQuarter";
    }
    return "UNKNOWN";
}

WeatherDrawer::Weather WeatherDrawer::findMostCommonWeather(const std::vector<uint16_t> &weatherIds) const
{
    std::map<uint16_t, uint16_t> counters;
    uint16_t currentValue = 0;
    uint16_t currentMaxValueCount = 0;
    for (uint16_t id : weatherIds) {
        ++counters[id];
        if (currentMaxValueCount < counters[id]) {
            currentMaxValueCount = 1;
            currentValue = id;
        }
    }
    return weatherIdToEnum(currentValue);
}

WeatherDrawer::Weather WeatherDrawer::weatherIdToEnum(uint16_t id) const
{
    // Check https://openweathermap.org/weather-conditions for code descriptions
    if (id >= 200 && id < 300) { // Thunderstorm
        if (id < 210 || id >= 230 ) {
            return Rainstorm;
        }
        return Storm;
    } else if (id >= 300 && id < 400) { // Drizzle
        return Rain; 
    } else if (id >= 500 && id < 600) { // Rain
        return Rain;
    } else if (id >= 600 && id < 700) { // Snow
        if (id > 610 && id < 620) {
            return SnowWithRain;
        }
        return Snow;
    } else if (id >= 700 && id < 800) { // Atmosphere
        return Fog;
    } else if (id > 800) { // Clouds
        if (id <= 802) {
            return LowCloudy;
        }
        return FullCloudy;
    }
    return Sunny;
}