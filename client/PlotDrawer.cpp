#include "PlotDrawer.h"

#include "Screen.h"
#include "Logger.h"

PlotDrawer::PlotDrawer()
{

}

 void PlotDrawer::drawForecastPlot(int16_t x, int16_t y, int16_t width, int16_t height, 
                                   const PlotData &temp, const PlotData &rain, const std::vector<uint16_t> &weatherIds)
{
    const float diagramWidth = width - 35;
    const float xLabelsOffset = height * 0.1;
    const float diagramHeight = height - xLabelsOffset;
    const float diagramY = y + height - (diagramHeight + xLabelsOffset);
    const float diagramX = x + width - diagramWidth * 1.02;

    // Draw graph rect
    // Additional width added for border
    Screen::mDisplay.drawRect(diagramX, diagramY, diagramWidth + 3, diagramHeight, GxEPD_BLACK);
    // Set bold line for 0*C
    std::vector<size_t> boldYLines;
    for (size_t i = 0 ; i < temp.yAxis.labels.size() ; i++) {
        if (temp.yAxis.labels[i] == "0") {
            // Label min is at the bottom
            boldYLines.push_back(temp.yAxis.labels.size() - i - 1);
            break;
        }
    }
    // Draw labels
    drawYLabels(diagramX, diagramY, diagramWidth, diagramHeight, temp.yAxis.labels, boldYLines);
    drawXLabels(diagramX, diagramY, diagramWidth, diagramHeight, temp.xAxis.labels, temp.xAxis.labelPositions);
    // Draw temperature
    drawSeries(diagramX, diagramY, diagramWidth, diagramHeight, temp);
    // Draw rain
    const uint16_t barDiagramHeight = diagramHeight * 0.2;
    const uint16_t barDiagramY = diagramY + diagramHeight - barDiagramHeight - 2;
    drawBars(diagramX, barDiagramY, diagramWidth, barDiagramHeight, rain);
}

void PlotDrawer::drawLinePlot(int16_t x, int16_t y, int16_t width, int16_t height, const PlotData &data)
{
    const float diagramWidth = width - 35;
    const float titleOffset = height * 0.1;
    const float xLabelsOffset = height * 0.1;
    const float diagramHeight = height - titleOffset - xLabelsOffset;
    const float diagramY = y + height - (diagramHeight + xLabelsOffset);
    const float diagramX = x + width - diagramWidth * 1.02;

    // Draw graph rect
    // Additional width added for border
    Screen::mDisplay.drawRect(diagramX, diagramY, diagramWidth + 3, diagramHeight, GxEPD_BLACK);
    // Draw title
    Screen::drawString(x + width / 2 + 6, y, data.title, Screen::CENTER);
    // Draw labels
    drawYLabels(diagramX, diagramY, diagramWidth, diagramHeight, data.yAxis.labels);
    drawXLabels(diagramX, diagramY, diagramWidth, diagramHeight, data.xAxis.labels, data.xAxis.labelPositions);

    // Draw series
    drawSeries(diagramX, diagramY, diagramWidth, diagramHeight, data);
}

void PlotDrawer::drawBarchart(int16_t x, int16_t y, int16_t width, int16_t height, const PlotData &data)
{
    const float diagramWidth = width * 0.85;
    const float diagramHeight = height * 0.95;
    const float diagramY = y + height - diagramHeight;
    const float diagramX = x + width - diagramWidth * 1.02;

    // Draw graph rect
    Screen::mDisplay.drawRect(diagramX, diagramY, diagramWidth, diagramHeight, GxEPD_BLACK);
    // Draw title
    Screen::drawString(x + width / 2 + 6, y, data.title, Screen::CENTER);
    // Draw y labels
    drawYLabels(diagramX, diagramY, diagramWidth, diagramHeight, data.yAxis.labels);
    // Draw bars
    drawBars(diagramX, diagramY, diagramWidth, diagramHeight, data);
}

