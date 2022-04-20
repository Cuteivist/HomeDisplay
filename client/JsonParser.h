#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <WString.h>
#include "PlotDrawer.h"

struct JsonSensorData
{
    String name, description;
    float temperature, humidity;
};

struct JsonWeatherData
{
    PlotData tempPlotData;
    PlotData rainPlotData;
    std::vector<uint16_t> weatherIds;
};

struct JsonData
{
    String time;
    uint64_t nextSleepTimeout;
    std::vector<PlotData> homePlots;
    JsonWeatherData weatherData;
    std::vector<JsonSensorData> sensors;
};

class JsonParser
{
public:
    JsonParser();

    bool parse(const String &data);
    JsonData data() const;

private:
    JsonData mData;
};

#endif // JSONPARSER_H