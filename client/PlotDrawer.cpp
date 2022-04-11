#include "PlotDrawer.h"

#include "Screen.h"
#include "Logger.h"

PlotDrawer::PlotDrawer()
{

}

void PlotDrawer::drawPlot(int16_t x, int16_t y, int16_t width, int16_t height, String title,
                          const AxisData &yAxis, const AxisData &xAxis)
{
    const float diagramWidth = width * 0.85;
    const float titleOffset = height * 0.1;
    const float xLabelsOffset = height * 0.1;
    const float diagramHeight = height - titleOffset - xLabelsOffset;
    const float diagramY = y + height - (diagramHeight + xLabelsOffset);
    const float diagramX = x + width - diagramWidth * 1.02;

    const float yDiff = yAxis.max - yAxis.min;
    float startDataX = 0, startDataY = 0, endDataX = 0, endDataY = 0;
    float barChartBarWidth = 0;
    float dataXOffset = 0;

    // Draw graph rect
    // Additional width added for border
    Screen::mDisplay.drawRect(diagramX, diagramY, diagramWidth + 3, diagramHeight, GxEPD_BLACK);
    // Draw title
    Screen::drawString(x + width / 2 + 6, y, title, Screen::CENTER);
    // Draw labels
    drawYLabels(diagramX, diagramY, diagramWidth, diagramHeight, yAxis.labels);
    drawXLabels(diagramX, diagramY, diagramWidth, diagramHeight, xAxis.labels);

    if (yAxis.values.empty()) {
        return;
    }

    // Draw the data with same x spacing between values
    if (xAxis.values.size() != yAxis.values.size() || xAxis.values.empty()) {
        dataXOffset = diagramWidth / (yAxis.values.size() - 1);
        startDataX = diagramX;
        startDataY = getYPos(diagramY, diagramHeight, yAxis.values[0], yAxis.min, yDiff);
        for (int i = 1 ; i < yAxis.values.size() ; i++) {
            endDataX = diagramX + i * dataXOffset;
            endDataY = getYPos(diagramY, diagramHeight, yAxis.values[i], yAxis.min, yDiff);
            Screen::mDisplay.drawLine(startDataX, startDataY, endDataX, endDataY, GxEPD_BLACK);
            startDataX = endDataX;
            startDataY = endDataY;
        }
        return;
    }

    // Draw plot using X values
    const float xDiff = xAxis.max - xAxis.min;
    startDataX = getXPos(diagramX, diagramWidth, xAxis.values[0], xAxis.min, xDiff);
    startDataY = getYPos(diagramY, diagramHeight, yAxis.values[0], yAxis.min, yDiff);
    for (int i = 1 ; i < yAxis.values.size() ; i++) {
        endDataX = getXPos(diagramX, diagramWidth, xAxis.values[i], xAxis.min, xDiff);
        endDataY = getYPos(diagramY, diagramHeight, yAxis.values[i], yAxis.min, yDiff);
        Screen::mDisplay.drawLine(startDataX, startDataY, endDataX, endDataY, GxEPD_BLACK);
        startDataX = endDataX;
        startDataY = endDataY;
    }
}

void PlotDrawer::drawBarchart(int16_t x, int16_t y, int16_t width, int16_t height, String title, const AxisData &data)
{
    const float diagramWidth = width * 0.85;
    const float diagramHeight = height * 0.95;
    const float diagramY = y + height - diagramHeight;
    const float diagramX = x + width - diagramWidth * 1.02;
    const float valueDiff = data.max - data.min;

    float startDataX = 0, startDataY = 0, endDataX = 0, endDataY = 0;
    float dataXOffset = diagramWidth / data.values.size();
    float barChartBarWidth = dataXOffset * 0.8;;

    // Draw graph rect
    Screen::mDisplay.drawRect(diagramX, diagramY, diagramWidth, diagramHeight, GxEPD_BLACK);
    // Draw title
    Screen::drawString(x + width / 2 + 6, y, title, Screen::CENTER);

    // Draw the data
    for (int i = 0 ; i < data.values.size() ; i++) {
        endDataX = diagramX + i * dataXOffset;
        endDataY = getYPos(diagramY, diagramHeight, data.values[i], data.min, valueDiff);
        Screen::mDisplay.fillRect(endDataX, endDataY, barChartBarWidth, diagramY + diagramHeight - endDataY + 2, GxEPD_BLACK);
        startDataX = endDataX;
        startDataY = endDataY;
    }

    drawYLabels(diagramX, diagramY, diagramWidth, diagramHeight, data.labels);
}

void PlotDrawer::drawYLabels(int16_t x, int16_t y, int16_t width, int16_t height, const std::vector<String> &labels)
{
    // Draw Y axis labels and lines
    const int horizontalDashCount = 20;
    const float horizontalDashWithSpacing = width / horizontalDashCount;
    const float horizontalDashWidth = horizontalDashWithSpacing / 2;
    const int yAxisCount = labels.size();
    const int yAxisLabelOffset = -4;
    const int xOffset = x + horizontalDashWidth;
    for (int i = 0; i < yAxisCount; i++) {
        // Draw dashed graph grid lines in the middle
        const float yOffset = y + (height * i / (yAxisCount - 1));
        // Draw Y label
        Screen::drawString(x + yAxisLabelOffset , yOffset, labels[yAxisCount - (i + 1)], Screen::RIGHT, Screen::CENTER);
        if (i == 0 || i == yAxisCount - 1) {
            continue;
        }
        for (int j = 0; j <= horizontalDashCount; j++) {
            Screen::mDisplay.drawFastHLine((xOffset + j * horizontalDashWithSpacing), yOffset, horizontalDashWidth, GxEPD_BLACK);
        }
    }
}

void PlotDrawer::drawXLabels(int16_t x, int16_t y, int16_t width, int16_t height, const std::vector<String> &labels)
{
    const int16_t horizontalDashCount = 20;
    const float horizontalDashWithSpacing = height / horizontalDashCount;
    const float horizontalDashWidth = horizontalDashWithSpacing / 2;
    const int16_t xAxisCount = labels.size();
    const int16_t xAxisLabelOffset = -width * 0.02;
    const int16_t bottom = y + height;
    const int16_t labelYPos = bottom + 2;
    for (int16_t i = 0; i < xAxisCount; i++) {
        // Draw dashed graph grid lines in the middle
        const float xOffset = x + width / (xAxisCount - 1)* i;
        // Draw X label
        Screen::drawString(xOffset, labelYPos, labels[i], Screen::CENTER, Screen::TOP);
        if (i == 0 || i == xAxisCount - 1) {
            continue;
        }
        for (int j = 1; j <= horizontalDashCount; j++) {
            Screen::mDisplay.drawFastVLine(xOffset, bottom - 3 - j * horizontalDashWithSpacing, horizontalDashWidth, GxEPD_BLACK);
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
