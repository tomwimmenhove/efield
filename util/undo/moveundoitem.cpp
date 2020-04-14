#include "moveundoitem.h"

MoveUndoItem::MoveUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                           int id,
                           const QPoint& oldPoint,
                           const QPoint& newPoint,
                           const QString& title)
    : UndoItem(title)
{
    undoFunc = [scene, id, oldPoint]()
    {
        auto it = scene->findId(id);
        Q_ASSERT(it != scene->end());
        it->setCenter(oldPoint);
    };

    doFunc = [scene, id, newPoint]()
    {
        auto it = scene->findId(id);
        Q_ASSERT(it != scene->end());
        it->setCenter(newPoint);
    };
}
