#ifndef CLASS_TOOLS
#define CLASS_TOOLS

class vector2i
{
public:
    vector2i();
    vector2i(int nx, int ny);
    ~vector2i();

    int x;
    int y;
};

float getDistance(int x1, int y1, int x2, int y2);

#endif // CLASS_TOOLS
