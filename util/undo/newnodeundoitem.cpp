#include "newnodeundoitem.h"

NewNodeUndoItem::NewNodeUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                                 int id, const QPoint& point, const QString& title)
    : UndoItem(scene, title), id(id), point(point)
{ }

void NewNodeUndoItem::undoFunction()
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(it->elementType() == drawingElementType::Node);
    scene->remove(it);
}

void NewNodeUndoItem::doFunction()
{
    scene->add(std::move(NodeElement<float>::uniqueElement(SharedNode(id, point), scene->bounds())));
}
