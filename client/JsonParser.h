#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <WString.h>
#include "PlotDrawer.h"

struct JsonData
{
    String time;
    std::vector<PlotData> homePlots;
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