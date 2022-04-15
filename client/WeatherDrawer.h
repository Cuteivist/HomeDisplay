#ifndef WEATHERDRAWER_H
#define WEATHERDRAWER_H

#include <WString.h>
#include <vector>

class WeatherDrawer {
public:
    WeatherDrawer();
    
    enum Weather {
        Sunny = 0,
        LowCloudy,
        FullCloudy,
        Rain,
        Storm,
        Rainstorm,
        Snow,
        SnowWithRain,
        WindyCloud,
        Windy,
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

    String weatherToString(Weather weather) const;
    String moonToString(MoonPhase weather) const;

    void drawWeather(int16_t x, int16_t y, int16_t height, Weather weather);
    void drawMoon(int16_t x, int16_t y, int16_t height, MoonPhase phase, int16_t linesize = 2);
    Weather findMostCommonWeather(const std::vector<uint16_t> &weatherIds) const;

private:
    void drawCloud(int16_t x, int16_t y, int16_t height, Weather weather, bool clearInside = true, int16_t linesize = 2);
    void drawSun(int16_t x, int16_t y, int16_t height, int16_t linesize = 2);
    void drawRainDrop(int16_t x, int16_t y, int16_t height, int16_t linesize = 2);
    void drawSnowFlake(int16_t x, int16_t y, int16_t height, int16_t linesize = 2);
    void drawThunder(int16_t x, int16_t y, int16_t height, int16_t linesize = 2);
    void drawWeatherUnderCloud(int16_t x, int16_t y, int16_t height, Weather weather, int16_t linesize = 2);
    void drawFog(int16_t x, int16_t y, int16_t height, int16_t linesize = 2);
    void drawWind(int16_t x, int16_t y, int16_t height, int16_t linesize = 2);

    Weather weatherIdToEnum(uint16_t id) const;
};

#endif // WEATHERDRAWER_H