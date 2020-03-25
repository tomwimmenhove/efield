#ifndef HEATLEGENDWIDGET_H
#define HEATLEGENDWIDGET_H

#include <QObject>
#include <QWidget>

class HeatLegendWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HeatLegendWidget(QWidget *parent = nullptr);

    double Min() const { return min; }
    void SetMin(double value);

    double Max() const { return max; }
    void SetMax(double value);

    bool IsStepped() const { return stepped; }
    void SetStepped(bool value);

    float GetSteppedValue(float value);

    double TickStep() const { return tickStep; }

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    /* Thanks to https://stackoverflow.com/questions/4947682/intelligently-calculating-chart-tick-positions/4948320 for these two */
    static double NiceNumber (double value, bool round);
    static double GetNiceTicks(double& axisStart, double& axisEnd, int maxTicks);

    void UpdateTickStep();

    const int maxTicks = 20;
    double axisStart = 0.0;
    double axisEnd = 1.0;
    double min = 0.0;
    double max = 1.0;
    bool stepped = true;

    double tickStep = 0.1;
};

#endif // HEATLEGENDWIDGET_H
