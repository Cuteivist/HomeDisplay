#ifndef SCREEN_H
#define SCREEN_H

#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>

#include "WeatherDrawer.h"
#include "PlotDrawer.h"

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

    // Display needs to be global to properly work
    static GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> mDisplay;

    void display();
    WeatherDrawer* weatherDrawer();
    PlotDrawer* plotDrawer();

    static void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y, uint16_t color = GxEPD_BLACK);
    static void drawString(int16_t x, int16_t y, String text, Alignment horizontalAlignment = LEFT, Alignment verticalAlignment = TOP);

private:
    WeatherDrawer mWeatherDrawer;
    PlotDrawer mPlotDrawer;
    static U8G2_FOR_ADAFRUIT_GFX u8g2Fonts; // Select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
};
#endif // SCREEN_H