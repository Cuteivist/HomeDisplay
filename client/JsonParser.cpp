#include "JsonParser.h"

#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson needs version v6 or above
#include "Logger.h"

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
    DeserializationError err = deserializeJson(doc, data);

    if (err) {
        DEBUG("Deserialization error: " + String(err.c_str()));
        return false;
    }

    mData.time = doc["time"].as<const char*>();

    return true;
}

JsonData JsonParser::data() const
{
    return mData;
}