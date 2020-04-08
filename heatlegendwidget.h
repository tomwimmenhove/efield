#ifndef HEATLEGENDWIDGET_H
#define HEATLEGENDWIDGET_H

#include <QObject>
#include <QWidget>

#include "util/simplevaluestepper.h"

class HeatLegendWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HeatLegendWidget(QWidget *parent = nullptr);

    double min() const { return minVal; }
    void setMin(double value);

    double max() const { return maxVal; }
    void setMax(double value);

    bool isStepped() const { return stepped; }
    void setStepped(bool value);

    inline SimpleValueStepper stepper() { return SimpleValueStepper(isStepped() ? tickStepVal : 0); }

    float steppedValue(float value);

    double tickStep() const { return tickStepVal; }

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    /* Thanks to https://stackoverflow.com/questions/4947682/intelligently-calculating-chart-tick-positions/4948320 for these two */
    static double niceNumber (double value, bool round);
    static double getNiceTicks(double& axisStart, double& axisEnd, int maxTicks);

    void updateTickStepu();

    const int maxTicks = 20;
    double axisStart = 0.0;
    double axisEnd = 1.0;
    double minVal = 0.0;
    double maxVal = 1.0;
    bool stepped = true;

    double tickStepVal = 0.1;
};

#endif // HEATLEGENDWIDGET_H
