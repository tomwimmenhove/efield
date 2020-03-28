#ifndef LINEELEMENT_H
#define LINEELEMENT_H

#include "sharednode.h"
#include "drawingelement.h"
#include "model/drawing.h"

template<typename T>
class LineElement : public DrawingElement<T>
{
public:
    LineElement(const SharedNode& p1, const SharedNode& p2, const T& value)
        : p1(p1), p2(p2), value(value)
    { }

    void Draw(IDrawer<T>& drawer) override
    {
        Drawing<T> drawing(drawer);

        drawing.DrawLine(p1, p2, value);
    }

private:
    SharedNode p1;
    SharedNode p2;
    T value;
};

#endif // LINEELEMENT_H
