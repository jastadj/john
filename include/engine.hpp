#ifndef CLASS_ENGINE
#define CLASS_ENGINE

#include <cstdlib>
#include <vector>
#include <time.h>
#include "curses.h"

#include "map.hpp"
#include "camera.hpp"
#include "console.hpp"
#include "item.hpp"

#define ENABLE_COLOR 1
#define MAX_COLORS 8

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
    bool initTiles();
    bool initItems();

    Camera m_Camera;

    // master game data
    std::vector< std::vector <int> > m_ColorTable;
    std::vector<Tile> m_Tiles;
    std::vector<Item*> m_Items;

    // console
    Console *m_Console;

    // current game data
    void newGame();
    void clearGame();
    time_t m_Seed;
    Actor *m_Player;
    int m_CurrentLevel;
    std::vector<Map*> m_Levels;

    // main
    void mainLoop();
    void setMainLoopEnvironment();

    // draw
    void drawCamera(Camera *tcamera);
    bool inLOS(int x1, int y1, int x2, int y2);

    // actor
    bool walkActor(Actor *tactor, int dir, bool noclip=false);

    // level generation
    bool generateLevel(Map *tmap);

    // items
    Item *newItem(int itmindex);
    bool addItemToMap(Map *tlevel, Item *titem, int x, int y);

    // debug options
    std::vector<bool> m_DebugFlags;

public:
    static Engine *getInstance();

    void start();

    bool setColor(int foreground, int background, bool bold);
    void colorOff();

    void drawGlyph(glyph tglyph, int x, int y);

    const std::vector<Item*> *getItemList() { return &m_Items;}

    // debug
    bool toggleDebug(E_DEBUG dtype);
    bool setDebug(E_DEBUG dtype, bool dstate);
    bool isDebug(E_DEBUG dtype);

};
#endif // CLASS_ENGINE
