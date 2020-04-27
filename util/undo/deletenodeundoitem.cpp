#include "deletenodeundoitem.h"

DeleteNodeUndoItem::DeleteNodeUndoItem(const QSharedPointer<SceneElement<float>>& scene, int id)
    : scene(scene), id(id)
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(typeid(*it).hash_code() == typeid(NodeElement<float>).hash_code());
    oldPoint = it->center();
}

void DeleteNodeUndoItem::undoFunction()
{
    scene->add(NodeElement<float>::uniqueElement(SharedNode(id, oldPoint), scene->sceneSize()));
}

void DeleteNodeUndoItem::doFunction()
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(typeid(*it).hash_code() == typeid(NodeElement<float>).hash_code());
    scene->remove(it);
}
