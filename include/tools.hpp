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

class recti
{
public:
    recti();
    recti(int nx, int ny, int nwidth, int nheight);
    ~recti();

    int x;
    int y;
    int width;
    int height;
};

float getDistance(int x1, int y1, int x2, int y2);

char getIndexChar(int i);
int getIndexFromChar(char c);

#endif // CLASS_TOOLS
