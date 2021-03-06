#ifndef SIMPLEVALUESTEPPER_H
#define SIMPLEVALUESTEPPER_H

#include "ivaluestepper.h"

class SimpleValueStepper : public IValueStepper
{
public:
    SimpleValueStepper(double stepSize = 0) : stepSize(stepSize) { }

    virtual double makeStepped(double value) const override
    {
        return stepSize == 0 ? value : (qRound(value / stepSize)) * stepSize;
    }

private:
    double stepSize;
};

#endif // SIMPLEVALUESTEPPER_H
