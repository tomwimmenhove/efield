#ifndef SIMULATOR_H
#define SIMULATOR_H


class Simulator
{
public:
    Simulator(int Width, int Height);
    ~Simulator();

    inline float& XYValue(int x, int y) { return curBuf[x + y * w]; }
    inline float& XYValue(float* buf, int x, int y) { return buf[x + y * w]; }
    inline int Height() const { return h; }
    inline int Width() const { return w; }

    void IterateSimulation();
    float MaxValue() const;
    float MinValue() const;

private:
    inline float* SwitchBuf() { return curBuf = values[curBufIdx ^= 1]; }
    float SlowValueAverager(float* buf, int x, int y);

    int w, h;
    float* values[2];
    float* curBuf;
    int curBufIdx = 0;
};

#endif // SIMULATOR_H
