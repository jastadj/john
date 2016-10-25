#ifndef CLASS_ACTOR
#define CLASS_ACTOR

#include "worldobject.hpp"

class Actor: public WorldObject
{
private:

    int m_LOSRadius;

public:
    Actor();
    ~Actor();
    OBJTYPE getType() { return OBJ_ACTOR;}

    void setLOSRadious(int nrad) { m_LOSRadius = nrad;}
    int getLOSRadius(){ return m_LOSRadius;}
};
#endif // CLASS_ACTOR
