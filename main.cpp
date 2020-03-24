#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

#include <cmath>

double RoundNumDigits(double x, int n)
{
    double scale = pow(10.0, ceil(log10(fabs(x))) + n);

    return round(x * scale) / scale;
}

double NiceNumber (double value, bool round) {
  int    exponent;
  double fraction;
  double niceFraction;

  exponent = (int) floor(log10(value));
  fraction = value/pow(10, (double)exponent);

  if (round) {
    if (fraction < 1.5)
      niceFraction = 1.0;
    else if (fraction < 3.0)
      niceFraction = 2.0;
    else if (fraction < 7.0)
      niceFraction = 5.0;
    else
      niceFraction = 10.0;
   }
  else {
    if (fraction <= 1.0)
      niceFraction = 1.0;
    else if (fraction <= 2.0)
      niceFraction = 2.0;
    else if (fraction <= 5.0)
      niceFraction = 5.0;
    else
      niceFraction = 10.0;
   }

  return niceFraction*pow(10, (double)exponent);
 }

int main(int argc, char *argv[])
{
    double AxisStart = .5;
    double AxisEnd   = 12.4;
    double NumTicks  = 20;

    double AxisWidth;
    double NewAxisStart;
    double NewAxisEnd;
    double NiceRange;
    double NiceTick;

      /* Check for special cases */
    AxisWidth = AxisEnd - AxisStart;
    if (AxisWidth == 0.0) return (0.0);

      /* Compute the new nice range and ticks */
    NiceRange = NiceNumber(AxisEnd - AxisStart, 0);
    NiceTick = NiceNumber(NiceRange/(NumTicks - 1), 1);

      /* Compute the new nice start and end values */
    NewAxisStart = floor(AxisStart/NiceTick)*NiceTick;
    NewAxisEnd = ceil(AxisEnd/NiceTick)*NiceTick;

    AxisStart = NewAxisStart; //26.4
    AxisEnd = NewAxisEnd;     //28.4


    qDebug() << "AxisStart: " << AxisStart;
    qDebug() << "AxisEnd: " << AxisEnd;

    qDebug() << "NiceTick: " << NiceTick;

    int n = 0;
    for (float x = AxisStart; x <= AxisEnd; x += NiceTick)
    {
        qDebug() << (n++) << ": " << x;
    }

    //return 0;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
