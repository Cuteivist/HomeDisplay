#ifndef SCREEN_H
#define SCREEN_H

#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>

#define SCREEN_WIDTH 800 // Set for landscape mode
#define SCREEN_HEIGHT 480


class Screen {
public:
    Screen();
    ~Screen();
    enum Alignment {
        TOP,
        BOTTOM,
        LEFT,
        RIGHT,
        CENTER
    };

    enum Weather {
        Sunny = 0,
        LowCloudy,
        FullCloudy,
        Rain,
        Storm,
        Rainstorm,
        Snow,
        SnowWithRain,
        Fog
    };

    enum MoonPhase {
        NewMoon = 0,
        WaxingCrescent, // Light crescent on the right
        FirstQuarter, // Right side is light
        WaxingGibbous, // Black crescent on the eleft
        FullMoon,
        WaningGibbous, // Black crescent on the right
        LastQuarter, // Left side is light
        WaningCrescent // Light crescent on the left
    };

    void display();

    String weatherToString(Weather weather) const;
    String moonToString(MoonPhase weather) const;

    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y, uint16_t color = GxEPD_BLACK);
    void drawString(int16_t x, int16_t y, String text, Alignment horizontalAlignment = LEFT, Alignment verticalAlignment = TOP);
    void drawWeather(int16_t x, int16_t y, int16_t height, Weather weather);
    void drawMoon(int16_t x, int16_t y, int16_t height, MoonPhase phase, int16_t linesize = 2);

private:
    // Display needs to be global to properly work
    static GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> mDisplay;
    static U8G2_FOR_ADAFRUIT_GFX u8g2Fonts; // Select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall

    void drawCloud(int16_t x, int16_t y, int16_t height, Weather weather, bool clearInside = true, int16_t linesize = 2);
    void drawSun(int16_t x, int16_t y, int16_t height, int16_t linesize = 2);
    void drawRainDrop(int16_t x, int16_t y, int16_t height, int16_t linesize = 2);
    void drawSnowFlake(int16_t x, int16_t y, int16_t height, int16_t linesize = 2);
    void drawThunder(int16_t x, int16_t y, int16_t height, int16_t linesize = 2);
    void drawWeatherUnderCloud(int16_t x, int16_t y, int16_t height, Weather weather, int16_t linesize = 2);
};
#endif // SCREEN_H