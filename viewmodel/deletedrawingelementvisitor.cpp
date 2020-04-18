#include "deletedrawingelementvisitor.h"

#include "util/undo/deletelineundoitem.h"
#include "util/undo/deletenodeundoitem.h"
#include "util/undo/compositundoitem.h"

bool DeleteDrawingElementVisitor::deleteElement(const QSharedPointer<UndoStack>& undoStack,
                                     const QSharedPointer<SceneElement<float>>& scene,
                                     DrawingElement<float>& element)
{
    DeleteDrawingElementVisitor v(undoStack, scene);
    element.accept(v);

    return v.update;
}

bool DeleteDrawingElementVisitor::deleteSelected(const QSharedPointer<UndoStack>& undoStack, const QSharedPointer<SceneElement<float> >& scene)
{
    QSharedPointer<UndoStack> nestedUndoStack = QSharedPointer<UndoStack>::create();

    bool update = false;
    int n;
    do {
        n = 0;
        for(auto& element: *scene)
        {
            if (element.isHighlighted() && !element.isInUse())
            {
                update |= DeleteDrawingElementVisitor::deleteElement(nestedUndoStack, scene, element);
                n++;
                break;
            }
        }
    } while (n != 0);

    undoStack->add(std::make_unique<CompositUndoItem>(scene, nestedUndoStack, "Delete selection"));

    return update;
}

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
