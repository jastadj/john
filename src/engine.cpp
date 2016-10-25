#include "engine.hpp"
#include "actor.hpp"
#include <cmath>
#include <sstream>

Engine *Engine::m_Instance = NULL;

Engine::Engine()
{
    m_Player = NULL;
    m_Console = NULL;

    //configure debug
    m_DebugFlags.resize(DBG_TOTAL);
    m_DebugFlags[DBG_CLIP] = false;
    m_DebugFlags[DBG_LOS] = false;
    m_DebugFlags[DBG_LIGHT] = false;

}

Engine::~Engine()
{
    delete m_Player;
}

Engine *Engine::getInstance()
{
    if(m_Instance == NULL)
    {
        m_Instance = new Engine;
    }

    return m_Instance;
}

void Engine::start()
{
    initCurses();
    initConsole();
    initTiles();
    initItems();

    newGame();

    mainLoop();
}

bool Engine::initCurses()
{

    // initialize screen
    initscr();

    // allow extended keys
    keypad(stdscr, TRUE);



    // if color is enabled
    if(ENABLE_COLOR)
    {
        // color init
        start_color();

        // create color pairs in color table
        for(int i = 0; i < MAX_COLORS; i++)
        {
            m_ColorTable.resize( m_ColorTable.size() + 1);

            for(int n = 0; n < MAX_COLORS; n++)
            {
                int colorpair = ( i * MAX_COLORS) + n;

                init_pair(colorpair, n, i);
                m_ColorTable[i].push_back( colorpair);
            }
        }
    }

    return true;
}

// console must initialize after curses init
bool Engine::initConsole()
{
    m_Console = Console::getInstance();
    m_Console->print("Console started.");

    return true;
}

bool Engine::initTiles()
{
    m_Console->print("Loading tiles...");
    // tile 0 = not used, index 0 should be no tile data
    Tile newtile;
    newtile.m_Glyph.character = '!';
    newtile.m_IsWalkable = false;
    newtile.m_Name = "NO TILE!\n";
    m_Tiles.push_back(newtile);

    // tile 1 = wall
    newtile = Tile();
    newtile.m_Glyph.character = chtype(219);
    newtile.m_IsWalkable = false;
    newtile.m_Name = "wall";
    m_Tiles.push_back(newtile);

    // tile 2 = floor
    newtile = Tile();
    newtile.m_Glyph.character = '.';
    newtile.m_IsWalkable = true;
    newtile.m_Name = "floor";
    m_Tiles.push_back(newtile);

    std::stringstream msg;
    msg << m_Tiles.size() << " tiles loaded.";
    m_Console->print(msg.str());

    return true;
}

bool Engine::initItems()
{
    m_Console->print("Loading items...");

    Item *newitem = new Item;
    newitem->setName("rock");
    newitem->setIcon('*');
    newitem->setColors(COLOR_WHITE, COLOR_BLACK, true);
    newitem->setValue(0.2);
    newitem->setWeight(1.0);
    m_Items.push_back(newitem);

    std::stringstream msg;
    msg << m_Items.size() << " items loaded.";
    m_Console->print(msg.str());

    return true;
}

void Engine::clearGame()
{
    m_Console->print("Clearing game data...");

    // clear player data
    if(m_Player != NULL) delete m_Player;
    m_Player = NULL;

    // clear map data
    if(!m_Levels.empty())
    {
        for(int i = 0; i < int(m_Levels.size()); i++)
        {
            delete m_Levels[i];
        }
        m_Levels.clear();
    }


}

void Engine::newGame()
{
    m_Console->print("Starting new game...");

    // clear game data
    clearGame();

    // init seed
    m_Seed = time(NULL);
    srand(m_Seed);

    // init player
    vector2i playerpos(0,0);
    m_Player = new Actor();
    m_Player->setName("player");
    m_Player->setIcon('@');
    m_Player->setPosition(playerpos);

    m_CurrentLevel = 0;

    // init maps
    Map *newmap = new Map();
    newmap->resize(25,25);
    //generateLevel(newmap);
    newmap->fill(2);
    newmap->setTileAt(5,5,1);
    addItemToMap(newmap, newItem(0), 2, 2);
    m_Levels.push_back(newmap);

    // init camera
    m_Camera.setDimensions(80,25);
    m_Camera.setScreenPosition(0,0);
    m_Camera.setWorldPosition(0,0);
    m_Camera.setCenter(playerpos);

}

void Engine::setMainLoopEnvironment()
{
    // disable key echoing
    noecho();

    // remove cursor
    curs_set(0);
}

