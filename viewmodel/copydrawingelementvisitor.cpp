#include "copydrawingelementvisitor.h"

#include "util/undo/newlineundoitem.h"
#include "util/undo/newcircleundoitem.h"
#include "util/undo/newnodeundoitem.h"

void CopyDrawingElementVisitor::visit(SceneElement<float>&)
{ }

void CopyDrawingElementVisitor::visit(NodeElement<float>& node)
{
    Q_ASSERT(idMap.find(node.identifier()) == idMap.end());

    cloneNode(node.anchorNode());
}

void CopyDrawingElementVisitor::visit(LineElement<float>& line)
{
    copyTwoNodeElement<LineElement<float>, NewLineUndoItem>(line);
}

void CopyDrawingElementVisitor::visit(CircleElement<float>& circle)
{
    copyTwoNodeElement<CircleElement<float>, NewCircleUndoItem>(circle);
}

int CopyDrawingElementVisitor::cloneNode(const SharedNode& point)
{
    auto it = idMap.find(point.identifier());
    if (it != idMap.end())
        return it->second;

    int id = destination->newId();

    auto undoItem = std::make_unique<NewNodeUndoItem>(destination, id, point.point());
    undoItem->doFunction();
    if (undoStack)
        undoStack->add(std::move(undoItem));

    Q_ASSERT(typeid(destination->back()).hash_code() == typeid(NodeElement<float>&).hash_code());
    destination->back().setHighlighted(true);

    idMap[point.identifier()] = id;

    return id;
}

template<typename T, typename U>
void CopyDrawingElementVisitor::copyTwoNodeElement(T& element)
{
    Q_ASSERT(idMap.find(element.identifier()) == idMap.end());

    int pointId1 = cloneNode(element.point1());
    int pointId2 = cloneNode(element.point2());

    int id = destination->newId();

    auto undoItem = std::make_unique<U>(destination, id, pointId1, pointId2, element.value());
    undoItem->doFunction();
    if (undoStack)
        undoStack->add(std::move(undoItem));

    Q_ASSERT(typeid(destination->back()).hash_code() == typeid(T&).hash_code());
    destination->back().setHighlighted(true);

    idMap[element.identifier()] = id;
}
