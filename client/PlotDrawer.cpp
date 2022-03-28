#include "PlotDrawer.h"

#include "Screen.h"

PlotDrawer::PlotDrawer()
{

}

void PlotDrawer::drawPlot(int16_t x, int16_t y, int16_t width, int16_t height, String title,
                          const std::vector<float> &yAxisLabels, const std::vector<float> &yAxisLabels,
                          const std::vector<float> &yValues, const std::vector<float> &xValues, bool isBarchart)
{
    // TODO split barchart and lines plot, because bar chart doesn't need x labels
    if (yAxisLabels.empty()) {
        Screen::drawString(x, y, "ERROR: Empty y values!");
        return;
    }
    const float diagramWidth = width * 0.85;
    const float diagramHeight = height * 0.95;
    const float diagramY = y + height - diagramHeight;
    const float diagramX = x + width - diagramWidth;

    const float yMax = yAxisLabels.back();
    const float yMin = yAxisLabels.front();
    float startDataX = 0, startDataY = 0, dataEndX = 0, dataEndY = 0;
    float barChartBarWidth = 0;
    float dataXOffset = 0;

    if (isBarchart) {
        dataXOffset = diagramWidth / yValues.size();
        barChartBarWidth = dataXOffset * 0.8;
    } else if (!data.empty()) {
        dataXOffset = diagramWidth / (yValues.size() - 1);
        startDataX = diagramX;
        startDataY = diagramY + (yMax - constrain(yValues[0], yMin, yMax)) / (yMax - yMin) * diagramHeight;
    }
    // Draw graph rect
    Screen::mDisplay.drawRect(diagramX, diagramY, diagramWidth, diagramHeight, GxEPD_BLACK);
    // Draw title
    Screen::drawString(x + width / 2 + 6, y, title, Screen::CENTER);

    // Draw the data
    for (int i = isBarchart ? 0 : 1 ; i < yValues.size() ; i++) {
        // TODO handle X data
        dataEndX = diagramX + i * dataXOffset;
        dataEndY = diagramY + (yMax - constrain(yValues[i], yMin, yMax)) / (yMax - yMin) * diagramHeight + 1;
        if (isBarchart) {
            Screen::mDisplay.fillRect(dataEndX, dataEndY, barChartBarWidth, diagramY + diagramHeight - dataEndY + 2, GxEPD_BLACK);
        } else {
            Screen::mDisplay.drawLine(startDataX, startDataY, dataEndX, dataEndY, GxEPD_BLACK);
        }
        startDataX = dataEndX;
        startDataY = dataEndY;
    }

    // Draw Y axis labels and lines
    const int horizontalDashCount = 20;
    const float horizontalDashWithSpacing = diagramWidth / horizontalDashCount;
    const float horizontalDashWidth = horizontalDashWithSpacing / 2;
    const int yAxisCount = yAxisLabels.size();
    const int yAxisLabelOffset = -diagramWidth * 0.02;
    for (int i = 0; i < yAxisCount; i++) {
        // Draw dashed graph grid lines in the middle
        const float yOffset = (diagramHeight * i / (yAxisCount - 1));
        // Draw Y label
        Screen::drawString(diagramX + yAxisLabelOffset , diagramY + yOffset, String(yAxisLabels[yAxisCount - (i + 1)], 1), Screen::RIGHT, Screen::CENTER);
        if (i == 0 || i == yAxisCount - 1) {
            continue;
        }
        for (int j = 0; j < horizontalDashCount; j++) {
            Screen::mDisplay.drawFastHLine((diagramX + 3 + j * horizontalDashWithSpacing), diagramY + yOffset, horizontalDashWidth, GxEPD_BLACK);
        }
    }

    // TODO add x axis labels
}