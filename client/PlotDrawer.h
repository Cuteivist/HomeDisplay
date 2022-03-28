#ifndef PLOTDRAWER_H
#define PLOTDRAWER_H

#include <WString.h>
#include <vector>

class PlotDrawer {
public:
    PlotDrawer();

    void drawPlot(int16_t x, int16_t y, int16_t width, int16_t height, String title,
                  const std::vector<float> &yAxisLabels, const std::vector<String> &yAxisLabels,
                  const std::vector<float> &yValues, const std::vector<float> &xValues, bool isBarchart)
};

#endif // PLOTDRAWER_H