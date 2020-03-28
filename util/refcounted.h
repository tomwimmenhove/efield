#ifndef REFCOUNTED_H
#define REFCOUNTED_H

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

    inline void Use() { refCount++; }
    inline void Release() { refCount--; }
    inline int RefCount() { return refCount; }

private:
    T value;
    int refCount = 0;
};

#endif // REFCOUNTED_H
