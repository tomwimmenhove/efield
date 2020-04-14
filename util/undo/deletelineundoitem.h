#ifndef DELETELINEUNDOITEM_H
#define DELETELINEUNDOITEM_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "undostack.h"

class DeleteLineUndoItem : public UndoItem
{
public:
    DeleteLineUndoItem(const QSharedPointer<SceneElement<float> >& scene,
                      int id,
                      const QString& title);
};

#endif // DELETELINEUNDOITEM_H
