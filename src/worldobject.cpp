#include "worldobject.hpp"
#include "console.hpp"
#include <sstream>

WorldObject::WorldObject()
{
    m_Name = "unnamed";

}

WorldObject::~WorldObject()
{

}

void WorldObject::setPosition(vector2i npos)
{
    m_Position = npos;
}

void WorldObject::setPosition(int nx, int ny)
{
    setPosition(vector2i(nx, ny));
}

void WorldObject::setColors(int foreground, int background, bool bold)
{
    COLOR tcolor(foreground, background, bold);
    m_Glyph.m_Color = tcolor;
}

void WorldObject::printInfo()
{
    Console *console = Console::getInstance();
    console->print("");
    console->print("Name:" + m_Name);

    std::stringstream ss;
    ss << "Position:" << m_Position.x << "," << m_Position.y;
    console->print(ss.str() );
    ss.str(std::string());

    // print glyph info
    m_Glyph.printInfo();
}
