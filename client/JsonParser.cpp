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
    const size_t requestSize = strlen(data.c_str());
    DEBUG("Response length: " + String(requestSize));
    DynamicJsonDocument doc(requestSize * 3);
    DeserializationError err = deserializeJson(doc, data);

    if (err) {
        // TODO handle deserialiation error
        DEBUG("Deserialization error: " + String(err.c_str()));
        return false;
    }

    mData.time = doc["time"].as<const char*>();
    const JsonArray &plotArray = doc["plots"].as<JsonArray>();

    for (const JsonVariant &plotVar : plotArray) {
        const JsonObject &plotJson = plotVar.as<JsonObject>();
        PlotData plotData;
        plotData.title = plotJson["title"].as<const char*>();
        plotData.type = plotJson["type"].as<const char*>();
        plotData.location = plotJson["location"].as<const char*>();
        // Get x axis data
        plotData.xAxis.min = plotJson["xMin"].as<float>();
        plotData.xAxis.max = plotJson["xMax"].as<float>();
        const JsonArray &xLabels = plotJson["xLabels"].as<JsonArray>();
        plotData.xAxis.labels.reserve(xLabels.size());
        for (const JsonVariant &val : xLabels) {
            plotData.xAxis.labels.push_back(val.as<const char*>());
        }
        // Get y axis data
        plotData.yAxis.min = plotJson["yMin"].as<float>();
        plotData.yAxis.max = plotJson["yMax"].as<float>();
        const JsonArray &yLabels = plotJson["yLabels"].as<JsonArray>();
        plotData.yAxis.labels.reserve(yLabels.size());
        for (const JsonVariant &val : yLabels) {
            plotData.yAxis.labels.push_back(val.as<const char*>());
        }

        // Populate x and y values
        const JsonArray &xSeries = plotJson["x"].as<JsonArray>();
        const JsonArray &ySeries = plotJson["y"].as<JsonArray>();
        const size_t ySeriesSize = ySeries.size();
        const size_t xSeriesSize = xSeries.size();
        plotData.series.reserve(ySeriesSize);
        for (size_t i = 0 ; i < ySeries.size() ; i++) {
            SeriesData series;
            const JsonArray &seriesYValues = ySeries[i].as<JsonArray>();
            series.yValues.reserve(seriesYValues.size());
            for (const JsonVariant &valueVar : seriesYValues) {
                series.yValues.push_back(valueVar.as<float>());
            }
            if (xSeriesSize <= i) {
                continue;
            }
            const JsonArray &seriesXValues = xSeries[i].as<JsonArray>();
            series.xValues.reserve(seriesXValues.size());
            for (const JsonVariant &valueVar : seriesXValues) {
                series.xValues.push_back(valueVar.as<float>());
            }
            plotData.series.push_back(series);
        }
        if (plotData.location == "home") {
            mData.homePlots.push_back(plotData);
        }
    }

    return true;
}

JsonData JsonParser::data() const
{
    return mData;
}
