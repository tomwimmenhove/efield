#include "newnodemouseoperation.h"
#include "graphics/nodeelement.h"
#include "util/undo/newnodeundoitem.h"

void NewNodeMouseOperation::activate(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    placeNewNodeElement(pointerPosition);
}

void NewNodeMouseOperation::mousePressed(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto highLighted = scene->findFirstHighLighted();
    Q_ASSERT(highLighted != scene->end());
    Q_ASSERT(typeid(*highLighted).hash_code() == typeid(NodeElement<float>&).hash_code());

    undoStack->add(std::make_unique<NewNodeUndoItem>(scene, highLighted->identifier(), highLighted->center()));

    placeNewNodeElement(pointerPosition);
}

void NewNodeMouseOperation::cancelOperation(std::unique_ptr<MouseOperation>& current)
{
    auto highLighted = scene->findFirstHighLighted();

    scene->highlightExclusive(scene->end());
    if (highLighted != scene->end())
        scene->remove(highLighted);

    parent->update();
    current = std::move(parent);
}

void NewNodeMouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition, Qt::MouseButtons)
{
    auto highLighted = scene->findFirstHighLighted();
    Q_ASSERT(highLighted != scene->end());
    Q_ASSERT(typeid(*highLighted).hash_code() == typeid(NodeElement<float>&).hash_code());
    highLighted->anchorNode().setPoint(pointerPosition);

    update();
}

void NewNodeMouseOperation::placeNewNodeElement(const QPoint& pointerPosition)
{
    scene->highlightExclusive(scene->end());

    auto newNode = NodeElement<float>::uniqueElement(SharedNode(scene->newId(), pointerPosition), scene->sceneBounds());
    newNode->setHighlighted(true);
    scene->add(std::move(newNode));

    update();
}
