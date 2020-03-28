#ifndef SHAREDINT_H
#define SHAREDINT_H

#include <QSharedPointer>

#include "util/refcounted.h"

class SharedInt : public QSharedPointer<Refcounted<int>>
{
public:
    inline SharedInt(int x) { this->reset(new Refcounted<int>(x)); }

    inline operator int() { return *this->data(); }

    inline SharedInt& operator= (int x)
    {
        *this->data() = x;
        return *this;
    }
};

#endif // SHAREDINT_H
