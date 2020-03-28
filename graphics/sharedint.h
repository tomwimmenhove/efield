#ifndef SHAREDINT_H
#define SHAREDINT_H

#include <QSharedPointer>

class SharedInt : public QSharedPointer<int>
{
public:
    inline SharedInt(int x) { this->reset(new int(x)); }

    inline operator int() const { return *this->data(); }

    inline SharedInt& operator= (int x)
    {
        *this->data() = x;
        return *this;
    }
};

#endif // SHAREDINT_H
