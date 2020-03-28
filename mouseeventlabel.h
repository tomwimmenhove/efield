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
    void leaveEvent(QEvent* ev);

    void resizeEvent(QResizeEvent* ev);

signals:
    void Mouse_Moved(const QPoint& pos);
    void Mouse_Pressed(const QPoint& pos, Qt::MouseButtons buttons);
    void Mouse_Left();
    void Resized(const QSize& size);
};

#endif // MOUSEEVENTLABEL_H
