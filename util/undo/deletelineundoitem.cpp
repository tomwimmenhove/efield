#include "deletelineundoitem.h"
#include "graphics/lineelement.h"

DeleteLineUndoItem::DeleteLineUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                                     int id,
                                     const QString& title)
    : UndoItem(scene, title), id(id)
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(it->elementType() == drawingElementType::Line);
    LineElement<float>& line = static_cast<LineElement<float>&>(*it);
    pointId1 = line.point1().identifier();
    pointId2 = line.point2().identifier();
    value = line.value();
}

void DeleteLineUndoItem::undoFunction()
{
    auto it1 = scene->findId(pointId1);
    Q_ASSERT(it1 != scene->end());
    Q_ASSERT(it1->elementType() == drawingElementType::Node);
    SharedNode node1 = static_cast<NodeElement<float>&>(*it1).anchorNode();

    auto it2 = scene->findId(pointId2);
    Q_ASSERT(it2 != scene->end());
    Q_ASSERT(it2->elementType() == drawingElementType::Node);
    SharedNode node2 = static_cast<NodeElement<float>&>(*it2).anchorNode();

    scene->add(LineElement<float>::uniqueElement(id, scene->bounds(), node1, node2, value));
}

void DeleteLineUndoItem::doFunction()
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(it->elementType() == drawingElementType::Line);
    scene->remove(it);
}
