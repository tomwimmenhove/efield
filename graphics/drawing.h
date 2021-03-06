#ifndef DRAWING_H
#define DRAWING_H

#include <QPoint>
#include "idrawer.h"

template <typename T>
class Drawing
{
public:
    Drawing(IDrawer<T>& drawer)
     : drawer(drawer)
    { }

    inline void drawLine(const QPoint& p1, const QPoint& p2, const T& value) { drawLine(p1.x(), p1.y(), p2.x(), p2.y(), value); }

    // XXX: Fuck this motherfucker.
    void drawLine(int x1, int y1, int x2, int y2, const T& value)
    {
        int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;
        dx=x2-x1;
        dy=y2-y1;
        dx1=dx > 0 ? dx : -dx;
        dy1=dy > 0 ? dy : -dy;
        px=2*dy1-dx1;
        py=2*dx1-dy1;
        if(dy1<=dx1)
        {
            if(dx>=0)
            {
                x=x1;
                y=y1;
                xe=x2;
            }
            else
            {
                x=x2;
                y=y2;
                xe=x1;
            }
            drawer.putPixel(x, y, value);
            for(i=0;x<xe;i++)
            {
                x=x+1;
                if(px<0)
                {
                    px=px+2*dy1;
                }
                else
                {
                    if((dx<0 && dy<0) || (dx>0 && dy>0))
                    {
                        y=y+1;
                    }
                    else
                    {
                        y=y-1;
                    }
                    px=px+2*(dy1-dx1);
                }
                drawer.putPixel(x, y, value);
            }
        }
        else
        {
            if(dy>=0)
            {
                x=x1;
                y=y1;
                ye=y2;
            }
            else
            {
                x=x2;
                y=y2;
                ye=y1;
            }
            drawer.putPixel(x, y, value);
            for(i=0;y<ye;i++)
            {
                y=y+1;
                if(py<=0)
                {
                    py=py+2*dx1;
                }
                else
                {
                    if((dx<0 && dy<0) || (dx>0 && dy>0))
                    {
                        x=x+1;
                    }
                    else
                    {
                        x=x-1;
                    }
                    py=py+2*(dx1-dy1);
                }
                drawer.putPixel(x, y, value);
            }
        }
    }

    inline void drawCircle(const QPoint& p, int radius, const T& value) { drawCircle(p.x(), p.y(), radius, value); }

    void drawCircle(int x1, int y1, int r, const T& value)
    {
        int x, y, p;
        x = 0;
        y = r;
        p = 3 - (2 * r);
        while(x <= y)
        {
            drawer.putPixel(x1 + x, y1 + y, value);
            drawer.putPixel(x1 - x, y1 + y, value);
            drawer.putPixel(x1 + x, y1 - y, value);
            drawer.putPixel(x1 - x, y1 - y, value);
            drawer.putPixel(x1 + y, y1 + x, value);
            drawer.putPixel(x1 + y, y1 - x, value);
            drawer.putPixel(x1 - y, y1 + x, value);
            drawer.putPixel(x1 - y, y1 - x, value);
            x = x + 1;
            if(p < 0)
            {
                p = p + 4 * x + 6;
            }
            else
            {
                p = p + 4 * (x - y) + 10;
                y = y - 1;
            }
        }
    }

private:
    IDrawer<T>& drawer;
};

#endif // DRAWING_H
