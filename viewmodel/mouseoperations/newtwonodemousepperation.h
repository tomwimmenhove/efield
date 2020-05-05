#ifndef NEWTWONODEMOUSEPPERATION_H
#define NEWTWONODEMOUSEPPERATION_H

#include "mouseoperation.h"

template <typename T, typename U>
class NewTwoNodeMouseOperation : public MouseOperation
{
public:
    NewTwoNodeMouseOperation(std::unique_ptr<MouseOperation>&& parent,
                          const QSharedPointer<UndoStack>& undoStack,
                          const QSharedPointer<SceneElement<float>>& scene) noexcept
        : MouseOperation(std::move(parent), undoStack, scene)
    { }

    Qt::CursorShape cursorShape() const override { return Qt::CrossCursor; }

    std::unique_ptr<MouseOperation> mousePressed(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition) override
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

                    auto element = std::make_unique<T>(scene->newId(), scene->sceneSize(), sharedStartNode, sharedEndNode, 0);
                    scene->add(std::move(element));

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
                    Q_ASSERT(typeid(scene->back()).hash_code() == typeid(T&).hash_code());

                    if (highLighted->identifier() != startId)
                    {
                        auto& line = static_cast<T&>(scene->back());
                        line.setPoint2(highLighted->anchorNode());

                        undoStack->add(std::make_unique<U>(scene, line.identifier(),
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

    std::unique_ptr<MouseOperation> cancelOperation(std::unique_ptr<MouseOperation>&& current) override
    {
        if (state == State::p2)
        {
            scene->highlightExclusive(scene->end());

            Q_ASSERT(typeid(scene->back()).hash_code() == typeid(T&).hash_code());
            scene->pop();

            state = State::p1;
            update();

            return std::move(current);
        }

        parent->update();

        return std::move(parent);
    }

    std::unique_ptr<MouseOperation> mouseMoved(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons) override
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

                Q_ASSERT(typeid(scene->back()).hash_code() == typeid(T&).hash_code());
                auto& line = static_cast<T&>(scene->back());
                line.point2().setPoint(pointerPosition);

                break;
            }
        }

        update();

        return std::move(current);
    }

private:
    QPoint nodeSavedPos;

    enum class State
    {
        p1,
        p2
    };

    int startId;

    State state = State::p1;
};

#endif // NEWTWONODEMOUSEPPERATION_H
