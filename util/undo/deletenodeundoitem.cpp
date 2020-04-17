#include "deletenodeundoitem.h"

DeleteNodeUndoItem::DeleteNodeUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                                     int id,
                                     const QString& title)
    : UndoItem(scene, title), id(id)
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(it->elementType() == drawingElementType::Node);
    oldPoint = it->center();
}

void DeleteNodeUndoItem::undoFunction()
{
    scene->add(NodeElement<float>::uniqueElement(SharedNode(id, oldPoint), scene->bounds()));
}

void DeleteNodeUndoItem::doFunction()
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(it->elementType() == drawingElementType::Node);
    scene->remove(it);
}
