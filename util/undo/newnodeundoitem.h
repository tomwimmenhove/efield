#ifndef NEWNODEUNDOITEM_H
#define NEWNODEUNDOITEM_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "undostack.h"

class NewNodeUndoItem : public UndoItem
{
public:
    NewNodeUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                  int id, const QPoint& point);

    QString title() const override { return "Place node"; }

    void undoFunction() override;
    void doFunction() override;

private:
    QSharedPointer<SceneElement<float>> scene;
    int id;
    QPoint point;
};

#endif // NEWNODEUNDOITEM_H
