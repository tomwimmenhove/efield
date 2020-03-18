#include "qclickablelabel.h"

QClickableLabel::QClickableLabel(QWidget* parent)
    : QLabel(parent)
{

}

void QClickableLabel::mouseMoveEvent(QMouseEvent* ev)
{
    emit Mouse_Moved(ev->x(), ev->y());
}

void QClickableLabel::mousePressEvent(QMouseEvent* ev)
{
    emit Mouse_Pressed(ev->x(), ev->y());
}

void QClickableLabel::leaveEvent(QEvent*)
{
    emit Mouse_Left();
}
