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
void drawString(int x, int16_t y, String text, Alignment horizontalAlignment = LEFT, Alignment verticalAlignment = TOP) {
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
    u8g2Fonts.setCursor(x, y);
    u8g2Fonts.print(text);
}

// ########################### CLOUD ###########################
void drawRainDrop(int x, int16_t y, int16_t height, int16_t linesize);
void drawWeatherUnderCloud(int x, int16_t y, int16_t height, Weather weather, int16_t linesize);
void drawSnow(int x, int16_t y, int16_t height);
void drawCloud(int x, int16_t y, int16_t height, Weather weather, bool clearInside = true);
void drawSun(int x, int16_t y, int16_t height);

void drawWeather(int x, int16_t y, int16_t height, Weather weather) {
    switch(weather) {
        case Fog:
            // TODO draw fog
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
        case SnowWithRain:
        case HighCloudy:
        case FullCloudy:
        case Rain:
        case Storm:
        case Rainstorm:
        case Snow:
            drawCloud(x, y, height, weather, true);
        default:
            // TODO write error message
            return;
    }
}

void drawMoon(int x, int16_t y, int16_t height, MoonPhase phase) {
    // TODO
}

void drawCloud(int x, int16_t y, int16_t height, Weather weather, bool clearInside) {
    const int16_t linesize = 2;
    // NOTE: width = height * 2
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

    drawWeatherUnderCloud(x, y + height, height, weather, linesize);
}

void drawWeatherUnderCloud(int x, int16_t y, int16_t height, Weather weather, int16_t linesize) {
    // NOTE y should be bottom of the cloud
    switch (weather) {
    case Rain:
        for (int i = 0; i < 5; i++) {
            int16_t size = height * 0.175;
            drawRainDrop(x + size * 2 + i * size * 1.5, y, size, linesize);
            drawRainDrop(x + size * 1.25 + i * size * 1.5, y + size * 1.5, size, linesize);
        }
        break;
    case Snow:
        // drawSnow(x + height, y, height * 0.2);
        break;
    default:
        break;
    }
}

void drawRainDrop(int x, int16_t y, int16_t height, int16_t linesize) {
    for (int i = 0; i < linesize; i++) {
        display.drawLine(x + i, y + height * 0.5, x - height * 0.5 + i, y + height * 1.5, GxEPD_BLACK);
    }
}

void drawSun(int x, int16_t y, int16_t height) {
    const int16_t linesize = 3;
    const int16_t halfSize = height * 0.5;
    const int16_t middleX = x + halfSize;
    const int16_t middleY = y + halfSize;
    const int16_t circleRadius = height * 0.2;
    const int16_t offsetBetweenCenterAndLines = circleRadius * 1.5;

    // Values for diagonal lines
    const int16_t diagonalLineSize = halfSize / 1.4;
    const int16_t diagonalLeftX = (middleX - diagonalLineSize);
    const int16_t diagonalRightX = (middleX + diagonalLineSize);
    const int16_t diagonalTopY = (middleY - diagonalLineSize);
    const int16_t diagonalBottomY = (middleY + diagonalLineSize);

    // left and right lines
    display.fillRect(x, middleY, height, linesize, GxEPD_BLACK);
    // top and bottom lines
    display.fillRect(middleX, y, linesize, height, GxEPD_BLACK);
    for (int i = 0; i < linesize + 1; i++) {
        // top left to bottom right lines
        display.drawLine(i + diagonalLeftX, diagonalTopY, i + diagonalRightX, diagonalBottomY, GxEPD_BLACK);
        // bottom left to top right lines
        display.drawLine(i + diagonalLeftX, diagonalBottomY, i + diagonalRightX, diagonalTopY, GxEPD_BLACK);
    }
    // Create offset between sun circle and lines
    display.fillCircle(middleX, middleY, offsetBetweenCenterAndLines, GxEPD_WHITE);
    // Drawing circle and clearing inside
    display.fillCircle(middleX, middleY, circleRadius, GxEPD_BLACK);
    display.fillCircle(middleX, middleY, circleRadius - linesize, GxEPD_WHITE);
}

// ###############################################################################
/// CODE TO UPDATE
void drawSnow(int x, int16_t y, int16_t scale) {
    int16_t dxo, dyo, dxi, dyi;
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
void drawStorm(int x, int16_t y, int16_t scale) {
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
void drawFog(int x, int16_t y, int16_t scale, int16_t linesize, bool isLarge) {
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