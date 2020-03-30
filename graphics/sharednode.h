#ifndef SHAREDNODE_H
#define SHAREDNODE_H

#include <QSharedPointer>
#include <QPoint>

#include "util/refcounted.h"

class SharedNode : public QSharedPointer<Refcounted<QPoint>>
{
public:
    inline SharedNode() { }
    inline SharedNode(const QPoint& p) { this->reset(new Refcounted<QPoint>(p)); }
    inline SharedNode(int x, int y) { this->reset(new Refcounted<QPoint>(x, y)); }

    inline operator QPoint() const { return *this->data(); }

    inline SharedNode& operator= (const QPoint& p)
    {
        *this->data() = p;
        return *this;
    }
};

#endif // SHAREDNODE_H
