#ifndef MOUSEEVENTLABEL_H
#define MOUSEEVENTLABEL_H

#include <QObject>
#include <QLabel>
#include <QMouseEvent>

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
    void Mouse_Moved(int x, int y);
    void Mouse_Pressed(int x, int y);
    void Mouse_Left();
    void Resized(QSize size);
};

#endif // MOUSEEVENTLABEL_H
