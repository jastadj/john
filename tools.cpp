#include "tools.hpp"
#include <cmath>

vector2i::vector2i()
{
    x = 0;
    y = 0;
}

vector2i::vector2i(int nx, int ny)
{
    x = nx;
    y = ny;
}

vector2i::~vector2i()
{

}

recti::recti()
{
    x = 0;
    y = 0;
    width = 0;
    height = 0;
}

recti::recti(int nx, int ny, int nwidth, int nheight)
{
    x = nx;
    y = ny;
    width = nwidth;
    height = nheight;
}

recti::~recti()
{

}

float getDistance(int x1, int y1, int x2, int y2)
{
    return float(sqrt( ((x2-x1)*(x2-x1)) + ((y2-y1)*(y2-y1)) ) ) + 0.5;
}

char getIndexChar(int i)
{
    const int lowerbase = int('a');
    const int upperbase = int('A');
    const int numbase = int('0');
    // lower case alphabet
    if(i >= 0 && i <=25) return char(i + lowerbase);
    else if(i >= 26 && i <= 51) return char(i + upperbase);
    else if(i >= 52 && i <= 61) return char(i + numbase);
    else return '?';
}

int getIndexFromChar(char c)
{
    const int lowerbase = int('a');
    const int upperbase = int('A');
    const int numbase = int('0');
    // lower case alphabet
    if( int(c) >= lowerbase && int(c) < lowerbase+26) return int(c) - lowerbase;
    else if( int(c) >= upperbase && int(c) < upperbase+26) return int(c) - upperbase + 26;
    else if( int(c) >= numbase && int(c) < numbase+9) return int(c) - numbase + (26*2);
    else return -1;
}
