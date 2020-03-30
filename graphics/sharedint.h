#ifndef SHAREDINT_H
#define SHAREDINT_H

#include <QSharedPointer>

#include "util/refcounted.h"

class SharedInt : public QSharedPointer<Refcounted<int>>
{
public:
    inline explicit SharedInt(int x) { this->reset(new Refcounted<int>(x)); }

    inline operator int() { return *this->data(); }

    inline int Int() const { return *this->data(); }
    inline void SetInt(int x) { *this->data() = x; }

//    inline SharedInt& operator= (int x)
//    {
//        *this->data() = x;
//        return *this;
//    }
};

#endif // SHAREDINT_H
