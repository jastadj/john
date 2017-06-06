#include "map.hpp"
#include <cstdlib>
#include "item.hpp"
#include "actor.hpp"
#include "console.hpp"
#include <sstream>

// debug
#include <iostream>

using namespace tinyxml2;

Tile::Tile()
{
    m_Name = "unnamed";
}

Tile::~Tile()
{

}

bool Tile::loadFromXMLNode(XMLNode *tnode)
{
    XMLNode *anode = NULL;

    bool hasID = false;

    anode = tnode->FirstChild();


    while(anode != NULL)
    {

        if(!strcmp(anode->Value(),"name") ) m_Name = std::string(anode->ToElement()->GetText());
        else if(!strcmp(anode->Value(), "character")) m_Glyph.m_Character = anode->ToElement()->GetText()[0];
        else if(!strcmp(anode->Value(), "chtype"))
        {
            int chtypenum = 0;
            anode->ToElement()->QueryIntText(&chtypenum);
            m_Glyph.m_Character = chtype(chtypenum);
        }
        else if(!strcmp(anode->Value(), "walkable"))
        {
            if( !strcmp(anode->ToElement()->GetText(), "true")) m_Glyph.m_Walkable = true;
            else m_Glyph.m_Walkable = false;
        }
        else if(!strcmp(anode->Value(), "passesLight"))
        {
            if( !strcmp(anode->ToElement()->GetText(), "true")) m_Glyph.m_PassesLight = true;
            else m_Glyph.m_PassesLight = false;
        }
        else if(!strcmp(anode->Value(), "id"))
        {
            hasID = true;
            anode->ToElement()->QueryIntText(&m_ID);
        }

        anode = anode->NextSibling();
    }

    // if no ID is provided, set ID to -1, this will be reassigned to an available ID
    // in processXML
    if(!hasID) m_ID = -1;

    return true;
}

/////////////////////////////////////////////////////////
//
Map::Map()
{

}

Map::~Map()
{
    for(int i = 0; i < int(m_Items.size()); i++) delete m_Items[i];
    m_Items.clear();

    for(int i = 0; i < int(m_Actors.size()); i++) delete m_Actors[i];
    m_Actors.clear();
}

vector2i Map::getDimensions() const
{
    vector2i dims;

    dims.y = int(m_Array.size());
    dims.x = 0;

    if(dims.y != 0)
    {
        dims.x = int(m_Array[0].size());

        // double check to make sure width is the same for each row
        for(int i = 0; i < dims.y; i++)
        {
            if( int(m_Array[i].size()) != dims.x)
            {
                printw("ERROR, map dimension widths not consistent when y = %d", i);
                getch();
                exit(1);
            }
        }
    }

    return dims;
}

void Map::clear()
{

    for(int i = 0; i < int(m_Array.size()); i++)
    {
        for(int n = 0; n < int(m_Array[i].size()); n++)
        {
            m_Array[i][n] = 0;
        }
    }
}

void Map::resize(unsigned int x, unsigned int y)
{
    m_Array.resize( int(y));

    for(int i = 0; i < int(y); i++)
    {
        m_Array[i].resize(int(x));
    }
}

void Map::fill(unsigned int tileindex)
{
    for(int i = 0; i < int(m_Array.size()); i++)
    {
        for(int n = 0; n < int(m_Array[i].size()); n++)
        {
            m_Array[i][n] = tileindex;
        }
    }
}

int Map::getMapTileIndexAt(unsigned int x, unsigned int y)
{
    vector2i dims = getDimensions();

    if( int(x) >= dims.x || int(y) >= dims.y) return -1;

    return m_Array[y][x];
}

int Map::getMapTileIndexAt(vector2i tpos)
{
    return getMapTileIndexAt(tpos.x, tpos.y);
}

bool Map::setTileAt(unsigned int x, unsigned int y, int ttile)
{
    vector2i dims = getDimensions();

    if(int(x) >= dims.x || int(y) >= dims.y) return false;

    m_Array[y][x] = ttile;

    return true;
}



bool Map::addItem(Item* nitem)
{
    if(nitem == NULL) return false;
    m_Items.push_back(nitem);
    return true;
}

bool Map::addActor(Actor *nactor)
{
    if(nactor == NULL) return false;
    m_Actors.push_back(nactor);
    return true;
}

std::vector<Item*> Map::getItemsAt(int x, int y)
{
    std::vector<Item*> ilist;

    for(int i = 0; i < int(m_Items.size()); i++)
    {
        vector2i ipos = m_Items[i]->getPosition();

        if(ipos.x == x && ipos.y == y) ilist.push_back(m_Items[i]);
    }

    return ilist;
}

Item *Map::removeItemFromMap(Item *titem)
{
    if(titem == NULL) return NULL;

    for(int i = 0; i < int(m_Items.size()); i++)
    {
        if(m_Items[i] == titem)
        {
            m_Items.erase( m_Items.begin() + i);

            return titem;
        }
    }

    return NULL;
}

bool Map::openDoorAt(int x, int y)
{
    for(int i = 0; i < int(m_Items.size()); i++)
    {
        if(m_Items[i]->getPosition().x == x && m_Items[i]->getPosition().y == y)
        {
            if(m_Items[i]->getDoor())
            {
                m_Items[i]->openDoor();

                return m_Items[i]->getDoor()->isOpen();
            }
        }
    }

    return false;
}

Actor *Map::getActorAt(int x, int y)
{
    Actor *tactor = NULL;

    for(int i = 0; i < int(m_Actors.size()); i++)
    {
        vector2i ipos = m_Actors[i]->getPosition();

        if(ipos.x == x && ipos.y == y) tactor = m_Actors[i];
    }

    return tactor;
}

Actor *Map::removeActorFromMap(Actor *tactor)
{
    if(tactor == NULL) return NULL;

    for(int i = 0; i < int(m_Actors.size()); i++)
    {
        if(m_Actors[i] == tactor)
        {
            m_Actors.erase( m_Actors.begin() + i);

            return tactor;
        }
    }

    return NULL;
}

void Map::update()
{
    // update map items
    for(int i = 0; i < int(m_Items.size()); i++) m_Items[i]->update();

    // update map actors
    for(int i = int(m_Actors.size())-1; i >= 0 ; i--)
    {
        m_Actors[i]->update();

        // if actor is dead
        if(!m_Actors[i]->isAlive())
        {
            // get actor reference
            Actor *tactor = m_Actors[i];

            // remove actor from map
            removeActorFromMap(m_Actors[i]);

            // delete actor
            delete tactor;
        }
    }
}

void Map::printInfo() const
{
    Console *console = NULL;
    console = Console::getInstance();

    std::stringstream sstr;

    console->print("Map Info");
    console->print("--------");
    sstr << "Dimensions : " << getDimensions().x << "," << getDimensions().y;
    console->print(sstr.str());

    sstr.str(std::string());
    sstr << "Item Count : " << m_Items.size();
    console->print(sstr.str());

}
