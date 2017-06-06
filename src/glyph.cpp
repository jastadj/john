#include "glyph.hpp"
#include "console.hpp" // for debug printinfo
#include "engine.hpp"  // for debug printinfo
#include <sstream> // for debug printinfo

using namespace tinyxml2;

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

bool glyph::loadFromXMLNode(XMLNode *tnode)
{
    XMLNode *anode = NULL;

    anode = tnode->FirstChild();


    while(anode != NULL)
    {
        if(!strcmp(anode->Value(), "character")) m_Character = anode->ToElement()->GetText()[0];
        else if(!strcmp(anode->Value(), "chtype"))
        {
            int chtypenum = 0;
            anode->ToElement()->QueryIntText(&chtypenum);
            m_Character = chtype(chtypenum);
        }
        else if(!strcmp(anode->Value(), "walkable"))
        {
            if( !strcmp(anode->ToElement()->GetText(), "true")) m_Walkable = true;
            else m_Walkable = false;
        }
        else if(!strcmp(anode->Value(), "passesLight"))
        {
            if( !strcmp(anode->ToElement()->GetText(), "true")) m_PassesLight = true;
            else m_PassesLight = false;
        }

        anode = anode->NextSibling();
    }

    return true;
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
