#include "newnodemouseoperation.h"
#include "graphics/nodeelement.h"
#include "util/undo/newnodeundoitem.h"

std::unique_ptr<MouseOperation> NewNodeMouseOperation::activate(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition)
{
    placeNewNodeElement(pointerPosition);

    return std::move(current);
}

std::unique_ptr<MouseOperation> NewNodeMouseOperation::mousePressed(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition)
{
    auto highLighted = scene->findFirstHighLighted();
    Q_ASSERT(highLighted != scene->end());
    Q_ASSERT(typeid(*highLighted).hash_code() == typeid(NodeElement<float>&).hash_code());

    undoStack->add(std::make_unique<NewNodeUndoItem>(scene, highLighted->identifier(), highLighted->center()));

    placeNewNodeElement(pointerPosition);

    return std::move(current);
}

std::unique_ptr<MouseOperation> NewNodeMouseOperation::cancelOperation(std::unique_ptr<MouseOperation>&&)
{
    auto highLighted = scene->findFirstHighLighted();

    scene->highlightExclusive(scene->end());
    if (highLighted != scene->end())
        scene->remove(highLighted);

    parent->update();

    return std::move(parent);
}

std::unique_ptr<MouseOperation> NewNodeMouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons)
{
    auto highLighted = scene->findFirstHighLighted();
    Q_ASSERT(highLighted != scene->end());
    Q_ASSERT(typeid(*highLighted).hash_code() == typeid(NodeElement<float>&).hash_code());
    highLighted->anchorNode().setPoint(pointerPosition);

    update();

    return std::move(current);
}

void NewNodeMouseOperation::placeNewNodeElement(const QPoint& pointerPosition)
{
    scene->highlightExclusive(scene->end());

    auto newNode = NodeElement<float>::uniqueElement(SharedNode(scene->newId(), pointerPosition), scene->sceneSize());
    newNode->setHighlighted(true);
    scene->add(std::move(newNode));

    update();
}
