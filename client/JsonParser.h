#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <WString.h>
#include "PlotDrawer.h"

struct JsonSensorData
{
    String name, description;
    float temperature, humidity;
};

struct JsonData
{
    String time;
    std::vector<PlotData> homePlots;
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