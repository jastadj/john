#ifndef CLASS_GLYPH
#define CLASS_GLYPH

#ifdef NCURSES
#include <ncurses.h>
#else
#include "curses.h"
#endif

#include "color.hpp"

class glyph
{
public:
    glyph();
    ~glyph();

    chtype m_Character;
    COLOR m_Color;
    bool m_Walkable;
    bool m_PassesLight;
    bool m_CanPickup;

    void printInfo() const;

    void draw(int x, int y);
};
#endif // CLASS_GLYPH
