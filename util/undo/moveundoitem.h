#ifndef MOVEUNDOITEM_H
#define MOVEUNDOITEM_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "undostack.h"

class MoveUndoItem : public UndoItem
{
public:
    MoveUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                 int id,
                 const QPoint& oldPoint,
                 const QPoint& newPoint,
                 const QString& title);

    void undoFunction() override;
    void doFunction() override;

private:
    int id;
    QPoint oldPoint;
    QPoint newPoint;
};

#endif // MOVEUNDOITEM_H
