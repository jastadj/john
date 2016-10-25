#ifndef CLASS_GLYPH
#define CLASS_GLYPH

#include "curses.h"

class glyph
{
public:
    glyph();
    ~glyph();

    chtype character;
    bool bold;
    int foreground;
    int background;

    void printInfo();
};
#endif // CLASS_GLYPH
