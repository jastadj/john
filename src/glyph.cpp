#include "glyph.hpp"
#include "console.hpp" // for debug printinfo
#include "engine.hpp"  // for debug printinfo
#include <sstream> // for debug printinfo

glyph::glyph()
{
    character = '?';
    bold = false;
    foreground = COLOR_WHITE;
    background = COLOR_BLACK;
}

glyph::~glyph()
{

}

void glyph::printInfo()
{
    Console *console = Console::getInstance();
    Engine *eptr = Engine::getInstance();

    console->setColor(foreground, background, bold);
    std::stringstream ss;
    ss << "Glyph Char:" << char(character);
    console->clearColor();
    console->print(ss.str());
    ss.str(std::string());
    eptr->colorOff();


/*

    ss << "Position:" << m_Position.x << "," << m_Position.y;
    console->print(ss.str() );

    ss.str(std::string());
    */
}
