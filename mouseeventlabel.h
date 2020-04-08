#ifndef MOUSEEVENTLABEL_H
#define MOUSEEVENTLABEL_H

#include <QObject>
#include <QLabel>
#include <QMouseEvent>
#include <QPoint>

class MouseEventLabel : public QLabel
{
    Q_OBJECT

public:
    explicit MouseEventLabel(QWidget* parent = 0);

    void mouseMoveEvent(QMouseEvent* ev);
    void mousePressEvent(QMouseEvent* ev);
    void mouseReleaseEvent(QMouseEvent* ev);
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void leaveEvent(QEvent* ev);

    void resizeEvent(QResizeEvent* ev);

signals:
    void mouse_Moved(const QPoint& pos);
    void mouse_Pressed(const QPoint& pos, Qt::MouseButtons buttons);
    void mouse_Released(const QPoint& pos, Qt::MouseButtons buttons);
    void mouse_DoubleClicked(const QPoint& pos, Qt::MouseButtons buttons);
    void mouse_Left();
    void resized(const QSize& size);
};

#endif // MOUSEEVENTLABEL_H
