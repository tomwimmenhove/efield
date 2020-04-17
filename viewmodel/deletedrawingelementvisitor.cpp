#include "deletedrawingelementvisitor.h"

#include "util/undo/deletelineundoitem.h"
#include "util/undo/deletenodeundoitem.h"

bool DeleteDrawingElementVisitor::deleteElement(const QSharedPointer<UndoStack>& undoStack,
                                     const QSharedPointer<SceneElement<float>>& scene,
                                     DrawingElement<float>& element)
{
    DeleteDrawingElementVisitor v(undoStack, scene);
    element.accept(v);

    return v.update;
}

void DeleteDrawingElementVisitor::visit(SceneElement<float>&)
{ }

void DeleteDrawingElementVisitor::visit(NodeElement<float>& node)
{
    auto undoItem = std::make_unique<DeleteNodeUndoItem>(scene, node.identifier(), "Delete line");
    undoItem->doFunction();
    undoStack->add(std::move(undoItem));

    update = true;
}

void DeleteDrawingElementVisitor::visit(LineElement<float>& line)
{
    auto undoItem = std::make_unique<DeleteLineUndoItem>(scene, line.identifier(), "Delete line");
    undoItem->doFunction();
    undoStack->add(std::move(undoItem));

    update = true;
}
