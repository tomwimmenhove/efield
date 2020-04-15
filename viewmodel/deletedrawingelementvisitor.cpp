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
    DeleteNodeUndoItem undoItem(scene, node.identifier(), "Delete line");
    undoStack->add(undoItem);
    undoItem.doFunction();

    update = true;
}

void DeleteDrawingElementVisitor::visit(LineElement<float>& line)
{
    DeleteLineUndoItem undoItem(scene, line.identifier(), "Delete line");
    undoStack->add(undoItem);
    undoItem.doFunction();

    update = true;
}
