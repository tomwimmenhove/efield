#ifndef SHAREDNODE_H
#define SHAREDNODE_H

#include <QSharedPointer>
#include <QPoint>

#include "util/refcounted.h"

class SharedNode : public QSharedPointer<Refcounted<QPoint>>
{
public:
    inline SharedNode() { }
    inline explicit SharedNode(const QPoint& p) { this->reset(new Refcounted<QPoint>(p)); }
    inline explicit SharedNode(int x, int y) { this->reset(new Refcounted<QPoint>(x, y)); }

    inline operator QPoint() const { return *this->data(); }

    inline QPoint Point() const { return *this->data(); }
    inline void SetPosition(const QPoint& p) { *this->data() = p; }

//    inline SharedNode& operator= (const QPoint& p)
//    {
//        *this->data() = p;
//        return *this;
//    }
};

#endif // SHAREDNODE_H
