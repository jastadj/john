#include "map.hpp"
#include <cstdlib>
#include "item.hpp"
#include "console.hpp"
#include <sstream>

Tile::Tile()
{
    m_Name = "unnamed";
}

Tile::~Tile()
{

}


/////////////////////////////////////////////////////////
//
Map::Map()
{

}

Map::~Map()
{

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
