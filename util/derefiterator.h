#ifndef DEREFITERATOR_H
#define DEREFITERATOR_H

template<typename T>
class DerefIterator
{
public:
    DerefIterator(T i)
        : i(i)
    { }

    using difference_type = typename T::difference_type;
    using value_type = typename T::value_type::element_type;
    using iterator_category = typename T::iterator_category;
    using pointer = value_type*;
    using reference = value_type&;

    T innerIterator() const { return i; }

    bool operator ==(const DerefIterator& other) { return i == other.i; }
    bool operator !=(const DerefIterator& other) { return i != other.i; }
    difference_type operator -(const DerefIterator& other) const { return i - other.innerIterator(); }

    reference operator*() const { return **i; }
    pointer operator->() const { return (*i).get(); }

    DerefIterator & operator++() { ++i; return *this; }
    DerefIterator & operator--() { --i; return *this; }

private:
    T i;
};

#endif // DEREFITERATOR_H
