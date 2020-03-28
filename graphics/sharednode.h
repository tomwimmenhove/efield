#ifndef SHAREDNODE_H
#define SHAREDNODE_H

#include <QSharedPointer>
#include <QPoint>

class SharedNode : public QSharedPointer<QPoint>
{
public:
    inline SharedNode(const QPoint& p) { this->reset(new QPoint(p)); }
    inline SharedNode(int x, int y) { this->reset(new QPoint(x, y)); }

    inline operator const QPoint&() const { return *this->data(); }

    inline SharedNode& operator= (const QPoint& p)
    {
        *this->data() = p;
        return *this;
    }
};

#endif // SHAREDNODE_H
