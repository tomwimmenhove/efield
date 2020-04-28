#include <QMessageBox>

#include "util/undo/compositundonamegenerator.h"
#include "util/undo/compositundoitem.h"
#include "util/undo/moveundoitem.h"
#include "copydrawingelementvisitor.h"
#include "deletedrawingelementvisitor.h"
#include "elementdependencyvisitor.h"
#include "elementmanipulators.h"

ElementManipulators::ElementManipulators(const QSharedPointer<SceneElement<float>>& scene,
                                         const QSharedPointer<UndoStack>& undoStack)
    : scene(scene), undoStack(undoStack)
{ }

void ElementManipulators::copySelection(QSharedPointer<SceneElement<float>> clipBoardScene, bool deleteAfter)
{
    clipBoardScene->clear();

    CopyDrawingElementVisitor copy(scene, clipBoardScene);

    for(auto& element: *scene)
        if (element.isHighlighted())
            element.accept(copy);

    if (deleteAfter)
        deleteSelected();
}

bool ElementManipulators::paste(QSharedPointer<SceneElement<float>> clipBoardScene)
{
    if (clipBoardScene->selectionBounds().width()  > scene->sceneSize().width() ||
        clipBoardScene->selectionBounds().height() > scene->sceneSize().height())
        return false;

    QSharedPointer<UndoStack> nestedUndoStack = QSharedPointer<UndoStack>::create();

    scene->highlightExclusive(scene->end());

    /* Paste all elements */
    CopyDrawingElementVisitor paste(clipBoardScene, scene, nestedUndoStack);
    for(auto& element: *clipBoardScene)
            element.accept(paste);

    /* And center them */
    QRect rect = scene->selectionBounds().translated(scene->center()- scene->selectionBounds().center());
    QRect clipped = Geometry::clip(rect, scene->sceneSize());
    QPoint delta = clipped.topLeft() - scene->selectionBounds().topLeft();
    for(auto& element: *scene)
    {
        if (element.isHighlighted() && element.canAnchor())
        {
            auto undoItem = std::make_unique<MoveUndoItem>
                            (scene, element.identifier(), element.center(), element.center() + delta);
            undoItem->doFunction();
            nestedUndoStack->add(std::move(undoItem));
        }
    }

    undoStack->add(std::make_unique<CompositUndoItem>( nestedUndoStack, "Paste"));

    update = true;

    return true;
}

void ElementManipulators::deleteSelected()
{
    QSharedPointer<UndoStack> nestedUndoStack = QSharedPointer<UndoStack>::create();
    CompositUndoNameGenerator nameGen("Delete");

    bool deleted = false;
    int n;
    do {
        n = 0;
        for(auto& element: *scene)
        {
            if (element.isHighlighted() && !element.isInUse())
            {
                nameGen.addElementName(element.name());

                DeleteDrawingElementVisitor v(nestedUndoStack, scene);
                element.accept(v);

                deleted |= v.needsUpdate();
                n++;
                break;
            }
        }
    } while (n != 0);

    if (deleted)
        undoStack->add(std::make_unique<CompositUndoItem>( nestedUndoStack, nameGen.generate()));

    update |= deleted;
}

bool ElementManipulators::rotateSelection(double angle)
{
    ElementDependencyVisitor deps;
    deps.allHighlighted(*scene);

    QPoint center = scene->selectionBounds().center();
    QMatrix mat;
    mat.rotate(angle);

    QSharedPointer<UndoStack> nestedUndoStack = QSharedPointer<UndoStack>::create();
    for(auto id: deps.dependencies())
    {
        auto it = scene->findId(id);
        Q_ASSERT(it != scene->end());
        if (!it->canAnchor())
            continue;

        QPoint oldPoint = it->anchorNode().point();
        QPoint newPoint = (oldPoint - center) * mat + center;

        if (!scene->bounds().contains(newPoint))
        {
            nestedUndoStack->undoAll();
            return false;
        }

        auto undoItem = std::make_unique<MoveUndoItem>(scene, it->identifier(), oldPoint, newPoint);
        undoItem->doFunction();
        nestedUndoStack->add(std::move(undoItem));

        update |= true;
    }

    undoStack->add(std::make_unique<CompositUndoItem>(nestedUndoStack, "Rotate selection"));

    return true;
}
