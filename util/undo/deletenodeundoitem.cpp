#include "deletenodeundoitem.h"

DeleteNodeUndoItem::DeleteNodeUndoItem(const QSharedPointer<SceneElement<float> >& scene,
                                     int id,
                                     const QString& title)
    : UndoItem(title)
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(it->elementType() == drawingElementType::Node);
    QPoint oldPoint = it->center();

    undoFunc = [scene, id, oldPoint]()
    {
        scene->add(NodeElement<float>::uniqueElement(SharedNode(id, oldPoint), scene->bounds()));
    };

    doFunc = [scene, id]()
    {
        auto it = scene->findId(id);
        Q_ASSERT(it != scene->end());
        Q_ASSERT(it->elementType() == drawingElementType::Node);
        scene->remove(it);
    };
}
