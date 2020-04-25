#include "copydrawingelementvisitor.h"

#include "util/undo/newlineundoitem.h"
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
    Q_ASSERT(idMap.find(line.identifier()) == idMap.end());

    int pointId1 = cloneNode(line.point1());
    int pointId2 = cloneNode(line.point2());

    int id = destination->newId();

    auto undoItem = std::make_unique<NewLineUndoItem>(destination, id, pointId1, pointId2, line.value(), true);
    undoItem->doFunction();
    if (undoStack)
        undoStack->add(std::move(undoItem));

    idMap[line.identifier()] = id;
}

int CopyDrawingElementVisitor::cloneNode(const SharedNode& point)
{
    auto it = idMap.find(point.identifier());
    if (it != idMap.end())
        return it->second;

    int id = destination->newId();

    auto undoItem = std::make_unique<NewNodeUndoItem>(destination, id, point.point(), true);
    undoItem->doFunction();
    if (undoStack)
        undoStack->add(std::move(undoItem));

    idMap[point.identifier()] = id;

    return id;
}