void PlotDrawer::drawYLabels(int16_t x, int16_t y, int16_t width, int16_t height, const std::vector<String> &labels, const std::vector<size_t> &boldIndexes)
{
    // Draw Y axis labels and lines
    const int16_t horizontalDashCount = 20;
    const float horizontalDashWidthSpacing = (float)width / (float)horizontalDashCount;
    const float horizontalDashWidth = horizontalDashWidthSpacing / 2;
    const int16_t yAxisCount = labels.size();
    const int16_t yAxisLabelOffset = 6;
    const int16_t xOffset = x + horizontalDashWidth;
    for (int16_t i = 0; i < yAxisCount; i++) {
        // Draw dashed graph grid lines in the middle
        const float yOffset = y + (height * i / (yAxisCount - 1));
        // Draw Y label
        Screen::drawString(x - yAxisLabelOffset , yOffset, labels[yAxisCount - (i + 1)], Screen::RIGHT, Screen::CENTER);
        if (i == 0 || i == yAxisCount - 1) {
            continue;
        }
        const bool boldLine = std::find(boldIndexes.begin(), boldIndexes.end(), i) != boldIndexes.end();
        for (int j = 0; j < horizontalDashCount; j++) {
            const uint16_t currentX = xOffset + j * horizontalDashWidthSpacing;
            Screen::mDisplay.drawFastHLine(currentX, yOffset, horizontalDashWidth, GxEPD_BLACK);
            if (boldLine) {
                Screen::mDisplay.drawFastHLine(currentX, yOffset + 1, horizontalDashWidth, GxEPD_BLACK);
                Screen::mDisplay.drawFastHLine(currentX, yOffset + 2, horizontalDashWidth, GxEPD_BLACK);
            }
        }
    }
}

void PlotDrawer::drawXLabels(int16_t x, int16_t y, int16_t width, int16_t height, const std::vector<String> &labels, const std::vector<float> &labelPositions)
{
    const int16_t horizontalDashCount = 20;
    const float horizontalDashWidthSpacing = (float)height / (float)horizontalDashCount;
    const float horizontalDashWidth = horizontalDashWidthSpacing / 2;
    const int16_t xAxisCount = labels.size();
    const int16_t xAxisLabelOffset = -width * 0.02;
    const int16_t bottom = y + height;
    const int16_t labelYPos = bottom + 2;
    for (int16_t i = 0; i < xAxisCount && i < labelPositions.size(); i++) {
        // Draw dashed graph grid lines in the middle
        const float xOffset = x + width * labelPositions[i];
        // Draw X label
        Screen::drawString(xOffset, labelYPos, labels[i], Screen::CENTER, Screen::TOP);
        for (int j = 1; j <= horizontalDashCount; j++) {
            Screen::mDisplay.drawFastVLine(xOffset, bottom - j * horizontalDashWidthSpacing, horizontalDashWidth, GxEPD_BLACK);
        }
    }
}

float PlotDrawer::getYPos(int16_t y, int16_t height, float value, float min, float diff ) const
{
    return y + height * (1 - (value - min) / diff);
}

float PlotDrawer::getXPos(int16_t x, int16_t width, float value, float min, float diff) const
{
    return x + width * ((value - min) / diff);
}

void PlotDrawer::drawSeries(int16_t x, int16_t y, int16_t width, int16_t height, const PlotData &data)
{
    if (data.series.empty()) {
        return;
    }

    float startDataX = 0, startDataY = 0, endDataX = 0, endDataY = 0;
    const size_t seriesCount = data.series.size();
    const float yDiff = data.yAxis.max - data.yAxis.min;
    const float xDiff = data.xAxis.max - data.xAxis.min;
    for (size_t s = 0 ; s < seriesCount ; s++) {
        const SeriesData &series = data.series[s];
        if (series.xValues.size() != series.yValues.size() || series.yValues.empty()) {
            DEBUG("Series is empty!");
            continue;
        }
        const size_t dataCount = series.yValues.size();
        startDataX = getXPos(x, width, series.xValues[0], data.xAxis.min, xDiff);
        startDataY = getYPos(y, height, series.yValues[0], data.yAxis.min, yDiff);
        for (int i = 1 ; i < dataCount ; i++) {
            endDataX = getXPos(x, width, series.xValues[i], data.xAxis.min, xDiff);
            endDataY = getYPos(y, height, series.yValues[i], data.yAxis.min, yDiff);
            Screen::mDisplay.drawLine(startDataX, startDataY, endDataX, endDataY, GxEPD_BLACK);
            startDataX = endDataX;
            startDataY = endDataY;
        }
    }
}

void PlotDrawer::drawBars(int16_t x, int16_t y, int16_t width, int16_t height, const PlotData &data)
{
    if (data.series.empty() || data.series[0].yValues.empty()) {
        return;
    }
    const auto &values = data.series[0].yValues;
    const float valueDiff = data.yAxis.max - data.yAxis.min;
    int16_t dataX = 0, dataY = 0;
    float dataXOffset = width / values.size();
    float barChartBarWidth = dataXOffset * 0.8;;

    // Draw the data
    for (int i = 0 ; i < values.size() ; i++) {
        if (values[i] == 0) {
            continue;
        }
        dataX = x + i * dataXOffset;
        dataY = y + (values[i] / data.yAxis.max) * height;
        Screen::mDisplay.fillRect(dataX, dataY, barChartBarWidth, y + height - dataY + 2, GxEPD_BLACK);
    }
}