#include "mouseoperation.h"

void MouseOperation::activate(std::unique_ptr<MouseOperation>&, const QPoint&)
{ }

void MouseOperation::cancelOperation(std::unique_ptr<MouseOperation>&)
{ }

void MouseOperation::mousePressed(std::unique_ptr<MouseOperation>&, const QPoint&)
{ }

void MouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&, const QPoint&)
{ }

void MouseOperation::mouseReleased(std::unique_ptr<MouseOperation>&, const QPoint&, Qt::MouseButtons)
{ }

void MouseOperation::mouseDoubleClicked(std::unique_ptr<MouseOperation>&, const QPoint&, Qt::MouseButtons)
{ }

bool MouseOperation::popUpdate()
{
    if (needsUpdate)
    {
        needsUpdate = false;
        return true;
    }

    return false;
}
