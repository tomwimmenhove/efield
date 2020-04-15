#include "newlinemouseoperation.h"
#include "graphics/lineelement.h"
#include "util/undo/newlineundoitem.h"

void NewLineMouseOperation::mousePressed(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto highLighted = scene->findHighLighted();
    auto closest = scene->closestElement(pointerPosition);

    switch (state)
    {
        case State::p1:
            if (closest != scene->end() && highLighted != scene->end())
            {
                Q_ASSERT(highLighted->canAnchor());
                startId = highLighted->identifier();
                SharedNode sharedStartNode = highLighted->anchorNode();
                SharedNode sharedEndNode = SharedNode(-1, pointerPosition);
                scene->add(std::move(LineElement<float>::uniqueElement(scene->newId(), scene->bounds(), sharedStartNode, sharedEndNode, 0)));

                state = State::p2;
                break;
            }
            return;
        case State::p2:
        {
            if (highLighted != scene->end())
            {
                /* Set the definitive end point for the new line segment */
                Q_ASSERT(highLighted->canAnchor());
                Q_ASSERT(scene->back().elementType() == drawingElementType::Line);

                if (highLighted->identifier() != startId)
                {
                    auto& line = static_cast<LineElement<float>&>(scene->back());
                    line.setPoint2(highLighted->anchorNode());

                    undoStack->add(NewLineUndoItem(scene, line.identifier(),
                                                   line.point1().identifier(), line.point2().identifier(), line.value(), "Place line"));

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
    auto closest = scene->closestElement(pointerPosition, [](const DrawingElement<float>& e) { return e.canAnchor(); });

    switch (state)
    {
        case State::p1:
        {
            scene->highlight(closest);
            break;
        }
        case State::p2:
        {
            if (closest == scene->end())
                scene->highlight(scene->end());
            else
                if (closest->identifier() != startId)
                    scene->highlight(closest);

            Q_ASSERT(scene->back().elementType() == drawingElementType::Line);
            static_cast<LineElement<float>&>(scene->back()).point2().setPoint(pointerPosition);
            break;
        }
    }

    update();
}
