#ifndef QCLICKABLELABEL_H
#define QCLICKABLELABEL_H

#include <QObject>
#include <QLabel>
#include <QMouseEvent>

class QClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit QClickableLabel(QWidget* parent = 0);

    void mouseMoveEvent(QMouseEvent* ev);
    void mousePressEvent(QMouseEvent* ev);
    void leaveEvent(QEvent* ev);

signals:
    void Mouse_Moved(int x, int y);
    void Mouse_Pressed(int x, int y);
    void Mouse_Left();
};

#endif // QCLICKABLELABEL_H
