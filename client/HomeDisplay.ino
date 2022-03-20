#include "Screen.h"

#define BUILTIN_LED 13

// Connections for Adafruit ESP32 Feather
static const uint8_t EPD_BUSY = 32; // to EPD BUSY
static const uint8_t EPD_CS = 15; // to EPD CS
static const uint8_t EPD_RST = 27; // to EPD RST
static const uint8_t EPD_DC = 33; // to EPD DC
static const uint8_t EPD_SCK = 5; // to EPD CLK
static const uint8_t EPD_MISO = 19; // Master-In Slave-Out not used, as no data from mDisplay
static const uint8_t EPD_MOSI = 18; // to EPD DIN

//#########################################################################################
void setup()
{
    Serial.begin(115200);

    changeLed(true);

    Screen screen;

    // debugPrintAllWeathersAndMoons(screen);
    // debugDrawPlot(screen);
}

//#########################################################################################
void loop()
{ // this will never run!
}

void changeLed(bool show)
{
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, show ? HIGH : LOW);
}

//################################### DEBUG ######################################################
void debugDrawPlot(Screen &screen) 
{
    std::vector<float> data = { 1.5, 1.5, 3.0, 3.5, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 13.0, 12.0, 11.0, 10.0, 9.0, 20.0 };
    std::vector<float> data2 = { 1.5, 1.5, 3.0, 3.5, 5.0, 6.0, 12.0, 11.0, 10.0, 9.0, 20.0 };
    std::vector<float> yValues = { 0.0, 6.0, 12.0, 18.0, 24.0 };
    screen.plotDrawer()->drawPlot(40, 20, 250, 300, "Line graph", yValues, data, false);
    screen.plotDrawer()->drawPlot(400, 20, 250, 300, "Bar graph", yValues, data, true);
}

void debugPrintAllWeathersAndMoons(Screen &screen) 
{
    const int lastWeather = (int)WeatherDrawer::Fog;
    const int spacing = 10;
    int x = spacing;
    int y = spacing * 2;
    const int cellHeight = 80;
    const int weatherHeight = 60;
    const int imageWidth = weatherHeight * 2 + spacing;
    screen.drawLine(0, y, SCREEN_WIDTH, y);
    bool isFirstRow = true;
    for (int i = 0; i <= (int)WeatherDrawer::Fog; i++) {
        screen.drawString(x + 2, y - (cellHeight - weatherHeight), screen.weatherDrawer()->weatherToString((WeatherDrawer::Weather)i));
        screen.weatherDrawer()->drawWeather(x, y, weatherHeight, (WeatherDrawer::Weather)i);
        if (isFirstRow) {
            screen.drawLine(x, 0, x, SCREEN_HEIGHT);
        }
        x += imageWidth;
        if (x + imageWidth > SCREEN_WIDTH) {
            isFirstRow = false;
            screen.drawLine(0, y + weatherHeight, SCREEN_WIDTH, y + weatherHeight, GxEPD_BLACK);
            x = spacing;
            y += cellHeight + spacing;
            screen.drawLine(0, y, SCREEN_WIDTH, y, GxEPD_BLACK);
        }
    }
    screen.drawLine(0, y + weatherHeight, SCREEN_WIDTH, y + weatherHeight, GxEPD_BLACK);
    const int16_t moonSpacing = spacing * 0.5;
    y += cellHeight + spacing + moonSpacing;
    x = spacing + moonSpacing;
    for (int i = 0; i <= WeatherDrawer::WaningCrescent; i++) {
        screen.drawString(x + 2, y - (cellHeight - weatherHeight), screen.weatherDrawer()->moonToString((WeatherDrawer::MoonPhase)i));
        screen.weatherDrawer()->drawMoon(x, y, weatherHeight, (WeatherDrawer::MoonPhase)i);
        x += imageWidth;
        if (x + imageWidth > SCREEN_WIDTH) {
            screen.drawLine(0, y - moonSpacing, SCREEN_WIDTH, y - moonSpacing);
            screen.drawLine(0, y + weatherHeight + moonSpacing * 2, SCREEN_WIDTH, y + weatherHeight + moonSpacing * 2);
            x = spacing + moonSpacing;
            y += cellHeight + spacing + moonSpacing * 2;
        }
    }
    screen.drawLine(0, y, SCREEN_WIDTH, y, GxEPD_BLACK);
}