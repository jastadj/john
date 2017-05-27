#include "glyph.hpp"
#include "console.hpp" // for debug printinfo
#include "engine.hpp"  // for debug printinfo
#include <sstream> // for debug printinfo

glyph::glyph()
{
    m_Character = '?';
    m_Color = COLOR();
    m_Walkable = true;
    m_PassesLight = true;
    m_CanPickup = false;
}

glyph::~glyph()
{

}

void glyph::draw(int x, int y)
{
    Engine *eptr = Engine::getInstance();
    int cpair = eptr->getColorPair( m_Color);


    //reset colors
    attrset( COLOR_PAIR(eptr->getColorPair(COLOR(COLOR_WHITE, COLOR_BLACK, false))) | A_NORMAL);

    // if glyph is bold
    if(m_Color.m_Bold) attrset( A_BOLD);

    // set color
    attron( COLOR_PAIR(eptr->getColorPair(m_Color)) );

    // draw glyph
    mvaddch(y, x, m_Character);
}

void glyph::printInfo() const
{
    Console *console = Console::getInstance();
    Engine *eptr = Engine::getInstance();

    std::stringstream ss;
    ss << "Glyph Char:" << char(m_Character);
    console->print(ss.str());
    ss.str(std::string());
    ss << "Walkable:" << m_Walkable;
    console->print(ss.str());
    ss.str(std::string());
    ss << "Passes Light:" << m_PassesLight;
    console->print(ss.str());
    ss.str(std::string());
    ss << "Can Pickup:" << m_CanPickup;
    console->print(ss.str());

}