void Engine::mainLoop()
{
    bool quit = false;
    int ch = 0;

    // set curses environment options for main game
    setMainLoopEnvironment();

    m_Console->print("starting main loop...");

    while(!quit)
    {
        // clear screen
        clear();

        // update
        vector2i playerpos = m_Player->getPosition();
        m_Camera.setCenter(playerpos);

        // draw
        drawCamera(&m_Camera);

        // debug
        setColor(COLOR_RED, COLOR_BLACK, false);
        mvprintw(0,0, "key:%d", ch);
        colorOff();

        // get input
        ch = getch();

        // handle input
        if(ch == 27) quit = true;
        else if( ch == 96) // ~
        {
            // clear colors
            colorOff();

            m_Console->openConsole();

            // set the curses environment back
            setMainLoopEnvironment();
        }
        else if(ch == 49)
        {
            walkActor(m_Player, DIR_SW, m_DebugFlags[DBG_CLIP]);
        }
        else if(ch == KEY_DOWN || ch == 50)
        {
            walkActor(m_Player, DIR_S, m_DebugFlags[DBG_CLIP]);
        }
        else if(ch == 51)
        {
            walkActor(m_Player, DIR_SE, m_DebugFlags[DBG_CLIP]);
        }
        else if(ch == KEY_LEFT || ch == 52)
        {
            walkActor(m_Player, DIR_W, m_DebugFlags[DBG_CLIP]);
        }
        else if(ch == KEY_RIGHT || ch == 54)
        {
            walkActor(m_Player, DIR_E, m_DebugFlags[DBG_CLIP]);
        }
        else if(ch == 55)
        {
            walkActor(m_Player, DIR_NW, m_DebugFlags[DBG_CLIP]);
        }
        else if(ch == KEY_UP || ch == 56)
        {
            walkActor(m_Player, DIR_N, m_DebugFlags[DBG_CLIP]);
        }
        else if(ch == 57)
        {
            walkActor(m_Player, DIR_NE, m_DebugFlags[DBG_CLIP]);
        }
    }
}

void Engine::drawCamera(Camera *tcamera)
{
    if(tcamera == NULL) return;

    // get camera properties
    vector2i cpos = tcamera->getWorldPosition();
    int cwidth = int(tcamera->getWidth());
    int cheight = int(tcamera->getHeight());
    vector2i spos = tcamera->getScreenPosition();

    // get map properties
    Map *tmap = m_Levels[m_CurrentLevel];
    vector2i mapdims = tmap->getDimensions();

    // get player position
    vector2i playerpos = m_Player->getPosition();

    // get player line of sight radius
    int pradius = m_Player->getLOSRadius();

    //draw map
    for(int i = cpos.y; i < cpos.y + cheight; i++)
    {
        for(int n = cpos.x; n < cpos.x + cwidth; n++)
        {
            // position out of bounds?  ignore
            if( n < 0 || n >= mapdims.x || i < 0 || i >= mapdims.y) continue;

            // position within player's line of sight radius?
            if(!m_DebugFlags[DBG_LIGHT])
            {
                if(n < playerpos.x - pradius || n > playerpos.x + pradius ||
                   i < playerpos.y - pradius || i > playerpos.y + pradius) continue;

                if( getDistance(n, i, playerpos.x, playerpos.y) > pradius ) continue;
            }


            // is within player line of sight?
            if(!m_DebugFlags[DBG_LOS])
                if(!inLOS(playerpos.x, playerpos.y, n, i)) continue;


            // get ascii
            int tileindex = tmap->getMapTileIndexAt(vector2i(n,i));
            if(tileindex == 0) continue;
            //chtype ttile = m_Tiles[tileindex].m_Icon;

            vector2i drawpos = tcamera->PositionToScreen( vector2i(n,i));

            // draw
            //mvaddch(drawpos.y, drawpos.x, ttile);
            drawGlyph(m_Tiles[tileindex].m_Glyph, drawpos.x, drawpos.y);

            //draw items
            std::vector<const Item*> ilist = tmap->getItemsAt(n, i);
            for(int k = 0; k < int(ilist.size()); k++)
            {
                glyph tglyph = ilist[k]->getGlyph();
                drawGlyph(tglyph, drawpos.x, drawpos.y);
            }
        }
    }


    // draw player
    if(tcamera->PositionInView(playerpos))
    {
        vector2i playerposscr = tcamera->PositionToScreen(playerpos);
        //mvaddch(playerposscr.y, playerposscr.x, m_Player->getIcon());
        drawGlyph(m_Player->getGlyph(), playerposscr.x, playerposscr.y);
    }


}

