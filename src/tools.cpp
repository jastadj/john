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

float getDistance(int x1, int y1, int x2, int y2)
{
    return float(sqrt( ((x2-x1)*(x2-x1)) + ((y2-y1)*(y2-y1)) ) ) + 0.5;
}

