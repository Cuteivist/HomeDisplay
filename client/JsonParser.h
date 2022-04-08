#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <WString.h>

struct JsonData
{
    String time;
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