bool Engine::inLOS(int x1, int y1, int x2, int y2)
{
    Map *tmap = m_Levels[m_CurrentLevel];
    vector2i mapdims = tmap->getDimensions();

    static const float roundoff = 0.8;

    // if any of these coordinates are outside of current map, invalid
    if(x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 ||
       x1 >= mapdims.x || y1 >= mapdims.y || x2 >= mapdims.x || y2 >= mapdims.y) return false;

    // get slope components
    int rise = y2 - y1;
    int run = x2 - x1;

    // if point 1 and point 2 are the same
    if(rise == 0 && run == 0) return true;

    // make sure point 1 is to the left
    // and swap coordinates if necessary
    if(x1 > x2)
    {
        int tx = x2;
        x2 = x1;
        x1 = tx;

        int ty = y2;
        y2 = y1;
        y1 = ty;
    }

    // horizontal
    if(rise == 0)
    {
        for(int i = x1; i <= x2; i++)
        {
            // ignore origin and destination
            if(i == x2 || i == x1) continue;

            // los is blocked
            if( !m_Tiles[tmap->getMapTileIndexAt(i, y1)].m_IsWalkable) return false;
        }
    }
    else if(run == 0)
    {
        int ylo = 0;
        int yhi = 0;

        // find lowest/highest y coordinate
        if(y1 < y2) {ylo = y1; yhi = y2;}
        else {ylo = y2; yhi = y1;}

        for(int i = ylo; i <= yhi; i++)
        {
            // ignore origin and destination
            if(i == y2 || i == y1) continue;

            // los is blocked
            if( !m_Tiles[tmap->getMapTileIndexAt(x1, i)].m_IsWalkable) return false;
        }
    }
    // not ortho
    else
    {
        // y = mx +b
        // b = y - mx
        float m = float(rise)/float(run);
        float b = float(y1) - m*float(x1);

        // y sweep
        for(int i = x1; i <= x2; i++)
        {
            if(i == x1 || i == x2) continue;

            float ty = m*float(i) + b;
            double num;
            float fract = modf(ty, &num);

            if(fract >= roundoff) ty = round(ty);

            // los is blocked
            int tileindex = tmap->getMapTileIndexAt(i, int(ty));
            if( !m_Tiles[tileindex].m_IsWalkable) return false;
        }

        //x sweep

        // y = mx +b
        // x = (y-b)/m
        int ylo = 0;
        int yhi = 0;

        if(y1 < y2) {ylo = y1; yhi = y2;}
        else {ylo = y2; yhi = y1;}

        for(int i = ylo; i <= yhi; i++)
        {
            if(i == y1 || i == y2) continue;

            float tx = ( float(i)-b) / m;
            double num;
            float fract = modf(tx, &num);

            if(fract >= roundoff) tx = round(tx);

            // los is blocked
            int tileindex = tmap->getMapTileIndexAt( int(tx), i );
            if( !m_Tiles[tileindex].m_IsWalkable) return false;
        }

    }

    return true;
}
void Engine::drawGlyph(glyph tglyph, int x, int y)
{
    setColor(tglyph.foreground, tglyph.background, tglyph.bold);

    mvaddch(y, x, tglyph.character);
}

bool Engine::walkActor(Actor *tactor, int dir, bool noclip)
{
    if(tactor == NULL) return false;

    Map *tmap = m_Levels[m_CurrentLevel];
    vector2i mapdims = tmap->getDimensions();

    // capture starting position
    vector2i spos = tactor->getPosition();
    vector2i npos = spos;

    switch(dir)
    {
    case DIR_SW:
        npos.x--;
        npos.y++;
        break;
    case DIR_S:
        npos.y++;
        break;
    case DIR_SE:
        npos.y++;
        npos.x++;
        break;
    case DIR_W:
        npos.x--;
        break;
    case DIR_NONE:
        break;
    case DIR_E:
        npos.x++;
        break;
    case DIR_NW:
        npos.x--;
        npos.y--;
        break;
    case DIR_N:
        npos.y--;
        break;
    case DIR_NE:
        npos.y--;
        npos.x++;
        break;
    default:
        return false;
        break;
    }

    // verify new position is valid
    // out of bounds
    if(npos.x < 0 || npos.x >= mapdims.x || npos.y < 0 || npos.y >= mapdims.y) return false;



    if(!noclip)
    {
        // tile is unwalkable
        int ttile = tmap->getMapTileIndexAt(npos);

        if(ttile < 0 || ttile >= int(m_Tiles.size()) ) return false;
        if(!m_Tiles[ttile].m_IsWalkable) return false;
    }


    // tile is valid, set actors position
    tactor->setPosition(npos);

    return true;
}

