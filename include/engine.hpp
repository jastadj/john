#ifndef CLASS_ENGINE
#define CLASS_ENGINE

#include <cstdlib>
#include <vector>
#include <time.h>

#ifdef NCURSES
#include <ncurses.h>
#else
#include "curses.h"
#endif

#include "color.hpp"
#include "map.hpp"
#include "camera.hpp"
#include "console.hpp"
#include "item.hpp"

#include <tinyxml2.h>

#define ENABLE_COLOR 1
#define MAX_COLORS 8


#define TILES_XML ".\\data\\tiles.xml"
#define ITEMS_XML ".\\data\\items.xml"
#define ACTORS_XML ".\\data\\actors.xml"


// namespace
using namespace tinyxml2;

// forward declarations
class Actor;

enum E_DIRECTION{DIR_SW, DIR_S, DIR_SE, DIR_W, DIR_NONE, DIR_E, DIR_NW, DIR_N, DIR_NE};

enum E_DEBUG{DBG_CLIP, DBG_LOS, DBG_LIGHT, DBG_TOTAL};

class Engine
{
private:
    // singleton
    Engine();
    ~Engine();
    static Engine *m_Instance;

    // init
    bool initCurses();
    bool initConsole();
    bool initColors();

    bool initData();
    bool processXML(std::string xfile);

    Camera m_Camera;

    // master game data
    std::vector< std::vector <int> > m_ColorTable;
    std::vector<Tile> m_Tiles;
    std::vector<Item*> m_Items;
    std::vector<Actor*> m_Actors;

    // console
    Console *m_Console;

    // current game data
    void newGame();
    void clearGame();
    time_t m_Seed;
    Actor *m_Player;
    unsigned int m_PlayerMoveCount;
    int m_CurrentLevel;
    std::vector<Map*> m_Levels;
    std::vector<ConsoleElement*> m_MessageLog;

    // main
    void mainLoop();
    void doTurn();
    void setMainLoopEnvironment();

    // draw
    void drawCamera(Camera *tcamera);
    bool inLOS(int x1, int y1, int x2, int y2);
    void drawUI(int x, int y);


    // actor
    bool walkActor(Actor *tactor, int dir, bool noclip=false);
    void printInventory(std::vector<Item*> *ilist);
    void openInventory();
    Item *dropItem();
    bool addActorToMap(Map *tlevel, Actor *tactor, int x, int y);
    Actor *newActor(int aindex);

    // level
    bool generateLevel(Map *tmap);
    bool lightPassesThroughAt(int x, int y, Map *tmap = NULL);
    bool isWalkableAt(int x, int y, Map *tmap = NULL);
    bool openDoorAt(int x, int y, Map *tmap = NULL);

    // items
    bool addItemToMap(Map *tlevel, Item *titem, int x, int y);
    Item *pickupItemFromMapAt(Actor *tactor, Map *tlevel, vector2i tpos);



    // debug options
    std::vector<bool> m_DebugFlags;

public:
    static Engine *getInstance();

    void start();

    // get stuff from main engine
    int getColorPair(COLOR tcolor);
    const Map *getCurrentMap() { return m_Levels[m_CurrentLevel];}
    const std::vector<Item*> *getItemList() { return &m_Items;}
    const std::vector<Actor*> *getActorList() { return &m_Actors;}
    Actor *getPlayer();
    unsigned int getPlayerMoveCount() const { return m_PlayerMoveCount;}

    // actor


    // create item
    Item *newItem(int itmindex);

    // debug
    bool toggleDebug(E_DEBUG dtype);
    bool setDebug(E_DEBUG dtype, bool dstate);
    bool isDebug(E_DEBUG dtype);

    // other
    void exportMapToASCIIFile(const Map *tmap, std::string fname = std::string("mapexport.txt"));

    // give console access

};
#endif // CLASS_ENGINE
