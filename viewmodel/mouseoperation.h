#ifndef MOUSEOPERATION_H
#define MOUSEOPERATION_H

#include <memory>
#include <QCursor>
#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "graphics/lineelement.h"
#include "graphics/nodeelement.h"

class MouseOperation
{
public:
    MouseOperation(std::unique_ptr<MouseOperation>&& parent, QSharedPointer<SceneElement<float>> scene) noexcept
        : parent(std::move(parent)), scene(scene)
    { }

    virtual Qt::CursorShape cursorShape() const { return Qt::ArrowCursor; }

    virtual void activateOperation(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition)
    {
        (void) current;
        (void) pointerPosition;
    }

    virtual void cancelOperation(std::unique_ptr<MouseOperation>& current)
    {
        (void) current;
    }

    virtual void mouseMoved(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition)
    {
        (void) current;
        (void) pointerPosition;
    }

    virtual void mouseReleased(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition, Qt::MouseButtons buttons)
    {
        (void) current;
        (void) pointerPosition;
        (void) buttons;
    }

    virtual void mouseDoubleClicked(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition, Qt::MouseButtons buttons)
    {
        (void) current;
        (void) pointerPosition;
        (void) buttons;
    }

    virtual ~MouseOperation() { }

protected:
    std::unique_ptr<MouseOperation> parent;
    QSharedPointer<SceneElement<float>> scene;
};

class DragNodeMouseOperation : public MouseOperation
{
public:
    DragNodeMouseOperation(std::unique_ptr<MouseOperation>&& parent, QSharedPointer<SceneElement<float>> scene) noexcept
        : MouseOperation(std::move(parent), scene)
    { }

    Qt::CursorShape cursorShape() const { return Qt::CrossCursor; }

    void activateOperation(std::unique_ptr<MouseOperation>& current, const QPoint&)
    {
        scene->highlight(scene->end());
        current = std::move(parent);
    }

    void cancelOperation(std::unique_ptr<MouseOperation>& current)
    {
        auto highLighted = scene->findHighLighted();
        Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
        NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);
        node.node().setPoint(nodeSavedPos);

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

    void mouseReleased(std::unique_ptr<MouseOperation>& current, const QPoint&, Qt::MouseButtons buttons)
    {
        if (buttons == Qt::RightButton)
            return;

        current = std::move(parent);
    }

private:
    QPoint nodeSavedPos;
};

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

class NormalMouseOperation : public MouseOperation
{
public:
    NormalMouseOperation(QSharedPointer<SceneElement<float>> scene) noexcept
        : MouseOperation(std::move(std::unique_ptr<MouseOperation>(nullptr)), scene)
    { }

    void activateOperation(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition)
    {
        auto closest = scene->closestElement(pointerPosition);
        auto highLighted = scene->findHighLighted();

        if (highLighted != scene->end() && closest == highLighted &&
            highLighted->elementType() == drawingElementType::Node)
        {
            current = std::make_unique<DragNodeMouseOperation>(std::move(current), scene);
        }

        scene->highlight(closest);
    }

private:
    QPoint nodeSavedPos;
};


#endif // MOUSEOPERATION_H
