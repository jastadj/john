#ifndef CLASS_ITEM
#define CLASS_ITEM

#include "worldobject.hpp"
#include <vector>

// forward dec
class Item;

enum DOOR_STATE{DOOR_VERT_CLOSED, DOOR_VERT_OPEN, DOOR_HORIZ_CLOSED, DOOR_HORIZ_OPEN};

class Door
{
private:

    Item *m_Parent;

    // bit 0 : 0=closed

    char m_DoorChars[4];

    unsigned int m_State;

    std::vector<glyph> m_StateGlyphs;

public:
    Door(Item *tparent);
    Door(const Door &tdoor, Item *tparent);
    ~Door();

    void printInfo() const;
    void open();
    void close();
    void toggle();
    void rotate();

    bool isOpen() const;
    bool isHorizontal() const;

    void setParent(Item *tparent) { m_Parent = tparent;}
    friend class Item;
};

class Item:public WorldObject
{
protected:

    float m_Value;
    float m_Weight;

    Door *m_Door;
public:
    Item();
    Item(const Item &titem);
    ~Item();

    virtual OBJTYPE getType() { return OBJ_ITEM;}

    void setValue(float nval) { m_Value = nval;}
    void setWeight(float nweight) { m_Weight = nweight;}

    float getValue() { return m_Value;}
    float getWeight() { return m_Weight;}

    // door
    void setDoor(Door *tdoor);
    const Door *getDoor();
    bool openDoor();


    virtual void printInfo();
};

#endif // CLASS_ITEM
