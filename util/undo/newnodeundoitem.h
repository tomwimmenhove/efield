#ifndef NEWNODEUNDOITEM_H
#define NEWNODEUNDOITEM_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "undostack.h"

class NewNodeUndoItem : public UndoItem
{
public:
    NewNodeUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                  int id, const QPoint& point, bool highlight = false);

    QString title() const override { return "Place node"; }

    void undoFunction() override;
    void doFunction() override;

private:
    int id;
    QPoint point;
    bool highlight;
};

#endif // NEWNODEUNDOITEM_H
