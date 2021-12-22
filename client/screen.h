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

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts; // Select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display(GxEPD2_750_T7(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

// ########################### SCREEN ###########################
void initialiseDisplay() {
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
void drawString(int16_t x, int16_t y, String text, Alignment horizontalAlignment = LEFT, Alignment verticalAlignment = TOP) {
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
void drawRainDrop(int16_t x, int16_t y, int16_t height, int16_t linesize = 2);
void drawWeatherUnderCloud(int16_t x, int16_t y, int16_t height, Weather weather, int16_t linesize = 2);
void drawSnow(int16_t x, int16_t y, int16_t height);
void drawCloud(int16_t x, int16_t y, int16_t height, Weather weather, bool clearInside = true, int16_t linesize = 2);
void drawSun(int16_t x, int16_t y, int16_t height, int16_t linesize = 2);
void drawThunder(int16_t x, int16_t y, int16_t height, int16_t linesize = 2);
void drawSnowFlake(int16_t x, int16_t y, int16_t height, int16_t linesize = 2);
void drawMoon(int16_t x, int16_t y, int16_t height, MoonPhase phase, int16_t linesize = 2);

void drawWeather(int16_t x, int16_t y, int16_t height, Weather weather) {
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

void drawMoon(int16_t x, int16_t y, int16_t height, MoonPhase phase, int16_t linesize) {
    const int16_t radius = height * 0.5;
    const int16_t moonX = x + radius;
    const int16_t moonY = y + radius;
    display.fillCircle(moonX, moonY, radius, GxEPD_BLACK);
    bool isLeftSide = true;
    switch (phase) {
    case NewMoon:
        break;
    case WaningCrescent: // Fallthrough
        isLeftSide = false;
    case WaxingCrescent: {
        const int16_t biggerRadius = radius * 1.1;
        for (int16_t i = 0 ; i < height * 0.3 ; i++) {
            display.drawCircle(moonX + i, moonY, biggerRadius, GxEPD_WHITE);
        }
        for (int16_t i = 0; i < linesize; i++) {
            display.drawCircle(moonX, moonY, radius - i, GxEPD_BLACK);
        }
        break;
    }
    case LastQuarter: // Fallthrough
        isLeftSide = false;
    case FirstQuarter: {
        const int16_t rectX = isLeftSide ? x + radius : x;
        const int16_t rectWidth = isLeftSide ? radius + linesize : radius;
        display.fillRect(rectX, y, rectWidth, height + linesize, GxEPD_WHITE);
        for (int16_t i = 0; i < linesize; i++) {
            display.drawCircle(moonX, moonY, radius - i, GxEPD_BLACK);
        }
        break;
    }
    case WaxingGibbous:
        isLeftSide = false;
    case WaningGibbous: {
        const int16_t secondCircleCenterX = moonX + radius * 0.6 * (isLeftSide ? -1 : 1);
        display.fillCircle(secondCircleCenterX, moonY, radius - linesize, GxEPD_WHITE);
        for (int16_t i = 0 ; i < linesize ; i++) {
            display.drawCircle(moonX, moonY, radius - i, GxEPD_BLACK);
        }
        break;
    }
    case FullMoon:
        display.fillCircle(moonX, moonY, radius - linesize, GxEPD_WHITE);
        break;
    }
}

void drawCloud(int16_t x, int16_t y, int16_t height, Weather weather, bool clearInside, int16_t linesize) {
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
}

void drawWeatherUnderCloud(int16_t x, int16_t y, int16_t height, Weather weather, int16_t linesize) {
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
        display.fillRect(leftX, y - linesize, stormWidth, linesize * 2, GxEPD_WHITE);
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
        display.fillRect(leftX, y - linesize, stormWidth, linesize * 2, GxEPD_WHITE);
        drawThunder(leftX + stormWidth * 0.1, y - height * 0.5, height, linesize);
        drawThunder(leftX + stormWidth * 0.5, y - height * 0.5, height * 1.35, linesize);
        break;
    }
    case Snow: {
        const int16_t snowWidth = height * 0.8;
        const int16_t leftX = x + (height * 2.5 - snowWidth);
        display.fillRect(leftX, y - linesize, snowWidth, linesize * 2, GxEPD_WHITE);

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
        for (int16_t i = 0 ; i < 3 ; i++) {
            const int16_t currentLeftX = leftX + height * 1.1 * i;
            drawRainDrop(currentLeftX, rowY - rainDropHeight * 0.3, rainDropHeight * 1.2, linesize);
            drawSnowFlake(currentLeftX + height * 0.2, rowY, rainDropHeight * 1.5, snowFlakeLinesize);
        }
        break;
    }
    default:
        break;
    }
}

void drawThunder(int16_t x, int16_t y, int16_t height, int16_t linesize) {
    const int16_t middleLineWidth = height * 0.3;
    const int16_t middleLineY = y + height * 0.45;
    const int16_t middleLineRightX = x + middleLineWidth;
    for (int i = 0 ; i < linesize ; i++) {
        display.drawLine(x + middleLineWidth * 0.5 + i, y, x + i, middleLineY, GxEPD_BLACK);
        display.drawLine(x + i, middleLineY - i, middleLineRightX, middleLineY - i, GxEPD_BLACK);
        display.drawLine(middleLineRightX + i, middleLineY - i, x + middleLineWidth * 0.3 + i, y + height, GxEPD_BLACK);
    }
}

void drawRainDrop(int16_t x, int16_t y, int16_t height, int16_t linesize) {
    for (int16_t i = 0; i < linesize; i++) {
        display.drawLine(x + i, y + height * 0.5, x - height * 0.5 + i, y + height * 1.5, GxEPD_BLACK);
    }
}

void drawSnowFlake(int16_t x, int16_t y, int16_t height, int16_t linesize) {
    const int16_t halfSize = height * 0.5;
    const int16_t middleX = x + halfSize;
    const int16_t middleY = y + halfSize;
    // Values for diagonal lines
    const int16_t diagonalLineSize = halfSize / 1.4;
    const int16_t diagonalLeftX = (middleX - diagonalLineSize);
    const int16_t diagonalRightX = (middleX + diagonalLineSize);
    const int16_t diagonalTopY = (middleY - diagonalLineSize);
    const int16_t diagonalBottomY = (middleY + diagonalLineSize);
    for (int i = 0 ; i < linesize ; i++) {
        // left and right lines
        display.fillRect(x, middleY, height, linesize, GxEPD_BLACK);
        // top and bottom lines
        display.fillRect(middleX, y, linesize, height, GxEPD_BLACK);
        for (int16_t i = 0; i < linesize + 1; i++) {
            // top left to bottom right lines
            display.drawLine(i + diagonalLeftX, diagonalTopY, i + diagonalRightX, diagonalBottomY, GxEPD_BLACK);
            // bottom left to top right lines
            display.drawLine(i + diagonalLeftX, diagonalBottomY, i + diagonalRightX, diagonalTopY, GxEPD_BLACK);
        }
    }
}

void drawSun(int16_t x, int16_t y, int16_t height, int16_t linesize) {
    const int16_t halfSize = height * 0.5;
    const int16_t middleX = x + halfSize;
    const int16_t middleY = y + halfSize;
    const int16_t circleRadius = height * 0.2;
    const int16_t offsetBetweenCenterAndLines = circleRadius * 1.5;

    drawSnowFlake(x, y, height, linesize);

    // Create offset between sun circle and lines
    display.fillCircle(middleX, middleY, offsetBetweenCenterAndLines, GxEPD_WHITE);
    // Drawing circle and clearing inside
    display.fillCircle(middleX, middleY, circleRadius, GxEPD_BLACK);
    display.fillCircle(middleX, middleY, circleRadius - linesize, GxEPD_WHITE);
}

String weatherToString(Weather weather) {
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
    }
    return "UNKNOWN";
}

String moonToString(MoonPhase moon) {
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