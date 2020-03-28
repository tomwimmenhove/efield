#include "mouseeventlabel.h"

MouseEventLabel::MouseEventLabel(QWidget* parent)
    : QLabel(parent)
{ }

void MouseEventLabel::mouseMoveEvent(QMouseEvent* ev)
{
    emit Mouse_Moved(ev->pos());
}

void MouseEventLabel::mousePressEvent(QMouseEvent* ev)
{
    emit Mouse_Pressed(ev->pos(), ev->buttons());
}

void MouseEventLabel::leaveEvent(QEvent*)
{
    emit Mouse_Left();
}

void MouseEventLabel::resizeEvent(QResizeEvent* ev)
{
    emit Resized(ev->size());
}
