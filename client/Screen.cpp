#include "Screen.h"

#include "NetworkManager.h"
#include "JsonParser.h"
#include "Logger.h"

#include <SPI.h>
#include "WiFiType.h"
#include <functional>

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
    setFontSize(-1); // Use default font size
    // mDisplay.setFont(u8g2Fonts);
    mDisplay.fillScreen(GxEPD_WHITE);
    mDisplay.setFullWindow();
    DEBUG("Display initialization finished.");
}

void Screen::display()
{
    mDisplay.display();
}

void Screen::setFontSize(uint16_t size)
{
    // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
    switch(size)
    {
    case 8:
        u8g2Fonts.setFont(u8g2_font_helvB08_tf);
        return;
    case 10:
        u8g2Fonts.setFont(u8g2_font_helvB10_tf);
        return;
    case 12:
        u8g2Fonts.setFont(u8g2_font_helvB12_tf);
        return;
    case 14:
        u8g2Fonts.setFont(u8g2_font_helvB14_tf);
        return;
    case 18:
        u8g2Fonts.setFont(u8g2_font_helvB18_tf);
        return;
    case 24:
        u8g2Fonts.setFont(u8g2_font_helvB24_tf);
        return;
    default:
        u8g2Fonts.setFont(u8g2_font_helvB10_tf);
        return;
    }
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

Rect Screen::drawString(int16_t x, int16_t y, const String &text, Alignment horizontalAlignment, Alignment verticalAlignment)
{
    Rect boundingRect;
    // DEBUG("Printing text: '" + text + "'");
    if (text.isEmpty()) {
        return boundingRect;
    }
    mDisplay.setTextWrap(false);
    mDisplay.getTextBounds(text, x, y, &boundingRect.x, &boundingRect.y, &boundingRect.w, &boundingRect.h);
    switch (horizontalAlignment) {
    case RIGHT:
        x -= boundingRect.w;
        break;
    case CENTER:
        x -= boundingRect.w / 2;
        break;
    default:
        break;
    }
    switch (verticalAlignment) {
    case TOP:
        y += boundingRect.h;
    case CENTER:
        y += boundingRect.h / 2;
        break;
    default:
        break;
    }
    u8g2Fonts.setCursor(x, y);
    u8g2Fonts.print(text);
    return boundingRect;
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

void Screen::drawError(const String &text)
{
    DEBUG("ERROR: " + text);
    // TODO draw error
}

bool Screen::updateScreenData()
{
    NetworkManager networkManager;
    // connect to wifi
    const bool connectedToWifi = networkManager.connectToWifi() == WL_CONNECTED;
    // draw status
    statusDrawer()->drawStatus(networkManager.signalStrength(), "Ver: 0.1 [debug]");
    if (!connectedToWifi) {
        drawError("WiFi disconnected!");
        return false;
    }
    // send request
    networkManager.requestDataToDraw();
    if (networkManager.response().isEmpty()) {
        drawError("Response is empty");
        return false;
    }
    // parse response
    JsonParser parser;
    if (!parser.parse(networkManager.response())) {
        drawError("Response parsing error!");
        return false;
    }
    // update screen using data
    const JsonData &data = parser.data();
    statusDrawer()->drawTime(data.time);

    // Drow home plots
    const uint16_t homePlotsSize = data.homePlots.size();
    uint16_t homePlotsWidth = 0;
    if (homePlotsSize > 0) {
        const uint16_t homePlotsY = statusDrawer()->statusBarHeight() + 5;
        const uint16_t homePlotsHeight = (SCREEN_HEIGHT - homePlotsY) / homePlotsSize;
        const uint16_t homePlotsX = SCREEN_WIDTH - homePlotsHeight - 5;
        homePlotsWidth = homePlotsHeight;
        for (size_t i = 0 ; i < homePlotsSize ; i++) {
            plotDrawer()->drawLinePlot(homePlotsX, homePlotsY + i * homePlotsHeight, homePlotsWidth, homePlotsHeight, data.homePlots[i]);
        }
    }

    mDisplay.drawRect(SCREEN_WIDTH - homePlotsWidth, statusDrawer()->statusBarHeight(), 1, SCREEN_HEIGHT - statusDrawer()->statusBarHeight(), GxEPD_BLACK);
    
    // Draw sensor strings
    const uint16_t sensorListSize = data.sensors.size();
    const uint16_t sensorRectHeight = homePlotsWidth * 0.45;
    const uint16_t sensorRectY = SCREEN_HEIGHT - sensorRectHeight;
    if (sensorListSize > 0) {
        const uint16_t sensorRectWidth = (SCREEN_WIDTH - homePlotsWidth) / sensorListSize;
        const uint16_t sensorTextSpacing = sensorRectHeight * 0.05;
        const uint16_t sensorHumidityYPos = sensorRectY + sensorRectHeight - 7;
        const uint16_t sensorTemperatureYPos = sensorHumidityYPos - 25;
        for(size_t i = 0 ; i < sensorListSize ; i++) {
            const JsonSensorData &sensorData = data.sensors[i];
            uint16_t currentSensorX = i * sensorRectWidth;
            mDisplay.drawRect(currentSensorX, sensorRectY, sensorRectWidth, sensorRectHeight, GxEPD_BLACK);

            currentSensorX += 5;
            setFontSize(14);
            drawString(currentSensorX, sensorRectY + 10, sensorData.name);
            if (!sensorData.description.isEmpty()) {
                setFontSize(10);
                drawString(currentSensorX, sensorRectY + 25, sensorData.description);
            }
            setFontSize(18);
            drawString(currentSensorX, sensorHumidityYPos, String(sensorData.humidity), LEFT, BOTTOM);
            setFontSize(24);
            drawString(currentSensorX, sensorTemperatureYPos, String(sensorData.temperature), LEFT, BOTTOM);
            setFontSize(-1);

            drawString(currentSensorX + 60, sensorHumidityYPos - 20, "%");
            drawString(currentSensorX + 82, sensorTemperatureYPos - 25, "°C");
        }
    }

    if (!data.weatherData.tempPlotData.series.empty()) {
        const uint16_t forecastWidth = SCREEN_WIDTH - homePlotsWidth;
        const uint16_t forecastHeight = homePlotsWidth;
        const uint16_t forecastY = sensorRectY - forecastHeight;
        const uint16_t forecastWeatherIconsHeight = forecastHeight * 0.15;
        const uint16_t forecastDiagramY = forecastY + forecastWeatherIconsHeight;
        const uint16_t forecastDiagramHeight = forecastHeight - forecastWeatherIconsHeight;
        Screen::mDisplay.drawRect(0, forecastY, forecastWidth, forecastHeight, GxEPD_BLACK); // DEBUG rect
        plotDrawer()->drawForecastPlot(0, forecastDiagramY, forecastWidth, forecastDiagramHeight, data.weatherData.tempPlotData, data.weatherData.rainPlotData, data.weatherData.weatherIds);

        const uint16_t weatherIconCount = 10;
        const uint16_t actualWeatherIconHeight = forecastWeatherIconsHeight * 0.7;
        const uint16_t weatherIconSpacing = forecastWidth * 0.9 / weatherIconCount;
        const uint16_t weatherIconXOffset = weatherIconSpacing;
        // Forecast has 40 ticks
        for (int i = 0 ; i < weatherIconCount ; i++) {
            weatherDrawer()->drawWeather(weatherIconXOffset + i * weatherIconSpacing, forecastY, actualWeatherIconHeight, WeatherDrawer::FullCloudy);
        }
    }

    return true;
}