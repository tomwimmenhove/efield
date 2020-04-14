#include "deletelineundoitem.h"
#include "graphics/lineelement.h"

DeleteLineUndoItem::DeleteLineUndoItem(const QSharedPointer<SceneElement<float> >& scene,
                                     int id,
                                     const QString& title)
    : UndoItem(title)
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(it->elementType() == drawingElementType::Line);
    LineElement<float>& line = static_cast<LineElement<float>&>(*it);
    int p1 = line.point1().identifier();
    int p2 = line.point2().identifier();
    float value = line.value();

    undoFunc = [scene, id, p1, p2, value]()
    {
        auto it1 = scene->findId(p1);
        Q_ASSERT(it1 != scene->end());
        Q_ASSERT(it1->elementType() == drawingElementType::Node);
        SharedNode node1 = static_cast<NodeElement<float>&>(*it1).anchorNode();

        auto it2 = scene->findId(p2);
        Q_ASSERT(it2 != scene->end());
        Q_ASSERT(it2->elementType() == drawingElementType::Node);
        SharedNode node2 = static_cast<NodeElement<float>&>(*it2).anchorNode();

        scene->add(LineElement<float>::uniqueElement(id, scene->bounds(), node1, node2, value));
    };

    doFunc = [scene, id]()
    {
        auto it = scene->findId(id);
        Q_ASSERT(it != scene->end());
        Q_ASSERT(it->elementType() == drawingElementType::Line);
        scene->remove(it);
    };
}
