#include "screen.h"

#include "BoardController.h"

#include <SPI.h>

#define SMALL_CLOUD_SIZE 6
#define BIG_CLOUD_SIZE 17
#define SMALL_ICON_SIZE 6
#define BIG_ICON_SIZE 17

namespace 
{
    // Connections for Adafruit ESP32 Feather
    const uint8_t EPD_BUSY = 32; // to EPD BUSY
    const uint8_t EPD_CS = 15; // to EPD CS
    const uint8_t EPD_RST = 27; // to EPD RST
    const uint8_t EPD_DC = 33; // to EPD DC
    const uint8_t EPD_SCK = 5; // to EPD CLK
    const uint8_t EPD_MISO = 19; // Master-In Slave-Out not used, as no data from mDisplay
    const uint8_t EPD_MOSI = 18; // to EPD DIN
}

GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> Screen::mDisplay = GxEPD2_750_T7(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY);
U8G2_FOR_ADAFRUIT_GFX Screen::u8g2Fonts;

Screen::Screen()
{
    mDisplay.init(115200, true, 2, false); // init(uint32_t serial_diag_bitrate, bool initial, uint16_t reset_duration, bool pulldown_rst_mode)
    SPI.end();
    SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);
    u8g2Fonts.begin(mDisplay); // connect u8g2 procedures to Adafruit GFX
    u8g2Fonts.setFontMode(1); // use u8g2 transparent mode (this is default)
    u8g2Fonts.setFontDirection(0); // left to right (this is default)
    u8g2Fonts.setForegroundColor(GxEPD_BLACK); // apply Adafruit GFX color
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // apply Adafruit GFX color
    u8g2Fonts.setFont(u8g2_font_helvB10_tf); // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
    mDisplay.fillScreen(GxEPD_WHITE);
    mDisplay.setFullWindow();
    BoardController::debug("Display initialization finished.");
}

Screen::~Screen()
{
    display();
}

void Screen::display()
{
    mDisplay.display();
}

void Screen::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t width, Alignment direction, uint16_t color)
{
    for (uint16_t i = 1 ; i <= width ; i++) {
        switch(direction) {
        case TOP:
            y0--;
            y1--;
            break;
        case BOTTOM:
            y0++;
            y1++;
            break;
        case LEFT:
            x0--;
            x1--;
            break;
        default:
            x0++;
            x1++;
            break;
        }
        drawLine(x0, y0, x1, y1, color);
    }
}

void Screen::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    mDisplay.drawLine(x0, y0, x1, y1, color);
}

void Screen::drawString(int16_t x, int16_t y, const String &text, Alignment horizontalAlignment, Alignment verticalAlignment)
{
    BoardController::debug("Printing text: '" + text + "'");
    int16_t x1, y1; // the bounds of x,y and w and h of the variable 'text' in pixels.
    uint16_t w, h;
    mDisplay.setTextWrap(false);
    mDisplay.getTextBounds(text, x, y, &x1, &y1, &w, &h);
    switch (horizontalAlignment) {
    case RIGHT:
        x -= w;
        break;
    case CENTER:
        x -= w / 2;
        break;
    default:
        break;
    }
    switch (verticalAlignment) {
    case TOP:
        y += h;
    case CENTER:
        y += h / 2;
        break;
    default:
        break;
    }
    u8g2Fonts.setCursor(x, y);
    u8g2Fonts.print(text);
}

WeatherDrawer* Screen::weatherDrawer() 
{
    return &mWeatherDrawer;
}

PlotDrawer* Screen::plotDrawer()
{
    return &mPlotDrawer;
}

StatusDrawer* Screen::statusDrawer()
{
    return &mStatusDrawer;
}