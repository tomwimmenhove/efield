#include <limits>

#include "simulator.h"

Simulator::Simulator(int width, int height)
    : w(width), h(height)
{
    values[0] = new float[w * h];
    values[1] = new float[w * h];

    for (int i = 0; i < w * h ;i++)
    {
        values[0][i] = 0;
        values[1][i] = 0;
    }

    SwitchBuf();
}

Simulator::~Simulator()
{
    delete[] values[0];
    delete[] values[1];
}

void Simulator::IterateSimulation()
{
    float* oldBuf = curBuf;
    SwitchBuf();

    for (int x = 0; x < w; x++)
        XYValue(x, 0) = SlowValueAverager(oldBuf, x, 0);

    for (int x = 0; x < w; x++)
        XYValue(x, h - 1) = SlowValueAverager(oldBuf, x, h - 1);

    for (int y = 0; y < h; y++)
        XYValue(0, y) = SlowValueAverager(oldBuf, 0, y);

    for (int y = 0; y < h; y++)
        XYValue(w - 1, y) = SlowValueAverager(oldBuf, w - 1, y);

#pragma omp parallel for
    for (int y = 1; y < h - 1; y++)
    {
        for (int x = 1; x < w - 1; x++)
        {
            XYValue(x, y) = (XYValue(oldBuf, x - 1, y - 1) +
                             XYValue(oldBuf, x    , y - 1) +
                             XYValue(oldBuf, x + 1, y - 1) +

                             XYValue(oldBuf, x - 1, y    ) +
                             XYValue(oldBuf, x + 1, y    ) +

                             XYValue(oldBuf, x - 1, y + 1) +
                             XYValue(oldBuf, x    , y + 1) +
                             XYValue(oldBuf, x + 1, y + 1)
                             ) / 8.0f;
        }
    }
}

float Simulator::MaxValue() const
{
    float max = std::numeric_limits<int>::min();

    for (int i = 0; i < w * h; i++)
    {
        float f = curBuf[i];
        if (f > max)
            max = f;
    }

    return max;
}

float Simulator::MinValue() const
{
    float min = std::numeric_limits<int>::max();

    for (int i = 0; i < w * h; i++)
    {
        float f = curBuf[i];
        if (f < min)
            min = f;
    }

    return min;
}

float Simulator::SlowValueAverager(float* buf, int x, int y)
{
    float total = 0;
    int n = 0;
    if (y > 0)
    {
        total += XYValue(buf, x, y - 1);
        n++;
        if (x < w - 2)
        {
            total += XYValue(buf, x + 1, y - 1);
            n++;
        }
    }

    if (x > 0)
    {
        total += XYValue(buf, x - 1, y);
        n++;
        if (y < h - 2)
        {
            total += XYValue(buf, x - 1, y + 1);
            n++;
        }
        if (y > 0)
        {
            total += XYValue(buf, x - 1, y - 1);
            n++;
        }
    }

    if (x < w - 2)
    {
        total += XYValue(buf, x + 1, y    );
        n++;
        if (y < h - 2)
        {
            total += XYValue(buf, x + 1, y + 1);
            n++;
        }
    }

    if (y < h - 2)
    {
        total += XYValue(buf, x, y + 1);
        n++;
    }


    return total / (float) n;
}
