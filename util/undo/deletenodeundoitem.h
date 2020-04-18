#ifndef DELETENODEUNDOITEM_H
#define DELETENODEUNDOITEM_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "undostack.h"

class DeleteNodeUndoItem : public UndoItem
{
public:
    DeleteNodeUndoItem(const QSharedPointer<SceneElement<float>>& scene, int id);

    QString title() const override { return "Delete node"; }

    void undoFunction() override;
    void doFunction() override;

private:
    int id;
    QPoint oldPoint;
};

#endif // DELETENODEUNDOITEM_H