bool Engine::generateLevel(Map *tmap)
{
    if(tmap == NULL) return false;

    // get map dimensions
    vector2i mapdims = tmap->getDimensions();
    long int maparea = mapdims.x * mapdims.y;

    // generation parameters
    bool allowoverlap = true; // allow tiles to be placed over tiles
    bool addwallborder = true;
    int riterations = 0.02 * maparea; // how my times to run through algorithm
    // room sizes
    int rwidth_min = 3;
    int rwidth_max = 6;
    int rheight_min = 3;
    int rheight_max = 6;




    // generate random rooms
    for(int k = 0; k < riterations; k++)
    {
        // get room dimensions
        int rwidth = rand()%(rwidth_max-rwidth_min) + rwidth_min;
        int rheight = rand()%(rheight_max-rheight_min) + rheight_min;

        // get room position
        vector2i rpos;
        rpos.x = rand()%mapdims.x;
        rpos.y = rand()%mapdims.y;

        // check for room placement validity
        bool validpos = true;
        for(int i = rpos.y; i < rpos.y + rheight; i++)
        {
            for(int n = rpos.x; n < rpos.x + rwidth; n++)
            {
                if(n < 0 || n >= mapdims.x || i < 0 || i >= mapdims.y)
                {
                    validpos = false;
                }
                else
                {
                    if(!allowoverlap)
                    {
                        int tindex = tmap->getMapTileIndexAt( unsigned(n), unsigned(i));
                        //Tile *ttile = &m_Tiles[tindex];

                        if(tindex != 0) validpos = false;
                    }
                }

                if(!validpos) break;
            }

            if(!validpos) break;
        }

        if(!validpos) continue;

        // position is valid, populate room
        for(int i = rpos.y; i < rpos.y + rheight; i++)
        {
            for(int n = rpos.x; n < rpos.x + rwidth; n++)
            {
                tmap->setTileAt(n, i, 2);
            }
        }

    }

    // if adding wall borders

    return true;
}

bool Engine::setColor(int foreground, int background, bool bold)
{
    if(!ENABLE_COLOR) return false;

    if(foreground < 0 || foreground >= MAX_COLORS ||
       background < 0 || background >= MAX_COLORS)
        return false;

    if(bold)
    {
        // set color and bold attributes
        attron( COLOR_PAIR( m_ColorTable[background][foreground]) | A_BOLD);
    }
    else
    {
        // set color
        attron( COLOR_PAIR( m_ColorTable[background][foreground]));

        // ensure bold is disabled
        attroff(A_BOLD);
    }

    return true;
}

void Engine::colorOff()
{
    setColor(COLOR_WHITE, COLOR_BLACK, false);
}

Item *Engine::newItem(int itmindex)
{
    if(itmindex < 0 || itmindex >= int(m_Items.size()) ) return NULL;

    Item *newitem = NULL;
    Item *tgtitem = m_Items[itmindex];

    if(tgtitem->getType() == OBJ_ITEM)
    {
        newitem = new Item;
        *newitem = *m_Items[itmindex];
    }

    return newitem;
}

bool Engine::addItemToMap(Map *tlevel, Item *titem, int x, int y)
{
    // level and item valid?
    if(tlevel == NULL) return false;
    if(titem == NULL) return false;

    const std::vector<Item*> *mapitems = tlevel->getItems();

    // destination position in bounds?
    vector2i mapdims = tlevel->getDimensions();
    if(x < 0 || y < 0 || x >= mapdims.x || y >= mapdims.y) return false;

    // set item position
    titem->setPosition(x, y);

    // add item to map
    tlevel->addItem(titem);

    return true;
}

/////////////////////////////////////////////////////////////////
//
bool Engine::toggleDebug(E_DEBUG dtype)
{
    if(dtype < 0 || dtype >= DBG_TOTAL) return false;
    m_DebugFlags[dtype] = !m_DebugFlags[dtype];
    return true;
}

bool Engine::setDebug(E_DEBUG dtype, bool dstate)
{
    if(dtype < 0 || dtype >= DBG_TOTAL) return false;
    m_DebugFlags[dtype] = dstate;
    return true;
}

bool Engine::isDebug(E_DEBUG dtype)
{
    if(dtype < 0 || dtype >= DBG_TOTAL) return false;

    return m_DebugFlags[dtype];
}
