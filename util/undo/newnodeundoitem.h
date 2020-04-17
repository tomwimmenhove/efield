#ifndef NEWNODEUNDOITEM_H
#define NEWNODEUNDOITEM_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "undostack.h"

class NewNodeUndoItem : public UndoItem
{
public:
    NewNodeUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                  int id, const QPoint& point,
                  const QString& title);
};

#endif // NEWNODEUNDOITEM_H
