#include "mouseeventlabel.h"

MouseEventLabel::MouseEventLabel(QWidget* parent)
    : QLabel(parent)
{ }

void MouseEventLabel::mouseMoveEvent(QMouseEvent* ev)
{
    emit mouse_Moved(ev->pos(), ev->buttons());
}

void MouseEventLabel::mousePressEvent(QMouseEvent* ev)
{
    emit mouse_Pressed(ev->pos(), ev->buttons());
}

void MouseEventLabel::mouseReleaseEvent(QMouseEvent* ev)
{
    emit mouse_Released(ev->pos(), ev->buttons());
}

void MouseEventLabel::mouseDoubleClickEvent(QMouseEvent* ev)
{
    emit mouse_DoubleClicked(ev->pos(), ev->buttons());
}

void MouseEventLabel::leaveEvent(QEvent*)
{
    emit mouse_Left();
}

void MouseEventLabel::resizeEvent(QResizeEvent* ev)
{
    emit resized(ev->size());
}
