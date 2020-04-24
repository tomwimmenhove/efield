#include <QGuiApplication>
#include <QInputDialog>

#include "graphics/nodeelement.h"
#include "graphics/lineelement.h"
#include "normalmouseoperation.h"
#include "dragmouseoperation.h"
#include "selectionmouseoperation.h"
#include "view/pointinputdialog.h"

std::unique_ptr<MouseOperation> NormalMouseOperation::mousePressed(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition)
{
    mousePressedAt = pointerPosition;

    bool controlPressed = QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier);

    auto closest = scene->closestElement(pointerPosition);
    if (closest != scene->end())
    {
        auto closestHighlighted = scene->closestElement(pointerPosition,
            [] (const DrawingElement<float>& e) { return e.isHighlighted(); });

        /* Prefer selected elements. This helps with moving pasted elements that are exactly on top of their parents */
        bool closerToSelected = closestHighlighted != scene->end() &&
                                 closestHighlighted->distanceTo(pointerPosition) <= closest->distanceTo(pointerPosition);

        if(controlPressed || !closerToSelected)
        {
            if (controlPressed)
                closest->setHighlighted(!closest->isHighlighted());
            else
                scene->highlightExclusive(closest);

            update();
        }

        return std::move(current);
    }

    if (controlPressed)
        return std::move(current);

    std::unique_ptr<MouseOperation> newOperation = std::make_unique<SelectionMouseOperation>(std::move(current),
                                                                                             undoStack, scene);
    newOperation = std::move(newOperation->activate(std::move(newOperation), pointerPosition));
    newOperation->update();

    return std::move(newOperation);
}

std::unique_ptr<MouseOperation> NormalMouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons buttons)
{
    if ((buttons & Qt::LeftButton) == 0)
        return std::move(current);

    if (!checkStartedDragging(pointerPosition))
        return std::move(current);

    startedDragging = false;

    auto closest = scene->closestElement(mousePressedAt);
    if (closest != scene->end())
    {
        std::unique_ptr<MouseOperation> newOperation = std::make_unique<DragMouseOperation>(std::move(current),
                                                                 undoStack, scene);

        newOperation = std::move(newOperation->activate(std::move(newOperation), mousePressedAt));
        newOperation = std::move(newOperation->mouseMoved(std::move(newOperation), pointerPosition, buttons));

        return std::move(newOperation);
    }

    return std::move(current);
}

std::unique_ptr<MouseOperation> NormalMouseOperation::mouseDoubleClicked(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons buttons)
{
    if (buttons != Qt::LeftButton)
        return std::move(current);

    auto closest = scene->closestElement(pointerPosition);
    if (closest != scene->end())
        emit editElement(*closest);

    return std::move(current);
}

bool NormalMouseOperation::checkStartedDragging(const QPoint& pointerPosition)
{
    if (!startedDragging)
    {
        QPoint p = pointerPosition - mousePressedAt;
        if (p.manhattanLength() > 5)
            startedDragging = true;
    }

    return startedDragging;
}
