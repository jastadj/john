#ifndef CLASS_ACTOR
#define CLASS_ACTOR

#include <vector>

#include "worldobject.hpp"

// forward dec
class Item;

class Actor: public WorldObject
{
private:

    int m_LOSRadius;

    std::vector<Item*> m_Inventory;

public:
    Actor();
    ~Actor();
    OBJTYPE getType() { return OBJ_ACTOR;}

    void setLOSRadious(int nrad) { m_LOSRadius = nrad;}
    int getLOSRadius() const { return m_LOSRadius;}

    bool addItemToInventory(Item *titem);
    std::vector<Item*> *getInventory() { return &m_Inventory;}

    void printInfo() const;
};
#endif // CLASS_ACTOR
