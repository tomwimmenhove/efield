#ifndef SHAREDNODE_H
#define SHAREDNODE_H

#include <QSharedPointer>
#include <QPoint>

#include "util/refcounted.h"

class SharedNode : public QSharedPointer<Refcounted<QPoint>>
{
public:
    inline SharedNode()
    { }

    inline SharedNode(int id)
        : id(id)
    { }

    inline explicit SharedNode(int id, const QPoint& p)
        : id(id)
    {
        this->reset(new Refcounted<QPoint>(p));
    }

    inline explicit SharedNode(int id, int x, int y)
        : id(id)
    {
        this->reset(new Refcounted<QPoint>(x, y));
    }

    inline int identifier() const { return id; }

    inline operator QPoint() const { return *this->data(); }

    inline QPoint point() const { return *this->data(); }
    inline void setPoint(const QPoint& p) { *this->data() = p; }

private:
    int id;
};

#endif // SHAREDNODE_H
