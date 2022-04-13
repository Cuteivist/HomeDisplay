#ifndef PLOTDRAWER_H
#define PLOTDRAWER_H

#include <WString.h>
#include <vector>

struct AxisData {
    float min = 0;
    float max = 0;
    std::vector<String> labels;
};

struct SeriesData {
    std::vector<float> xValues, yValues;
};

struct PlotData {
    String title;
    AxisData xAxis, yAxis;
    std::vector<SeriesData> series;
};

class PlotDrawer {
public:
    PlotDrawer();

    void drawLinePlot(int16_t x, int16_t y, int16_t width, int16_t height, const PlotData &data);
    void drawBarchart(int16_t x, int16_t y, int16_t width, int16_t height, const PlotData &data);

private:
    void drawYLabels(int16_t x, int16_t y, int16_t width, int16_t height, const std::vector<String> &labels);
    void drawXLabels(int16_t x, int16_t y, int16_t width, int16_t height, const std::vector<String> &labels);
    float getYPos(int16_t y, int16_t height, float value, float min, float diff) const;
    float getXPos(int16_t x, int16_t width, float value, float min, float diff) const;
};

#endif // PLOTDRAWER_H