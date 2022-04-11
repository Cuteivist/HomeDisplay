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
        DEBUG("Json string is empty!");
        return false;
    }
    DynamicJsonDocument doc(1024 * 10);
    DeserializationError err = deserializeJson(doc, data);

    if (err) {
        DEBUG("Deserialization error: " + String(err.c_str()));
        return false;
    }

    mData.time = doc["time"].as<const char*>();
    // TODO update json parsing
    const JsonArray &plotArray = doc["plots"].as<JsonArray>();

    for (const JsonVariant &plotVar : plotArray) {
        const JsonObject &plotJson = plotVar.as<JsonObject>();
        JsonPlotData plotData;
        plotData.title = plotJson["title"].as<const char*>();
        // Get x axis data
        plotData.xAxisData.min = plotJson["xMin"].as<float>();
        plotData.xAxisData.max = plotJson["xMax"].as<float>();
        const JsonArray &xValues = plotJson["x"].as<JsonArray>();
        plotData.xAxisData.values.reserve(xValues.size());
        for (const JsonVariant &val : xValues) {
            plotData.xAxisData.values.push_back(val.as<float>());
        }
        const JsonArray &xLabels = plotJson["xLabels"].as<JsonArray>();
        plotData.xAxisData.labels.reserve(xLabels.size());
        for (const JsonVariant &val : xLabels) {
            plotData.xAxisData.labels.push_back(val.as<const char*>());
        }
        // Get y axis data
        plotData.yAxisData.min = plotJson["yMin"].as<float>();
        plotData.yAxisData.max = plotJson["yMax"].as<float>();
        const JsonArray &yValues = plotJson["y"].as<JsonArray>();
        plotData.yAxisData.values.reserve(yValues.size());
        for (const JsonVariant &val : yValues) {
            plotData.yAxisData.values.push_back(val.as<float>());
        }
        const JsonArray &yLabels = plotJson["yLabels"].as<JsonArray>();
        plotData.yAxisData.labels.reserve(yLabels.size());
        for (const JsonVariant &val : yLabels) {
            plotData.yAxisData.labels.push_back(val.as<const char*>());
        }

        mData.plots.push_back(plotData);
    }

    return true;
}

JsonData JsonParser::data() const
{
    return mData;
}
