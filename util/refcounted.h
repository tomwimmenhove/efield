#ifndef REFCOUNTED_H
#define REFCOUNTED_H

#include <QUuid>

#include <utility>

template<typename T>
class Refcounted
{
public:
    template<typename... Args>
    Refcounted(Args && ...arguments)
        : value(std::forward<Args>(arguments)...)
    { }

    inline operator T() { return value; }

    inline Refcounted& operator= (T x)
    {
        value = x;
        return *this;
    }

    QUuid GetQUuid()
    {
        if (uuid.isNull())
            uuid = QUuid::createUuid();

        return uuid;
    }

    inline void Use() { refCount++; }
    inline void Release() { refCount--; }
    inline int RefCount() { return refCount; }

private:
    T value;
    int refCount = 0;
    QUuid uuid;
};

#endif // REFCOUNTED_H
