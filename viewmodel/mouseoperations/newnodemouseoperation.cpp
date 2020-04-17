#include "newnodemouseoperation.h"
#include "graphics/nodeelement.h"
#include "util/undo/newnodeundoitem.h"

void NewNodeMouseOperation::activate(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    placeNewNodeElement(pointerPosition);
}

void NewNodeMouseOperation::mousePressed(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto highLighted = scene->findHighLighted();
    Q_ASSERT(highLighted != scene->end());
    Q_ASSERT(highLighted->elementType() == drawingElementType::Node);

    undoStack->add(std::make_unique<NewNodeUndoItem>(scene, highLighted->identifier(), highLighted->center(), "Place node"));

    placeNewNodeElement(pointerPosition);
}

void NewNodeMouseOperation::cancelOperation(std::unique_ptr<MouseOperation>& current)
{
    auto highLighted = scene->findHighLighted();

    scene->highlight(scene->end());
    if (highLighted != scene->end())
        scene->remove(highLighted);

    parent->update();
    current = std::move(parent);
}

void NewNodeMouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto highLighted = scene->findHighLighted();
    Q_ASSERT(highLighted != scene->end());
    Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
    highLighted->anchorNode().setPoint(pointerPosition);

    update();
}

void NewNodeMouseOperation::placeNewNodeElement(const QPoint& pointerPosition)
{
    scene->highlight(scene->end());

    auto newNode = NodeElement<float>::uniqueElement(SharedNode(scene->newId(), pointerPosition), scene->bounds());
    newNode->setHighlighted(true);
    scene->add(std::move(newNode));

    update();
}
