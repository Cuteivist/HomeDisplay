#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <WString.h>
#include "PlotDrawer.h"

struct JsonPlotData
{
    String title;
    AxisData xAxisData, yAxisData;
};

struct JsonData
{
    String time;
    std::vector<JsonPlotData> plots;
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