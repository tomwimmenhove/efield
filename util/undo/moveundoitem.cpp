#include "moveundoitem.h"

MoveUndoItem::MoveUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                           int id,
                           const QPoint& oldPoint,
                           const QPoint& newPoint,
                           const QString& title)
    : UndoItem(scene, title), id(id), oldPoint(oldPoint), newPoint(newPoint)
{ }

void MoveUndoItem::undoFunction()
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    it->setCenter(oldPoint);
}

void MoveUndoItem::doFunction()
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    it->setCenter(newPoint);
}
