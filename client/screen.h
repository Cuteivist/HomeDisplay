#include <U8g2_for_Adafruit_GFX.h>
#include <GxEPD2_BW.h>

// Connections for Adafruit ESP32 Feather
static const uint8_t EPD_BUSY = 32; // to EPD BUSY
static const uint8_t EPD_CS = 15;   // to EPD CS
static const uint8_t EPD_RST = 27;  // to EPD RST
static const uint8_t EPD_DC = 33;   // to EPD DC
static const uint8_t EPD_SCK = 5;   // to EPD CLK
static const uint8_t EPD_MISO = 19; // Master-In Slave-Out not used, as no data from display
static const uint8_t EPD_MOSI = 18; // to EPD DIN

#define SCREEN_WIDTH 800 // Set for landscape mode
#define SCREEN_HEIGHT 480

#define SMALL_CLOUD_SIZE 6
#define BIG_CLOUD_SIZE 17
#define SMALL_ICON_SIZE 6
#define BIG_ICON_SIZE 17

enum Alignment {
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    CENTER
};

enum Weather {
    Sunny,
    LowCloudy,
    HighCloudy,
    FullCloudy,
    Rain,
    Storm,
    Rainstorm,
    Snow,
    SnowWithRain,
    Fog
};

enum MoonPhase {
    NewMoon,
    WaxingCrescent, // Light crescent on the right
    FirstQuarter, // Right side is light
    WaxingGibbous, // Black crescent on the eleft
    FullMoon,
    WaningGibbous, // Black crescent on the right
    LastQuarter, // Left side is light
    WaningCrescent // Light crescent on the left
};

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts; // Select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display(GxEPD2_750_T7(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

// ########################### SCREEN ###########################
void initialiseDisplay() {
    Serial.write("Initializing display...");
    display.init(115200, true, 2, false); // init(uint32_t serial_diag_bitrate, bool initial, uint16_t reset_duration, bool pulldown_rst_mode)
    SPI.end();
    SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);
    u8g2Fonts.begin(display);                  // connect u8g2 procedures to Adafruit GFX
    u8g2Fonts.setFontMode(1);                  // use u8g2 transparent mode (this is default)
    u8g2Fonts.setFontDirection(0);             // left to right (this is default)
    u8g2Fonts.setForegroundColor(GxEPD_BLACK); // apply Adafruit GFX color
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // apply Adafruit GFX color
    u8g2Fonts.setFont(u8g2_font_helvB10_tf);   // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
    display.fillScreen(GxEPD_WHITE);
    display.setFullWindow();
    Serial.write("Display initialization finished.\n");
}

// ########################### STRING ###########################
void drawString(int x, int y, String text, Alignment horizontalAlignment = LEFT, Alignment verticalAlignment = TOP) {
    Serial.write(String("Printing text: '" + text + "'\n").c_str());
    int16_t x1, y1; // the bounds of x,y and w and h of the variable 'text' in pixels.
    uint16_t w, h;
    display.setTextWrap(false);
    display.getTextBounds(text, x, y, &x1, &y1, &w, &h);
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
    // TODO add horizontal and vertical alignment as new function
    u8g2Fonts.setCursor(x, y);
    u8g2Fonts.print(text);
}

// ########################### CLOUD ###########################
void drawRainDrop(int x, int y, int scale);
void drawWeatherUnderCloud(int x, int y, int height, Weather weather);

void drawCloud(int x, int y, int height, Weather weather, int linesize = 2, bool clearInside = true) {
    // NOTE: width = height * 2
    const int size = height * 0.25;
    const int leftBottomCircleX = x + size;
    const int leftBottomCircleY = y + size * 3;
    const int rightBottomCircleX = x + size * 7;
    const int rightBottomCircleY = leftBottomCircleY;
    const int leftUpperCircleX = x + size * 3;
    const int leftUpperCircleY = y + size * 2;
    const int rightUpperCircleX = x + size * 5;
    const int rightUpperCircleY = y + size * 1.7 + 1;
    const int rectLinesX = x + size - 1;
    const int rectLinesY = y + size * 2;

    display.fillCircle(leftBottomCircleX, leftBottomCircleY, size, GxEPD_BLACK);
    display.fillCircle(rightBottomCircleX, rightBottomCircleY, size, GxEPD_BLACK);
    display.fillCircle(leftUpperCircleX, leftUpperCircleY, size * 1.4, GxEPD_BLACK);
    display.fillCircle(rightUpperCircleX, rightUpperCircleY, size * 1.75, GxEPD_BLACK);
    display.fillRect(rectLinesX, rectLinesY, size * 6, size * 2 + 1, GxEPD_BLACK);
    if (clearInside) {
        display.fillCircle(leftBottomCircleX, leftBottomCircleY, size - linesize, GxEPD_WHITE);
        display.fillCircle(rightBottomCircleX, rightBottomCircleY, size - linesize, GxEPD_WHITE);
        display.fillCircle(leftUpperCircleX, leftUpperCircleY, size * 1.4 - linesize, GxEPD_WHITE);
        display.fillCircle(rightUpperCircleX, rightUpperCircleY, size * 1.75 - linesize, GxEPD_WHITE);
        display.fillRect(rectLinesX + 1, rectLinesY + linesize, size * 5.9, size * 2 - linesize * 2 + 1, GxEPD_WHITE);
    }

    drawWeatherUnderCloud(x, y + height, height, weather);
}

