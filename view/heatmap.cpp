#include <tgmath.h>

#include "view/heatmap.h"

QRgb HeatMap::getColor(float value, int steps)
{
    if (steps)
    {
        /* Turn into 20 'steps' */
        value *= (float) steps;
        value = round(value);
        value /= (float) steps;
    }

    const int NUM_COLORS = 4;
    static float color[NUM_COLORS][3] = { {0,0,255}, {0,255,0}, {255,255,0}, {255,0,0} };
    // A static array of 4 colors:  (blue,   green,  yellow,  red) using {r,g,b} for each.

    int idx1;        // |-- Our desired color will be between these two indexes in "color".
    int idx2;        // |
    float fractBetween = 0;  // Fraction between "idx1" and "idx2" where our value is.

    if(value <= 0)      {  idx1 = idx2 = 0;            }    // accounts for an input <=0
    else if(value >= 1)  {  idx1 = idx2 = NUM_COLORS-1; }    // accounts for an input >=0
    else
    {
        value = value * (NUM_COLORS-1);        // Will multiply value by 3.
        idx1  = value;                         // Our desired color will be after this index.
        idx2  = idx1+1;                        // ... and before this index (inclusive).
        fractBetween = value - float(idx1);    // Distance between the two indexes (0-1).
    }

    float red   = (color[idx2][0] - color[idx1][0])*fractBetween + color[idx1][0];
    float green = (color[idx2][1] - color[idx1][1])*fractBetween + color[idx1][1];
    float blue  = (color[idx2][2] - color[idx1][2])*fractBetween + color[idx1][2];

    return 0xff000000 | ((uint8_t) red) << 16 | ((uint8_t) green) << 8 | ((uint8_t) blue);
}
