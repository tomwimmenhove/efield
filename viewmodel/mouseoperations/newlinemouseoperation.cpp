#include "newlinemouseoperation.h"
#include "graphics/lineelement.h"

void NewLineMouseOperation::mousePressed(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto highLighted = scene->findHighLighted();
    auto closest = scene->closestElement(pointerPosition);

    switch (state)
    {
        case State::p1:
            if (closest != scene->end() && highLighted != scene->end())
            {
                Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
                auto& startNode = static_cast<NodeElement<float>&>(*highLighted);
                startId = startNode.identifier();
                SharedNode sharedStartNode = startNode.node();
                SharedNode sharedEndNode = SharedNode(-1, pointerPosition);
                scene->add(std::move(LineElement<float>::uniqueElement(scene->newId(), sharedStartNode, sharedEndNode, 0)));

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
                auto& endNode = static_cast<NodeElement<float>&>(*highLighted);
                Q_ASSERT(scene->back().elementType() == drawingElementType::Line);
                auto& newLine = static_cast<LineElement<float>&>(scene->back());

                if (endNode.identifier() != startId)
                {
                    newLine.setPoint2(endNode.node());
                    state = State::p1;
                }
            }
            scene->highlight(scene->end());
            break;
        }
    }

    update();
}

void NewLineMouseOperation::cancelOperation(std::unique_ptr<MouseOperation>& current)
{
    if (state == State::p2)
    {
        scene->highlight(scene->end());

        Q_ASSERT(scene->back().elementType() == drawingElementType::Line);
        scene->pop();

        state = State::p1;
        update();

        return;
    }

    parent->update();
    current = std::move(parent);
}

void NewLineMouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto closest = scene->closestElement(pointerPosition, drawingElementType::Node);

    switch (state)
    {
        case State::p1:
        {
            scene->highlight(closest);
            break;
        }
        case State::p2:
        {
            Q_ASSERT(scene->back().elementType() == drawingElementType::Line);
            auto& newLine = static_cast<LineElement<float>&>(scene->back());

            if (closest == scene->end())
                scene->highlight(scene->end());
            else
            {
                Q_ASSERT(closest->elementType() == drawingElementType::Node);
                auto& closestNode = static_cast<NodeElement<float>&>(*closest);
                if (closestNode.identifier() != startId)
                    scene->highlight(closest);
            }

            newLine.point2().setPoint(pointerPosition);
            break;
        }
    }

    update();
}
