#ifndef NEWCIRCLEUNDOITEM_H
#define NEWCIRCLEUNDOITEM_H

#include <QSharedPointer>

#include "graphics/circleelement.h"
#include "newtwonodeundoitem.h"

class NewCircleUndoItem : public NewTwoNodeUndoItem<CircleElement<float>>
{
public:
    using NewTwoNodeUndoItem::NewTwoNodeUndoItem;

    QString title() const override { return "Place circle"; }
};

#endif // NEWCIRCLEUNDOITEM_H
