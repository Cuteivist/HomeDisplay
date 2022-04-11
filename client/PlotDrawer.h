#ifndef PLOTDRAWER_H
#define PLOTDRAWER_H

#include <WString.h>
#include <vector>

struct AxisData {
    std::vector<float> values;
    std::vector<String> labels;
    float min = 0;
    float max = 0;
};

class PlotDrawer {
public:
    PlotDrawer();

    void drawPlot(int16_t x, int16_t y, int16_t width, int16_t height, String title,
                  const std::vector<AxisData>& yAxis, const std::vector<AxisData> &xAxis);
    void drawBarchart(int16_t x, int16_t y, int16_t width, int16_t height, String title, const AxisData &data);

private:
    void drawYLabels(int16_t x, int16_t y, int16_t width, int16_t height, const std::vector<String> &labels);
    void drawXLabels(int16_t x, int16_t y, int16_t width, int16_t height, const std::vector<String> &labels);
    float getYPos(int16_t y, int16_t height, float value, float min, float diff) const;
    float getXPos(int16_t x, int16_t width, float value, float min, float diff) const;
};

#endif // PLOTDRAWER_H