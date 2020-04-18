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
        for(auto& i: *scene)
        {
            if (i.isHighlighted() && !i.isInUse())
            {
                update |= DeleteDrawingElementVisitor::deleteElement(nestedUndoStack, scene, i);
                n++;
                break;
            }
        }
    } while (n != 0);

    undoStack->add(std::make_unique<CompositUndoItem>(scene, nestedUndoStack, "Delete selected"));

    return update;
}

void DeleteDrawingElementVisitor::visit(SceneElement<float>&)
{ }

void DeleteDrawingElementVisitor::visit(NodeElement<float>& node)
{
    auto undoItem = std::make_unique<DeleteNodeUndoItem>(scene, node.identifier(), "Delete node");
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
