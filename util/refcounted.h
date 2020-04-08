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
    {
        id = idCounter++;
    }

    inline operator T() { return value; }

    inline Refcounted& operator= (T x)
    {
        value = x;
        return *this;
    }

//    QUuid getQUuid()
//    {
//        if (uuid.isNull())
//            uuid = QUuid::createUuid();

//        return uuid;
//    }

    inline uint64_t identifier() const { return id; }

    inline void use() { refCount++; }
    inline void release() { refCount--; }
    inline int refCounter() { return refCount; }

private:
    T value;
    int refCount = 0;
    //QUuid uuid;

    uint64_t id;
    static QAtomicInteger<uint64_t> idCounter;
};

template<typename T> QAtomicInteger<uint64_t> Refcounted<T>::idCounter;

#endif // REFCOUNTED_H
