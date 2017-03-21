#include "actor.hpp"
#include "item.hpp"
#include "console.hpp"
#include <sstream>

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

void Actor::printInfo() const
{
	// print parent class
	WorldObject::printInfo();

    Console *console = Console::getInstance();

    std::stringstream ss;
    ss << "LOS Radius:" << getLOSRadius();
    console->print(ss.str() );
    ss.str(std::string());

    ss << "Inventory:" << m_Inventory.size() << " items.";
    console->print(ss.str());
    ss.str(std::string());

}
