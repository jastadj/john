#ifndef CLASS_WORLDOBJECT
#define CLASS_WORLDOBJECT

#ifdef NCURSES
#include <ncurses.h>
#else
#include "curses.h"
#endif

#include <string>

#include "tools.hpp"
#include "glyph.hpp"

enum OBJTYPE{ OBJ_ACTOR, OBJ_ITEM, OBJ_TOTAL};

class WorldObject
{
protected:
    std::string m_Name;
    std::string m_Article;
    vector2i m_Position;

    glyph m_Glyph;

public:
    WorldObject();
    WorldObject(const WorldObject &tobj);
    virtual ~WorldObject();
    virtual OBJTYPE getType()=0;

    std::string getName() const { return m_Name;}
    std::string getArticle() const { return m_Article;}
    chtype getIcon() { return m_Glyph.m_Character;}
    vector2i getPosition() { return m_Position;}
    int getColorForeground() { return m_Glyph.m_Color.m_Foreground;}
    int getColorBackground() { return m_Glyph.m_Color.m_Background;}
    glyph getGlyph() const { return m_Glyph;}
    bool isWalkable() { return m_Glyph.m_Walkable;}
    bool passesLight() { return m_Glyph.m_PassesLight;}

    void setName(std::string nname, std::string narticle);
    void setIcon(chtype nicon) {m_Glyph.m_Character = nicon;}
    void setPosition(int nx, int ny);
    void setPosition(vector2i npos);
    void setColors(int foreground, int background, bool bold);
    void setWalkable(bool nwalkable) { m_Glyph.m_Walkable = nwalkable;}
    void setPassesLight(bool nplight) { m_Glyph.m_PassesLight = nplight;}

    virtual void printInfo() const;

};

#endif // CLASS_WORLDOBJECT
