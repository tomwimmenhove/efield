#ifndef NEWLINEMOUSEOPERATION_H
#define NEWLINEMOUSEOPERATION_H

#include "mouseoperation.h"

class NewLineMouseOperation : public MouseOperation
{
public:
    NewLineMouseOperation(std::unique_ptr<MouseOperation>&& parent, QSharedPointer<SceneElement<float>> scene) noexcept
        : MouseOperation(std::move(parent), scene)
    { }

    Qt::CursorShape cursorShape() const { return Qt::CrossCursor; }

    void activateOperation(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
    {
        auto highLighted = scene->findHighLighted();
        auto closest = scene->closestElement(pointerPosition);

        switch (state)
        {
            case State::p1:
                if (closest != scene->end() && highLighted != scene->end())
                {
                    Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
                    NodeElement<float>& startNode = static_cast<NodeElement<float>&>(*highLighted);
                    SharedNode sharedStartNode = startNode.node();
                    SharedNode sharedEndNode = SharedNode(pointerPosition);
                    scene->add(std::move(LineElement<float>::uniqueElement(sharedStartNode, sharedEndNode, 0)));

                    state = State::p2;
                    break;
                }
                return;
            case State::p2:
            {
                if (highLighted != scene->end())
                {
                    /* Set the definitive end point for the new line segment */
                    Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
                    NodeElement<float>& endNode = static_cast<NodeElement<float>&>(*highLighted);
                    Q_ASSERT(scene->back().elementType() == drawingElementType::Line);
                    LineElement<float>& newLine = static_cast<LineElement<float>&>(scene->back());

                    if (endNode.node()->identifier() != newLine.point1()->identifier())
                    {
                        newLine.setPoint2(endNode.node());
                        state = State::p1;
                    }
                }
                scene->highlight(scene->end());
                break;
            }
        }
    }

    void cancelOperation(std::unique_ptr<MouseOperation>& current)
    {
        scene->highlight(scene->end());

        if (state == State::p2)
        {
            Q_ASSERT(scene->back().elementType() == drawingElementType::Line);
            scene->pop();
        }

        current = std::move(parent);
    }

    void mouseMoved(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
    {
        auto closest = scene->closestElement(pointerPosition);

        switch (state)
        {
            case State::p1:
            {
                /* Highlight close nodes */
                if (closest == scene->end())
                {
                    scene->highlight(scene->end());
                    break;
                }

                if (closest->elementType() == drawingElementType::Node)
                {
                    scene->highlight(closest);
                    break;
                }

                return;
            }
            case State::p2:
            {
                Q_ASSERT(scene->back().elementType() == drawingElementType::Line);
                LineElement<float>& newLine = static_cast<LineElement<float>&>(scene->back());

                if (closest == scene->end())
                    scene->highlight(scene->end());
                else if (closest->elementType() == drawingElementType::Node &&
                         static_cast<NodeElement<float>&>(*closest).node()->identifier() != newLine.point1()->identifier())
                    scene->highlight(closest);

                newLine.point2().setPoint(pointerPosition);
                break;
            }
        }
    }

private:
    QPoint nodeSavedPos;

    enum class State
    {
        p1,
        p2
    };

    State state = State::p1;
};
#endif // NEWLINEMOUSEOPERATION_H
