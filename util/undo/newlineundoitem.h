#ifndef PLACEUNDOITEM_H
#define PLACEUNDOITEM_H

#include <QSharedPointer>

#include "graphics/lineelement.h"
#include "newtwonodeundoitem.h"

class NewLineUndoItem : public NewTwoNodeUndoItem<LineElement<float>>
{
public:
    using NewTwoNodeUndoItem::NewTwoNodeUndoItem;

    QString title() const override { return "Place line"; }
};

#endif // PLACEUNDOITEM_H
