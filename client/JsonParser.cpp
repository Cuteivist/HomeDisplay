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
        const JsonArray &xLabelsPos = plotJson["xLabelsPos"].as<JsonArray>();
        plotData.xAxis.labelPositions.reserve(xLabelsPos.size());
        for (const JsonVariant &val : xLabelsPos) {
            plotData.xAxis.labelPositions.push_back(val.as<float>());
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

    const JsonArray &sensorsArray = doc["sensors"].as<JsonArray>();
    mData.sensors.reserve(sensorsArray.size());
    for (const JsonVariant &sensorVar : sensorsArray) {
        JsonSensorData sensorData;
        const JsonObject &sensorObj = sensorVar.as<JsonObject>();
        sensorData.name = sensorObj["name"].as<const char*>();
        sensorData.description = sensorObj["description"].as<const char*>();
        sensorData.temperature = sensorObj["temperature"].as<float>();
        sensorData.humidity = sensorObj["humidity"].as<float>();
        mData.sensors.push_back(sensorData);
    }

    if (doc.containsKey("weather")) {
        PlotData &weatherData = mData.weatherData.tempPlotData;
        PlotData &rainData = mData.weatherData.rainPlotData;
        const JsonObject &weatherObj = doc["weather"].as<JsonObject>();
        weatherData.title = weatherObj["title"].as<const char*>();
        weatherData.location = weatherObj["outside"].as<const char*>();
        weatherData.type = weatherObj["type"].as<const char*>();
        weatherData.xAxis.min = weatherObj["xMin"].as<float>();
        weatherData.xAxis.max = weatherObj["xMax"].as<float>();
        weatherData.yAxis.min = weatherObj["yMin"].as<float>();
        weatherData.yAxis.max = weatherObj["yMax"].as<float>();
        const JsonArray &yLabels = weatherObj["yLabels"].as<JsonArray>();
        weatherData.yAxis.labels.reserve(yLabels.size());
        for (const JsonVariant &val : yLabels) {
            weatherData.yAxis.labels.push_back(val.as<const char*>());
        }
        const JsonArray &xLabels = weatherObj["xLabels"].as<JsonArray>();
        weatherData.xAxis.labels.reserve(xLabels.size());
        for (const JsonVariant &val : xLabels) {
            weatherData.xAxis.labels.push_back(val.as<const char*>());
        }

        const JsonArray &xLabelsPos = weatherObj["xLabelsPos"].as<JsonArray>();
        weatherData.xAxis.labelPositions.reserve(xLabelsPos.size());
        for (const JsonVariant &val : xLabelsPos) {
            weatherData.xAxis.labelPositions.push_back(val.as<float>());
        }

        const JsonArray &xSeries = weatherObj["x"].as<JsonArray>();
        const JsonArray &ySeries = weatherObj["y"].as<JsonArray>();
        const size_t ySeriesSize = ySeries.size();
        const size_t xSeriesSize = xSeries.size();
        SeriesData series;
        SeriesData rainSeries;
        const JsonArray &seriesYValues = weatherObj["y"].as<JsonArray>();
        weatherData.series.reserve(ySeriesSize);
        series.yValues.reserve(seriesYValues.size());
        for (const JsonVariant &valueVar : seriesYValues) {
            series.yValues.push_back(valueVar.as<float>());
        }
        const JsonArray &seriesXValues = weatherObj["x"].as<JsonArray>();
        series.xValues.reserve(seriesXValues.size());
        rainSeries.xValues.reserve(seriesXValues.size());
        for (const JsonVariant &valueVar : seriesXValues) {
            series.xValues.push_back(valueVar.as<float>());
            rainSeries.xValues.push_back(valueVar.as<float>());
        }
        weatherData.series.push_back(series);

        // Parse rain data
        const JsonArray &rainValues = weatherObj["rain"].as<JsonArray>();
        rainData.series.reserve(1);
        rainSeries.yValues.reserve(rainValues.size());
        for (const JsonVariant &valueVar : rainValues) {
            rainSeries.yValues.push_back(valueVar.as<float>());
        }
        rainData.series.push_back(rainSeries);
        rainData.yAxis.min = 0;
        rainData.yAxis.max = weatherObj["rainMax"].as<float>();

        // Parse weather ids
        const JsonArray &weatherIds = weatherObj["weather_id"].as<JsonArray>();
        mData.weatherData.weatherIds.reserve(weatherIds.size());
        for (const JsonVariant &valueVar : weatherIds) {
            mData.weatherData.weatherIds.push_back(valueVar.as<uint16_t>());
        }
    }

    mData.nextSleepTimeout = doc["sleep_timeout"].as<uint64_t>();

    return true;
}

JsonData JsonParser::data() const
{
    return mData;
}
