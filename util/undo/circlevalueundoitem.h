#ifndef CIRCLEVALUEUNDOITEM_H
#define CIRCLEVALUEUNDOITEM_H

#include "valueundoitem.h"
#include "graphics/circleelement.h"

class CircleValueUndoItem : public ValueUndoItem<CircleElement<float>>
{
public:
    using ValueUndoItem::ValueUndoItem;
    QString title() const override { return "Change circle voltage"; }
};

#endif // CIRCLEVALUEUNDOITEM_H
