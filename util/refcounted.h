#ifndef REFCOUNTED_H
#define REFCOUNTED_H

//#include <QUuid>
#include <QAtomicInteger>

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

    inline void use() { refCount++; }
    inline void release() { refCount--; }
    inline int refCounter() { return refCount; }

private:
    T value;
    int refCount = 0;
};

#endif // REFCOUNTED_H
