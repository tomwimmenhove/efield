#ifndef REFCOUNTED_H
#define REFCOUNTED_H

#include <QDebug>

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

    inline void Use() { refCount++; qDebug() << "Refcounted " << (void*) this << ": inc: " << refCount; }
    inline void Release() { refCount--; qDebug() << "Refcounted " << (void*) this << ": dec: " << refCount; }
    inline int RefCount() { return refCount; }

private:
    T value;
    int refCount = 0;
};

#endif // REFCOUNTED_H
