#include "item.hpp"
#include "console.hpp"
#include <sstream>

Item::Item()
{
    m_Value = 0;
    m_Weight = 0;
}

Item::~Item()
{

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

}
