#ifndef CLASS_MAP
#define CLASS_MAP

#include <string>
#include <vector>

#include "curses.h"
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
    bool m_IsWalkable;
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
    vector2i getDimensions();
    void clear();
    void resize(unsigned int x, unsigned int y);
    void fill(unsigned int tileindex);
    int getMapTileIndexAt(unsigned int x, unsigned int y);
    int getMapTileIndexAt(vector2i tpos);
    bool setTileAt(unsigned int x, unsigned int y, int ttile);

    // map objects
    const std::vector<Item*> *getItems() const { return &m_Items;}
    std::vector<const Item*> getItemsAt(int x, int y);

    friend class Engine;
};
#endif // CLASS_MAP
