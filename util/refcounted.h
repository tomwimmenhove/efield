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

//    QUuid GetQUuid()
//    {
//        if (uuid.isNull())
//            uuid = QUuid::createUuid();

//        return uuid;
//    }

    inline uint64_t GetId() const { return id; }

    inline void Use() { refCount++; }
    inline void Release() { refCount--; }
    inline int RefCount() { return refCount; }

private:
    T value;
    int refCount = 0;
    //QUuid uuid;

    uint64_t id;
    inline static QAtomicInteger<uint64_t> idCounter;
};

#endif // REFCOUNTED_H
