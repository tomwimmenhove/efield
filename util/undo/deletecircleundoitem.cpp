#include "deletecircleundoitem.h"
#include "graphics/circleelement.h"

DeleteCircleUndoItem::DeleteCircleUndoItem(const QSharedPointer<SceneElement<float>>& scene, int id)
    : scene(scene), id(id)
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(typeid(*it).hash_code() == typeid(CircleElement<float>).hash_code());
    CircleElement<float>& line = static_cast<CircleElement<float>&>(*it);
    pointId1 = line.point1().identifier();
    pointId2 = line.point2().identifier();
    value = line.value();
}

void DeleteCircleUndoItem::undoFunction()
{
    auto it1 = scene->findId(pointId1);
    Q_ASSERT(it1 != scene->end());
    Q_ASSERT(typeid(*it1).hash_code() == typeid(NodeElement<float>).hash_code());
    SharedNode node1 = static_cast<NodeElement<float>&>(*it1).anchorNode();

    auto it2 = scene->findId(pointId2);
    Q_ASSERT(it2 != scene->end());
    Q_ASSERT(typeid(*it2).hash_code() == typeid(NodeElement<float>).hash_code());
    SharedNode node2 = static_cast<NodeElement<float>&>(*it2).anchorNode();

    scene->add(CircleElement<float>::uniqueElement(id, scene->sceneSize(), node1, node2, value));
}

void DeleteCircleUndoItem::doFunction()
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(typeid(*it).hash_code() == typeid(CircleElement<float>).hash_code());
    scene->remove(it);
}
