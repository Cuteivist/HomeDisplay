#ifndef PLOTDRAWER_H
#define PLOTDRAWER_H

#include <WString.h>
#include <vector>

struct AxisData {
    float min = 0;
    float max = 0;
    std::vector<String> labels;
    std::vector<float> labelPositions;
};

struct SeriesData {
    std::vector<float> xValues, yValues;
};

struct PlotData {
    String title;
    String type;
    String location;
    AxisData xAxis, yAxis;
    std::vector<SeriesData> series;
};

class PlotDrawer {
public:
    PlotDrawer();

    void drawForecastPlot(int16_t x, int16_t y, int16_t width, int16_t height, 
                          const PlotData &temp, const PlotData &rain, const std::vector<uint16_t> &weatherIds);
    void drawLinePlot(int16_t x, int16_t y, int16_t width, int16_t height, const PlotData &data);
    void drawBarchart(int16_t x, int16_t y, int16_t width, int16_t height, const PlotData &data);

private:
    void drawSeries(int16_t x, int16_t y, int16_t width, int16_t height, const PlotData &data);
    void drawBars(int16_t x, int16_t y, int16_t width, int16_t height, const PlotData &data);
    void drawYLabels(int16_t x, int16_t y, int16_t width, int16_t height, const std::vector<String> &labels, const std::vector<size_t> &boldIndexes = {});
    void drawXLabels(int16_t x, int16_t y, int16_t width, int16_t height, const std::vector<String> &labels, const std::vector<float> &labelPositions);
    float getYPos(int16_t y, int16_t height, float value, float min, float diff) const;
    float getXPos(int16_t x, int16_t width, float value, float min, float diff) const;
};

#endif // PLOTDRAWER_H