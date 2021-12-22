#include "screen.h"
#include "http.h"
#include "timeUpdate.h"
#include "sleep.h"

#include <SPI.h>
#include <WiFi.h>                     // Built-in
#include "time.h"                     // Built-in

//################ PROGRAM VARIABLES and OBJECTS ################
#define max_readings 24
#define BUILTIN_LED 13

#define autoscale_on  true
#define autoscale_off false
#define barchart_on   true
#define barchart_off  false

float pressure_readings[max_readings]    = {0};
float temperature_readings[max_readings] = {0};
float humidity_readings[max_readings]    = {0};
float rain_readings[max_readings]        = {0};
float snow_readings[max_readings]        = {0};

int32_t mCounter = 0;

//#########################################################################################
void setup() {
    Serial.begin(115200);
    // pinMode(BUILTIN_LED, OUTPUT);
    // digitalWrite(BUILTIN_LED, LOW);
    initialiseDisplay();

    // debugPrintWeather(FullCloudy);
    debugPrintAllWeathersAndMoons();

    display.display();
}

//#########################################################################################
void loop() { // this will never run!
}

//################################### DEBUG ######################################################
void debugPrintAllWeathersAndMoons() {
    const int lastWeather = (int)Fog;
    const int spacing = 10;
    int x = spacing;
    int y = spacing * 2;
    const int cellHeight = 80;
    const int weatherHeight = 60;
    const int imageWidth = weatherHeight * 2 + spacing;
    display.drawLine(0, y, SCREEN_WIDTH, y, GxEPD_BLACK);
    bool isFirstRow = true;
    for (int i = 0; i <= (int)Fog; i++) {
        drawString(x + 2, y - (cellHeight - weatherHeight), weatherToString((Weather)i));
        drawWeather(x, y, weatherHeight, (Weather)i);
        if (isFirstRow) {
            display.drawLine(x, 0, x, SCREEN_HEIGHT, GxEPD_BLACK);
        }
        x += imageWidth;
        if (x + imageWidth > SCREEN_WIDTH) {
            isFirstRow = false;
            display.drawLine(0, y + weatherHeight, SCREEN_WIDTH, y + weatherHeight, GxEPD_BLACK);
            x = spacing;
            y += cellHeight + spacing;
            display.drawLine(0, y, SCREEN_WIDTH, y, GxEPD_BLACK);
        }
    }
    display.drawLine(0, y + weatherHeight, SCREEN_WIDTH, y + weatherHeight, GxEPD_BLACK);
    const int16_t moonSpacing = spacing * 0.5;
    y += cellHeight + spacing + moonSpacing;
    x = spacing + moonSpacing;
    for (int i = 0; i <= WaningCrescent; i++) {
        drawString(x + 2, y - (cellHeight - weatherHeight), moonToString((MoonPhase)i));
        drawMoon(x, y, weatherHeight, (MoonPhase)i);
        x += imageWidth;
        if (x + imageWidth > SCREEN_WIDTH) {
            display.drawLine(0, y - moonSpacing, SCREEN_WIDTH, y - moonSpacing, GxEPD_BLACK);
            display.drawLine(0, y + weatherHeight + moonSpacing * 2, SCREEN_WIDTH, y + weatherHeight + moonSpacing * 2, GxEPD_BLACK);
            x = spacing + moonSpacing;
            y += cellHeight + spacing + moonSpacing * 2;
        }
    }
    display.drawLine(0, y, SCREEN_WIDTH, y, GxEPD_BLACK);
}

void debugPrintWeather(int w1, int w2 = -1, int w3 = -1, int w4 = -1) {
    int16_t x0 = 100;
    int16_t y0 = 100;
    int16_t x1 = 350;
    int16_t y1 = 300;

    int16_t height = 80;
    if (w1 >= 0)
        drawWeather(x0, y0, height, (Weather)w1);
    if (w2 >= 0)
        drawWeather(x0, y1, height, (Weather)w2);
    if (w3 >= 0)
        drawWeather(x1, y0, height, (Weather)w3);
    if (w4 >= 0)
        drawWeather(x1, y0, height, (Weather)w4);

    display.drawLine(0, y0, SCREEN_WIDTH, y0, GxEPD_BLACK); // test line (horizontal)
    display.drawLine(0, y1, SCREEN_WIDTH, y1, GxEPD_BLACK); // test line (horizontal)
    display.drawLine(x0, 0, x0, SCREEN_HEIGHT, GxEPD_BLACK); // test line (vertical)
    display.drawLine(x1, 0, x1, SCREEN_HEIGHT, GxEPD_BLACK); // test line (vertical)
}