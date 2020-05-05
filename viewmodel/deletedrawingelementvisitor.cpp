#include "deletedrawingelementvisitor.h"
#include "util/undo/deletelineundoitem.h"
#include "util/undo/deletenodeundoitem.h"
#include "util/undo/deletecircleundoitem.h"

void DeleteDrawingElementVisitor::visit(SceneElement<float>&)
{ }

void DeleteDrawingElementVisitor::visit(NodeElement<float>& node)
{
    auto undoItem = std::make_unique<DeleteNodeUndoItem>(scene, node.identifier());
    undoItem->doFunction();
    undoStack->add(std::move(undoItem));

    update = true;
}

void DeleteDrawingElementVisitor::visit(LineElement<float>& line)
{
    auto undoItem = std::make_unique<DeleteLineUndoItem>(scene, line.identifier());
    undoItem->doFunction();
    undoStack->add(std::move(undoItem));

    update = true;
}

void DeleteDrawingElementVisitor::visit(CircleElement<float>& circle)
{
    auto undoItem = std::make_unique<DeleteCircleUndoItem>(scene, circle.identifier());
    undoItem->doFunction();
    undoStack->add(std::move(undoItem));

    update = true;
}