void drawWeatherUnderCloud(int x, int y, int height, Weather weather) {
    // NOTE y should be bottom of the cloud
    switch (weather) {
    case Rain:
        for (int d = 0; d < 4; d++) {
            int scale = height * 0.2;
            drawRainDrop(x + scale * (7.8 - d * 1.95) - scale * 2, y + scale * 2.1 - scale / 6, scale / 1.6);
        }
        break;
    case Snow:
        break;
    default:
        break;
    }
}

// ###############################################################################
/// CODE TO UPDATE
void drawRainDrop(int x, int y, int scale) {
    display.fillCircle(x, y, scale / 2, GxEPD_BLACK);
    display.fillTriangle(x - scale / 2, y, x, y - scale * 1.2, x + scale / 2, y, GxEPD_BLACK);
    x = x + scale * 1.6;
    y = y + scale / 3;
    display.fillCircle(x, y, scale / 2, GxEPD_BLACK);
    display.fillTriangle(x - scale / 2, y, x, y - scale * 1.2, x + scale / 2, y, GxEPD_BLACK);
}

void drawSnow(int x, int y, int scale) {
    int dxo, dyo, dxi, dyi;
    for (int flakes = 0; flakes < 5; flakes++) {
        for (int i = 0; i < 360; i = i + 45) {
            // TOOD extract it to `drawSnowFlake`
            dxo = 0.5 * scale * cos((i - 90) * 3.14 / 180);
            dxi = dxo * 0.1;
            dyo = 0.5 * scale * sin((i - 90) * 3.14 / 180);
            dyi = dyo * 0.1;
            display.drawLine(dxo + x + flakes * 1.5 * scale - scale * 3, dyo + y + scale * 2, dxi + x + 0 + flakes * 1.5 * scale - scale * 3, dyi + y + scale * 2, GxEPD_BLACK);
        }
    }
}
void drawStorm(int x, int y, int scale) {
    y = y + scale / 2;
    for (int i = 0; i < 5; i++) {
        display.drawLine(x - scale * 4 + scale * i * 1.5 + 0, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 0, y + scale, GxEPD_BLACK);
        if (scale != SMALL_ICON_SIZE) {
            display.drawLine(x - scale * 4 + scale * i * 1.5 + 1, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 1, y + scale, GxEPD_BLACK);
            display.drawLine(x - scale * 4 + scale * i * 1.5 + 2, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 2, y + scale, GxEPD_BLACK);
        }
        display.drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 0, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 0, GxEPD_BLACK);
        if (scale != SMALL_ICON_SIZE) {
            display.drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 1, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 1, GxEPD_BLACK);
            display.drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 2, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 2, GxEPD_BLACK);
        }
        display.drawLine(x - scale * 3.5 + scale * i * 1.4 + 0, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5, GxEPD_BLACK);
        if (scale != SMALL_ICON_SIZE) {
            display.drawLine(x - scale * 3.5 + scale * i * 1.4 + 1, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 1, y + scale * 1.5, GxEPD_BLACK);
            display.drawLine(x - scale * 3.5 + scale * i * 1.4 + 2, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 2, y + scale * 1.5, GxEPD_BLACK);
        }
    }
}
void drawSun(int x, int y, int scale, bool isLarge) {
    int linesize = 3;
    if (!isLarge)
        linesize = 1;
    display.fillRect(x - scale * 2, y, scale * 4, linesize, GxEPD_BLACK);
    display.fillRect(x, y - scale * 2, linesize, scale * 4, GxEPD_BLACK);
    display.drawLine(x - scale * 1.3, y - scale * 1.3, x + scale * 1.3, y + scale * 1.3, GxEPD_BLACK);
    display.drawLine(x - scale * 1.3, y + scale * 1.3, x + scale * 1.3, y - scale * 1.3, GxEPD_BLACK);
    if (isLarge) {
        display.drawLine(1 + x - scale * 1.3, y - scale * 1.3, 1 + x + scale * 1.3, y + scale * 1.3, GxEPD_BLACK);
        display.drawLine(2 + x - scale * 1.3, y - scale * 1.3, 2 + x + scale * 1.3, y + scale * 1.3, GxEPD_BLACK);
        display.drawLine(3 + x - scale * 1.3, y - scale * 1.3, 3 + x + scale * 1.3, y + scale * 1.3, GxEPD_BLACK);
        display.drawLine(1 + x - scale * 1.3, y + scale * 1.3, 1 + x + scale * 1.3, y - scale * 1.3, GxEPD_BLACK);
        display.drawLine(2 + x - scale * 1.3, y + scale * 1.3, 2 + x + scale * 1.3, y - scale * 1.3, GxEPD_BLACK);
        display.drawLine(3 + x - scale * 1.3, y + scale * 1.3, 3 + x + scale * 1.3, y - scale * 1.3, GxEPD_BLACK);
    }
    display.fillCircle(x, y, scale * 1.3, GxEPD_WHITE);
    display.fillCircle(x, y, scale, GxEPD_BLACK);
    display.fillCircle(x, y, scale - linesize, GxEPD_WHITE);
}
void drawFog(int x, int y, int scale, int linesize, bool isLarge) {
    if (!isLarge) {
        y -= 10;
        linesize = 1;
    }
    for (int i = 0; i < 6; i++) {
        display.fillRect(x - scale * 3, y + scale * 1.5, scale * 6, linesize, GxEPD_BLACK);
        display.fillRect(x - scale * 3, y + scale * 2.0, scale * 6, linesize, GxEPD_BLACK);
        display.fillRect(x - scale * 3, y + scale * 2.5, scale * 6, linesize, GxEPD_BLACK);
    }
}