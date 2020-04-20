#include "mouseoperation.h"

std::unique_ptr<MouseOperation> MouseOperation::activate(std::unique_ptr<MouseOperation>&& current, const QPoint&)
{
    return std::move(current);
}

std::unique_ptr<MouseOperation> MouseOperation::cancelOperation(std::unique_ptr<MouseOperation>&& current)
{
    return std::move(current);
}

std::unique_ptr<MouseOperation> MouseOperation::mousePressed(std::unique_ptr<MouseOperation>&& current, const QPoint&)
{
    return std::move(current);
}

std::unique_ptr<MouseOperation> MouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&& current, const QPoint&, Qt::MouseButtons)
{
    return std::move(current);
}

std::unique_ptr<MouseOperation> MouseOperation::mouseReleased(std::unique_ptr<MouseOperation>&& current, const QPoint&, Qt::MouseButtons)
{
    return std::move(current);
}

std::unique_ptr<MouseOperation> MouseOperation::mouseDoubleClicked(std::unique_ptr<MouseOperation>&& current, const QPoint&, Qt::MouseButtons)
{
    return std::move(current);
}

bool MouseOperation::popUpdate()
{
    if (needsUpdate)
    {
        needsUpdate = false;
        return true;
    }

    return false;
}
