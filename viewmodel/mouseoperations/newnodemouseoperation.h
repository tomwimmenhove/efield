#ifndef NEWNODEMOUSEOPERATION_H
#define NEWNODEMOUSEOPERATION_H

#include "mouseoperation.h"

class NewNodeMouseOperation : public MouseOperation
{
public:
    NewNodeMouseOperation(std::unique_ptr<MouseOperation>&& parent, QSharedPointer<SceneElement<float>> scene) noexcept
        : MouseOperation(std::move(parent), scene)
    { }

    Qt::CursorShape cursorShape() const { return Qt::CrossCursor; }

    void activateOperation(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
    {
        placeNewNodeElement(pointerPosition);
    }

    void cancelOperation(std::unique_ptr<MouseOperation>& current)
    {
        auto highLighted = scene->findHighLighted();

        scene->highlight(scene->end());
        if (highLighted != scene->end())
            scene->remove(highLighted);

        current = std::move(parent);
    }

    void mouseMoved(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
    {
        auto highLighted = scene->findHighLighted();
        Q_ASSERT(highLighted != scene->end());
        Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
        NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);
        node.node().setPoint(pointerPosition);
    }

private:
    void placeNewNodeElement(const QPoint& pointerPosition)
    {
        scene->highlight(scene->end());

        std::unique_ptr<DrawingElement<float>> newNode = NodeElement<float>::uniqueElement(SharedNode(pointerPosition));
        newNode->setHighlighted(true);
        scene->add(std::move(newNode));
    }

    QPoint nodeSavedPos;
};

#endif // NEWNODEMOUSEOPERATION_H
