#include "newlinemouseoperation.h"
#include "graphics/lineelement.h"
#include "util/undo/newlineundoitem.h"

std::unique_ptr<MouseOperation> NewLineMouseOperation::mousePressed(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition)
{
    auto highLighted = scene->findFirstHighLighted();
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
                scene->add(std::move(LineElement<float>::uniqueElement(scene->newId(), scene->sceneSize(), sharedStartNode, sharedEndNode, 0)));

                state = State::p2;
                break;
            }

            return std::move(current);
        case State::p2:
        {
            if (highLighted != scene->end())
            {
                /* Set the definitive end point for the new line segment */
                Q_ASSERT(highLighted->canAnchor());
                Q_ASSERT(typeid(scene->back()).hash_code() == typeid(LineElement<float>&).hash_code());

                if (highLighted->identifier() != startId)
                {
                    auto& line = static_cast<LineElement<float>&>(scene->back());
                    line.setPoint2(highLighted->anchorNode());

                    undoStack->add(std::make_unique<NewLineUndoItem>(scene, line.identifier(),
                                                   line.point1().identifier(), line.point2().identifier(), line.value()));

                    state = State::p1;
                }
            }
            scene->highlightExclusive(scene->end());
            break;
        }
    }

    update();

    return std::move(current);
}

std::unique_ptr<MouseOperation> NewLineMouseOperation::cancelOperation(std::unique_ptr<MouseOperation>&& current)
{
    if (state == State::p2)
    {
        scene->highlightExclusive(scene->end());

        Q_ASSERT(typeid(scene->back()).hash_code() == typeid(LineElement<float>&).hash_code());
        scene->pop();

        state = State::p1;
        update();

        return std::move(current);
    }

    parent->update();

    return std::move(parent);
}

std::unique_ptr<MouseOperation> NewLineMouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons)
{
    auto closest = scene->closestElement(pointerPosition, [](const DrawingElement<float>& e) { return e.canAnchor(); });

    switch (state)
    {
        case State::p1:
        {
            scene->highlightExclusive(closest);
            break;
        }
        case State::p2:
        {
            if (closest == scene->end())
                scene->highlightExclusive(scene->end());
            else
                if (closest->identifier() != startId)
                    scene->highlightExclusive(closest);

            Q_ASSERT(typeid(scene->back()).hash_code() == typeid(LineElement<float>&).hash_code());
            auto& line = static_cast<LineElement<float>&>(scene->back());
            line.point2().setPoint(pointerPosition);

            break;
        }
    }

    update();

    return std::move(current);
}
