#include "mouseeventlabel.h"

MouseEventLabel::MouseEventLabel(QWidget* parent)
    : QLabel(parent)
{

}

void MouseEventLabel::mouseMoveEvent(QMouseEvent* ev)
{
    emit Mouse_Moved(ev->x(), ev->y());
}

void MouseEventLabel::mousePressEvent(QMouseEvent* ev)
{
    emit Mouse_Pressed(ev->x(), ev->y());
}

void MouseEventLabel::leaveEvent(QEvent*)
{
    emit Mouse_Left();
}