#include "newlineundoitem.h"
#include "graphics/lineelement.h"

NewLineUndoItem::NewLineUndoItem(const QSharedPointer<SceneElement<float>>& scene, int id, int nodeId1, int nodeId2, float value, const QString& title)
    : UndoItem(scene, title), id(id), nodeId1(nodeId1), nodeId2(nodeId2), value(value)
{ }

void NewLineUndoItem::undoFunction()
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(it->elementType() == drawingElementType::Line);
    scene->remove(it);
}

void NewLineUndoItem::doFunction()
{
    auto it1 = scene->findId(nodeId1);
    Q_ASSERT(it1 != scene->end());
    Q_ASSERT(it1->elementType() == drawingElementType::Node);
    SharedNode node1 = static_cast<NodeElement<float>&>(*it1).anchorNode();

    auto it2 = scene->findId(nodeId2);
    Q_ASSERT(it2 != scene->end());
    Q_ASSERT(it2->elementType() == drawingElementType::Node);
    SharedNode node2 = static_cast<NodeElement<float>&>(*it2).anchorNode();

    scene->add(std::move(LineElement<float>::uniqueElement(id, scene->bounds(), node1, node2, value)));
}
