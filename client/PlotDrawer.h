#ifndef PLOTDRAWER_H
#define PLOTDRAWER_H

#include <WString.h>
#include <vector>

class PlotDrawer {
public:
    PlotDrawer();

    void drawPlot(int16_t x, int16_t y, int16_t width, int16_t height, String title,
                  const std::vector<float> &yAxisValues, const std::vector<float> &data, bool isBarchart);
};

#endif // PLOTDRAWER_H