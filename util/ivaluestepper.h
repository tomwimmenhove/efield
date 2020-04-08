#ifndef IVALUESTEPPER_H
#define IVALUESTEPPER_H

class IValueStepper
{
public:
    virtual double makeStepped(double value) const = 0;
    virtual ~IValueStepper() { }
};

#endif // IVALUESTEPPER_H
