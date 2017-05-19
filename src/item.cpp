#include "item.hpp"
#include "console.hpp"
#include <sstream>

Item::Item()
{
    m_Value = 0;
    m_Weight = 0;

    // null pointers
    m_Door = NULL;
}

Item::Item(const Item &titem) : WorldObject(titem)
{
    *this = titem;

    if(titem.m_Door)
    {
        m_Door = new Door(*titem.m_Door, this);
    }
}

Item::~Item()
{
    if(m_Door) delete m_Door;
}

// door
void Item::setDoor(Door *tdoor)
{
    if(m_Door) delete m_Door;

    m_Door = tdoor;
    m_Door->setParent(this);
}

const Door *Item::getDoor()
{
    return m_Door;
}

bool Item::openDoor()
{
    if(m_Door)
    {
        m_Door->open();
        return m_Door->isOpen();
    }

    return false;
}

void Item::printInfo()
{
	// print parent class
	WorldObject::printInfo();

    Console *console = Console::getInstance();

    std::stringstream ss;
    ss << "Value:" << getValue();
    console->print(ss.str() );
    ss.str(std::string());

    ss << "Weight:" << getWeight();
    console->print(ss.str());
    ss.str(std::string());

    ss << "Components:";
    if(m_Door) ss << "Door,";
    console->print(ss.str());
    ss.str(std::string());
}

/////////////////////////////////////////////////////
// Door
Door::Door(Item *tparent)
{
    m_Parent = tparent;

    m_State = 0;

    m_DoorChars[DOOR_VERT_CLOSED] = '|'; // vert closed
    m_DoorChars[DOOR_VERT_OPEN] = '\\'; // vert open
    m_DoorChars[DOOR_HORIZ_CLOSED] = '-'; // horiz closed
    m_DoorChars[DOOR_HORIZ_OPEN] = '\\'; // horiz open
}

Door::Door(const Door &tdoor, Item *tparent)
{
    m_Parent = tparent;

    m_State = tdoor.m_State;

    for(int i = 0; i < 4; i++)
        m_DoorChars[i] = tdoor.m_DoorChars[i];

}

Door::~Door()
{

}

void Door::open()
{
    m_State |= 0x01;

    if(isHorizontal()) m_Parent->setIcon(m_DoorChars[DOOR_HORIZ_OPEN]);
    else m_Parent->setIcon(m_DoorChars[DOOR_VERT_OPEN]);

    // allow light to pass
    if(m_Parent)
    {
        m_Parent->setPassesLight(true);
    }

}

void Door::close()
{
    m_State ^= 0x01;

    if(isHorizontal()) m_Parent->setIcon(m_DoorChars[DOOR_HORIZ_CLOSED]);
    else m_Parent->setIcon(m_DoorChars[DOOR_VERT_CLOSED]);

    // dont allow light ot pass
    if(m_Parent)
    {
        m_Parent->setPassesLight(false);
    }
}

void Door::toggle()
{
    if(isOpen()) close();
    else open();
}

void Door::rotate()
{
    if( m_State&0x02) m_State ^= 0x02;
    else m_State |= 0x02;

    // send another open/close command to update orientation
    if(isOpen()) open();
    else close();
}

bool Door::isOpen() const
{
    return m_State&0x01;
}

bool Door::isHorizontal() const
{
    return m_State&0x02;
}

void Door::printInfo() const
{

    Console *console = Console::getInstance();

    std::stringstream ss;
    ss << "Door State:" << m_State;
    console->print(ss.str() );
    ss.str(std::string());

}
