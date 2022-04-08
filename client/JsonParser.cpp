#include "JsonParser.h"

#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson needs version v6 or above

JsonParser::JsonParser()
{

}

bool JsonParser::parse(const String &data)
{
    mData = {};
    if (data.length() == 0) {
        return false;
    }
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, data);

    if (doc.isNull()) {
        return false;
    }

    return true;
}

JsonData JsonParser::data() const
{
    return mData;
}