#ifndef CLASS_ITEM
#define CLASS_ITEM

#include "worldobject.hpp"

class Item:public WorldObject
{
private:

    float m_Value;
    float m_Weight;

public:
    Item();
    ~Item();

    OBJTYPE getType() { return OBJ_ITEM;}

    void setValue(float nval) { m_Value = nval;}
    void setWeight(float nweight) { m_Weight = nweight;}

    float getValue() { return m_Value;}
    float getWeight() { return m_Weight;}

    void printInfo();
};
#endif // CLASS_ITEM
