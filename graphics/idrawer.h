#ifndef IDRAWER_H
#define IDRAWER_H

template <typename T>
class IDrawer
{
public:
    virtual void putPixel(int x, int y, T value) = 0;
    virtual ~IDrawer() { }
};

#endif // IDRAWER_H
