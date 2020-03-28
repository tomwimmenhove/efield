#ifndef DRAWINGELEMENT_H
#define DRAWINGELEMENT_H

#include "model/idrawer.h"

template<typename T>
class DrawingElement
{
public:
    virtual void Draw(IDrawer<T>& drawer) = 0;
    //virtual void DrawAnnotation() = 0;

    virtual ~DrawingElement() { }
};

#endif // DRAWINGELEMENT_H
