#ifndef CLASS_COLOR

#define CLASS_COLOR

#include <tinyxml2.h>

using namespace tinyxml2;

bool initColors();

struct COLOR
{
    COLOR() : m_Foreground(7),
              m_Background(0),
              m_Bold(false)
              {};
    COLOR(int fc, int bc, bool bold) : m_Foreground(fc),
                                       m_Background(bc),
                                       m_Bold(bold)
                                       {};
    int m_Foreground;
    int m_Background;
    bool m_Bold;
};

COLOR loadColorFromXMLNode(XMLNode *tnode);

#endif // CLASS_COLOR
