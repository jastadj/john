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
}

glyph::~glyph()
{

}

void glyph::draw(int x, int y)
{
    Engine *eptr = Engine::getInstance();
    int cpair = eptr->getColorPair( m_Color);
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

}
