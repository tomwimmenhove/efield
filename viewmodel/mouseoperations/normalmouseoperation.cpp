#include <QGuiApplication>
#include <QInputDialog>

#include "graphics/nodeelement.h"
#include "graphics/lineelement.h"
#include "normalmouseoperation.h"
#include "dragmouseoperation.h"
#include "selectionmouseoperation.h"
#include "pointinputdialog.h"

void NormalMouseOperation::mousePressed(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition)
{
    mousePressedAt = pointerPosition;
    pressed = true;

    bool controlPressed = QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier);

    auto closest = scene->closestElement(pointerPosition);
    if (closest != scene->end())
    {
        if(controlPressed || !closest->isHighlighted())
        {
            if (controlPressed)
                closest->setHighlighted(!closest->isHighlighted());
            else
                scene->highlightExclusive(closest);

            update();
        }

        return;
    }

    if (controlPressed)
        return;

    current = std::make_unique<SelectionMouseOperation>(std::move(current),
                                                   undoStack, scene);
    current->activate(current, pointerPosition);

    update();
}

void NormalMouseOperation::mouseMoved(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition)
{
    if (!pressed)
        return;

    auto closest = scene->closestElement(mousePressedAt);
    if (closest != scene->end())
    {
        pressed = false;
        current = std::make_unique<DragMouseOperation>(std::move(current),
                                                       undoStack, scene);
        current->activate(current, mousePressedAt);
        current->mouseMoved(current, pointerPosition);
        return;
    }
}

void NormalMouseOperation::mouseReleased(std::unique_ptr<MouseOperation>&, const QPoint&, Qt::MouseButtons buttons)
{
    if (buttons != Qt::LeftButton)
        pressed = false;
}

void NormalMouseOperation::mouseDoubleClicked(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition, Qt::MouseButtons buttons)
{
    if (buttons != Qt::LeftButton)
        return;

    auto closest = scene->closestElement(pointerPosition);
    if (closest != scene->end())
        emit editElement(*closest);
}
