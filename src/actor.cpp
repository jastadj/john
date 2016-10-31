#include "actor.hpp"
#include "item.hpp"

Actor::Actor()
{
    // set default parameters
    m_LOSRadius = 5;
}

Actor::~Actor()
{

}

bool Actor::addItemToInventory(Item *titem)
{
    m_Inventory.push_back(titem);
}
