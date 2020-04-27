#include "newnodeundoitem.h"

NewNodeUndoItem::NewNodeUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                                 int id, const QPoint& point)
    : scene(scene), id(id), point(point)
{ }

void NewNodeUndoItem::undoFunction()
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(typeid(*it).hash_code() == typeid(NodeElement<float>).hash_code());
    scene->remove(it);
}

void NewNodeUndoItem::doFunction()
{
    auto element = NodeElement<float>::uniqueElement(SharedNode(id, point), scene->sceneSize());
    scene->add(std::move(element));
}
