#ifndef CLASS_MAP
#define CLASS_MAP

#include <string>
#include <vector>

#ifdef NCURSES
#include <ncurses.h>
#else
#include "curses.h"
#endif

#include "tools.hpp"
#include "glyph.hpp"

// forward declaration
class Item;

class Tile
{
public:
    Tile();
    ~Tile();

    std::string m_Name;
    glyph m_Glyph;
};

class Map
{
private:

    std::vector< std::vector<int> > m_Array;
    std::vector< Item*> m_Items;

    bool addItem(Item* nitem);

public:
    Map();
    ~Map();

    // map tiles
    vector2i getDimensions() const;
    void clear();
    void resize(unsigned int x, unsigned int y);
    void fill(unsigned int tileindex);
    int getMapTileIndexAt(unsigned int x, unsigned int y);
    int getMapTileIndexAt(vector2i tpos);
    bool setTileAt(unsigned int x, unsigned int y, int ttile);

    // map objects
    const std::vector<Item*> *getItems() { return &m_Items;}
    std::vector<Item*> getItemsAt(int x, int y);
    Item *removeItemFromMap(Item *titem);

    bool openDoorAt(int x, int y);

    void printInfo() const;

    friend class Engine;
};
#endif // CLASS_MAP
