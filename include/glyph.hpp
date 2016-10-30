#ifndef CLASS_GLYPH
#define CLASS_GLYPH

#include "curses.h"
#include "color.hpp"

class glyph
{
public:
    glyph();
    ~glyph();

    chtype m_Character;
    COLOR m_Color;

    void printInfo();

    void draw(int x, int y);
};
#endif // CLASS_GLYPH
