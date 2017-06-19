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

#include <tinyxml2.h>

using namespace tinyxml2;

// forward declaration
class Item;
class Actor;

class Tile
{
public:
    Tile();
    ~Tile();

    std::string m_Name;
    glyph m_Glyph;
    int m_ID;

    bool loadFromXMLNode(XMLNode *tnode);
};

class Map
{
private:

    std::vector< std::vector<int> > m_Array;
    std::vector< Item*> m_Items;
    std::vector< Actor*> m_Actors;

    bool addItem(Item* nitem);
    bool addActor(Actor *nactor);

public:
    Map();
    ~Map();

    // map tiles
    vector2i getDimensions() const;
    void clear();
    void resize(unsigned int x, unsigned int y);
    void fill(unsigned int tileindex);
    int getMapTileIndexAt(unsigned int x, unsigned int y) const;
    int getMapTileIndexAt(vector2i tpos) const;
    bool setTileAt(unsigned int x, unsigned int y, int ttile);

    // map objects
    // map items
    const std::vector<Item*> *getItems() const { return &m_Items;}
    std::vector<Item*> getItemsAt(int x, int y);
    Item *removeItemFromMap(Item *titem);
    bool openDoorAt(int x, int y);

    // map actors
    const std::vector<Actor*> *getActors() const { return &m_Actors;}
    Actor *getActorAt(int x, int y);
    Actor *removeActorFromMap(Actor *tactor);

    void update();

    void printInfo() const;

    friend class Engine;
};
#endif // CLASS_MAP
