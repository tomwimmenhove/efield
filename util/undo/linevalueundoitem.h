#ifndef LINEVALUEUNDOITEM_H
#define LINEVALUEUNDOITEM_H

#include "valueundoitem.h"
#include "graphics/lineelement.h"

class LineValueUndoItem : public ValueUndoItem<LineElement<float>>
{
public:
    using ValueUndoItem::ValueUndoItem;
    QString title() const override { return "Change line voltage"; }
};

#endif // LINEVALUEUNDOITEM_H
