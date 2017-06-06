#ifndef CLASS_GLYPH
#define CLASS_GLYPH

#ifdef NCURSES
#include <ncurses.h>
#else
#include "curses.h"
#endif

#include "color.hpp"

#include <tinyxml2.h>

using namespace tinyxml2;

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

    bool loadFromXMLNode(XMLNode *tnode);
};
#endif // CLASS_GLYPH
