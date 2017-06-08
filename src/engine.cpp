#include "engine.hpp"
#include "actor.hpp"
#include <cmath>
#include <sstream>

#include <iostream>

using namespace tinyxml2;

Engine *Engine::m_Instance = NULL;

Engine::Engine()
{
    m_Player = NULL;
    m_Console = NULL;

    //configure debug
    m_DebugFlags.resize(DBG_TOTAL);
    m_DebugFlags[DBG_CLIP] = false;
    m_DebugFlags[DBG_LOS] = false;
    m_DebugFlags[DBG_LIGHT] = true; // = false;

}

Engine::~Engine()
{
    delete m_Player;

    // shutdown curses
    echo();
    curs_set(1);
    clear();
    endwin();

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
    // init subsystem
    initCurses();
    initConsole();
    if(ENABLE_COLOR) initColors();

    // init data
    initData();
    //initTiles();
    //initItems();
    initActors();

    newGame();

    mainLoop();
}

bool Engine::initCurses()
{
    static bool initialized = false;
    if(initialized) return false;

    // initialize screen
    initscr();

    // allow extended keys
    keypad(stdscr, TRUE);

    initialized = true;
    return true;
}

// console must initialize after curses init
bool Engine::initConsole()
{
    m_Console = Console::getInstance();
    m_Console->print("Console started.");

    return true;
}

bool Engine::initColors()
{
    static bool initialized = false;

    if(initialized) return false;

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

    initialized = true;

    return true;
}

bool Engine::initData()
{
    // load tile data
    if(!processXML(TILES_XML)) return false;

    // load item data
    if(!processXML(ITEMS_XML)) return false;
    // load actor data


    // if no tiles are provided, create default tile
    if(m_Tiles.empty())
    {
        // tile 0 = not used, index 0 should be no tile data
        Tile newtile;
        newtile.m_Glyph.m_Character = '!';
        newtile.m_Glyph.m_Walkable = false;
        newtile.m_Name = "NO TILE!\n";
        m_Tiles.push_back(newtile);

        m_Console->print("No tiles find during init, creating default tile.");
    }

    return true;
}

bool Engine::processXML(std::string xfile)
{
    m_Console->print(std::string("Processing xml file: " + xfile));

    tinyxml2::XMLDocument tdoc;
    if(tdoc.LoadFile(xfile.c_str()))
    {
        std::stringstream ess;
        ess << "Error loading " << xfile;
        m_Console->print(ess.str());
        return false;
    }

    // look for important nodes
    XMLNode *root = tdoc.FirstChild();
    XMLNode *tilesnode = root->FirstChildElement("tiles");
    XMLNode *itemsnode = root->FirstChildElement("items");
    XMLNode *tnode = NULL;

    // diagnostics
    int tilecount = 0;
    int itemcount = 0;
    std::stringstream tss;

    // process tiles first
    if(tilesnode)
    {

        tnode = tilesnode->FirstChildElement("tile");

        while(tnode != NULL)
        {
            tilecount++;

            m_Tiles.push_back(Tile());
            m_Tiles.back().loadFromXMLNode(tnode);

            tnode = tnode->NextSiblingElement("tile");
        }

    }

    // process items second
    if(itemsnode)
    {
        tnode = itemsnode->FirstChildElement("item");

        while(tnode != NULL)
        {
            itemcount++;

            m_Items.push_back(new Item);
            m_Items.back()->loadFromXMLNode(tnode);

            tnode = tnode->NextSiblingElement("item");
        }
    }

    m_Console->print("...done");

    tss << tilecount << " tiles loaded from xml.";
    if(tilecount) m_Console->print(tss.str());
    tss.str() = std::string("");
    if(itemcount) tss << itemcount << " items loaded from xml.";

    return true;
}

bool Engine::initActors()
{
    static bool initialized = false;
    if(initialized) return false;

    m_Console->print("Loading actors...");

    Actor *newactor = NULL;

    newactor = new Actor();
    newactor->setName("rat", "a");
    newactor->setIcon('r');
    newactor->setColors(COLOR_YELLOW, COLOR_BLACK, false);
    m_Actors.push_back(newactor);


    std::stringstream msg;
    msg << m_Actors.size() << " actors loaded.";
    m_Console->print(msg.str());

    initialized = true;
    return true;
}


void Engine::clearGame()
{
    m_Console->print("Clearing game data...");

    // clear player data
    if(m_Player != NULL) delete m_Player;
    m_Player = NULL;

    // clear message log
    for(int i = 0; i < int(m_MessageLog.size()); i++) delete m_MessageLog[i];
    m_MessageLog.clear();

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
    m_Player->setName("player", "");
    m_Player->setIcon('@');
    m_Player->setPosition(playerpos);
    m_PlayerMoveCount = 0;

    m_CurrentLevel = 0;

    // init maps
    Map *newmap = new Map();
    newmap->resize(100,100);
    //generateLevel(newmap);
    newmap->fill(2);
    newmap->setTileAt(5,5,1);
    addItemToMap(newmap, newItem(0), 2, 2);
    addItemToMap(newmap, newItem(1), 4, 4);
    addActorToMap(newmap, newActor(0), 0, 3);
    m_Levels.push_back(newmap);

    // init camera
    m_Camera.setDimensions(40,20);
    m_Camera.setScreenPosition(0,0);
    m_Camera.setWorldPosition(0,0);
    m_Camera.setCenter(playerpos);

    addMessage(&m_MessageLog, "Welcome!");
    addMessage(&m_MessageLog, "this is a test", COLOR(COLOR_RED, COLOR_BLACK, false));
    addMessage(&m_MessageLog, "and another..", COLOR(COLOR_BLUE, COLOR_BLACK, true));
    addMessage(&m_MessageLog, "and another..", COLOR(COLOR_BLUE, COLOR_BLACK, true));
    addMessage(&m_MessageLog, "and another..", COLOR(COLOR_BLUE, COLOR_BLACK, true));
    addMessage(&m_MessageLog, "and the last message", COLOR(COLOR_BLUE, COLOR_BLACK, true));
}

void Engine::setMainLoopEnvironment()
{
    // disable key echoing
    noecho();

    // remove cursor
    curs_set(0);

    // disable screen scrolling
    scrollok(stdscr, false);
}

void Engine::mainLoop()
{
    bool quit = false;
    int ch = 0;

    recti messagelogrect(0, 19, 80, 6);


    // set curses environment options for main game
    setMainLoopEnvironment();

    m_Console->print("starting main loop...");

    while(!quit)
    {
        // clear screen
        clear();
        attrset(A_NORMAL);

        // update
        vector2i playerpos = m_Player->getPosition();
        m_Camera.setCenter(playerpos);

        // draw
        drawCamera(&m_Camera);
        // draw message log
        printMessages(&m_MessageLog, &messagelogrect);
        // draw ui
        drawUI(40, 0 );

        // debug
        mvprintw(0,0, "key:%d", ch);

        // get input
        ch = getch();

        // handle input
        if(ch == 27) quit = true;
        else if( ch == 96) // ~
        {
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
        else if(ch == int('g'))
        {
            Item *titem = pickupItemFromMapAt(m_Player, m_Levels[m_CurrentLevel], m_Player->getPosition());
            if(titem != NULL)
            {
                addMessage(&m_MessageLog, "You pick up " + titem->getArticle() + titem->getName() + ".");

                // update
                doTurn();

            }
        }
        else if(ch == int('i'))
        {
            openInventory();
        }
        else if(ch == int('d'))
        {
            dropItem();
        }
    }
}

void Engine::doTurn()
{
    m_PlayerMoveCount++;

    // update map and all objects on map
    m_Levels[m_CurrentLevel]->update();

    // update player
    m_Player->update();

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

    // get tile count
    int tilecount = int(m_Tiles.size());

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
            if(tileindex < tilecount && tileindex >= 0)
                m_Tiles[tileindex].m_Glyph.draw(drawpos.x, drawpos.y);

            //draw items
            std::vector<Item*> ilist = tmap->getItemsAt(n, i);
            for(int k = 0; k < int(ilist.size()); k++)
            {
                glyph tglyph = ilist[k]->getGlyph();
                tglyph.draw(drawpos.x, drawpos.y);
            }

            // draw actors
            Actor *tactor = tmap->getActorAt(n, i);
            if(tactor != NULL)
            {
                glyph tg = tactor->getGlyph();
                tg.draw(drawpos.x, drawpos.y);
            }
        }
    }


    // draw player
    if(tcamera->PositionInView(playerpos))
    {
        vector2i playerposscr = tcamera->PositionToScreen(playerpos);
        m_Player->getGlyph().draw(playerposscr.x, playerposscr.y);
    }


}

void Engine::drawUI(int x, int y)
{
    std::stringstream uss;

    mvprintw(y+1, x+2, m_Player->getName().c_str());

    uss << "moves:" << m_PlayerMoveCount;
    mvprintw(y+3, x+2, uss.str().c_str());
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
            //if( !m_Tiles[tmap->getMapTileIndexAt(i, y1)].m_Glyph.m_PassesLight) return false;
            if( !lightPassesThroughAt(i, y1)) return false;
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
            //if( !m_Tiles[tmap->getMapTileIndexAt(x1, i)].m_Glyph.m_PassesLight) return false;
            if( !lightPassesThroughAt(x1, i) ) return false;
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
            //int tileindex = tmap->getMapTileIndexAt(i, int(ty));
            //if( !m_Tiles[tileindex].m_Glyph.m_PassesLight) return false;
            if( !lightPassesThroughAt(i, int(ty)) ) return false;;
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
            //int tileindex = tmap->getMapTileIndexAt( int(tx), i );
            //if( !m_Tiles[tileindex].m_Glyph.m_PassesLight) return false;
            if( !lightPassesThroughAt( int(tx), i)) return false;
        }

    }

    return true;
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
        if(!isWalkableAt(npos.x, npos.y))
        {
            // get items at blocked position
            std::vector<Item*> titems = m_Levels[m_CurrentLevel]->getItemsAt( npos.x, npos.y);

            // if an actor is there (mob or player)
            Actor *bactor = tmap->getActorAt(npos.x, npos.y);
            if(!bactor && m_Player->getPosition().x == npos.x && m_Player->getPosition().y == npos.y) bactor = m_Player;

            // if found actor is not current target actor, actor collision (attack)
            if(bactor != tactor && bactor != NULL)
            {
                // combat

                // update
                doTurn();
            }
            // check if colliding with a closed door
            else
            {
                // check each item in list at position
                for(int i = 0; i < int(titems.size()); i++)
                {
                    // if door is found in list
                    if( titems[i]->getDoor())
                    {

                        // attempt to open door
                        if(titems[i]->openDoor())
                        {
                            // if successful, do turn
                            doTurn();
                            return false;
                        }
                    }
                }
            }

            return false;
        }
    }


    // tile is valid, set actors position
    tactor->setPosition(npos);

    // if actor is player, find and print any items at their feet
    if(tactor == m_Player)
    {
        std::vector<Item*> titems = m_Levels[m_CurrentLevel]->getItemsAt( m_Player->getPosition().x, m_Player->getPosition().y);

        if(!titems.empty())
        {

            std::stringstream ifind;
            ifind << "You see ";

            for(int n = 0; n < int(titems.size()); n++)
            {
                // if item has article add a space after
                if(titems[n]->getArticle() != "")
                    ifind << titems[n]->getArticle() << " ";

                // add item name
                ifind << titems[n]->getName();

                // determine separator
                if(n == int(titems.size())-1) ifind << ".";
                else ifind << ",";
            }

            addMessage(&m_MessageLog, ifind.str());

        }

    }

    // update
    doTurn();

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
bool Engine::lightPassesThroughAt(int x, int y, Map *tmap)
{
    if(tmap == NULL) tmap = m_Levels[m_CurrentLevel];

    vector2i dims = tmap->getDimensions();
    Tile *ttile = NULL;
    std::vector<Item*> titems;

    // check x,y validity
    if(int(x) >= dims.x || int(y) >= dims.y) return false;

    // get tile at x,y and check if passes light
    int ti = tmap->getMapTileIndexAt(x,y);
    if(ti >= int(m_Tiles.size()) ) return false;
    ttile = &m_Tiles[ti];
    if(!ttile) return false;
    if( !ttile->m_Glyph.m_PassesLight ) return false;

    // get items at x,y and check if passes light
    titems = tmap->getItemsAt(x, y);
    for(int i = 0; i < int(titems.size()); i++)
    {
        if( !titems[i]->getGlyph().m_PassesLight) return false;
    }



    return true;
}

bool Engine::isWalkableAt(int x, int y, Map *tmap)
{
    if(tmap == NULL) tmap = m_Levels[m_CurrentLevel];

    vector2i dims = tmap->getDimensions();
    Tile *ttile = NULL;
    std::vector<Item*> titems;

    // check x,y validity
    if(int(x) >= dims.x || int(y) >= dims.y) return false;

    // get tile at x,y and check if passes light
    ttile = &m_Tiles[tmap->getMapTileIndexAt(x,y)];
    if(!ttile) return false;
    if( !ttile->m_Glyph.m_Walkable ) return false;

    // get items at x,y and check if passes light
    titems = tmap->getItemsAt(x, y);
    for(int i = 0; i < int(titems.size()); i++)
    {
        if( !titems[i]->getGlyph().m_Walkable) return false;
    }

    // check if an actor occupies that space
    if(tmap->getActorAt(x, y)) return false;
    vector2i ppos = m_Player->getPosition();
    if(ppos.x == x && ppos.y == y) return false;

    return true;
}

bool Engine::openDoorAt(int x, int y, Map *tmap)
{
    if(!tmap) tmap = m_Levels[m_CurrentLevel];

    return tmap->openDoorAt(x, y);
}

Item *Engine::newItem(int itmindex)
{
    if(itmindex < 0 || itmindex >= int(m_Items.size()) ) return NULL;

    Item *newitem = NULL;
    Item *tgtitem = m_Items[itmindex];

    if(tgtitem->getType() == OBJ_ITEM)
    {
        newitem = new Item(*tgtitem);
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

Item *Engine::pickupItemFromMapAt(Actor *tactor, Map *tlevel, vector2i tpos)
{
    if(tactor == NULL || tlevel == NULL) return NULL;

    // get list of items at target position
    std::vector<Item*> ilist = tlevel->getItemsAt(tpos.x, tpos.y);

    // get first item in the list?
    for(int i = 0; i < int(ilist.size()); i++)
    {
        if(ilist[i]->canPickup())
        {
            Item *titem = tlevel->removeItemFromMap(ilist[i]);
            tactor->addItemToInventory(titem);

            return titem;
        }

    }

    return NULL;
}

Actor *Engine::getPlayer()
{
	return m_Player;
}

void Engine::printInventory(std::vector<Item*> *ilist)
{
    if(ilist == NULL) return;

    if(ilist->empty())
    {
        printw("You are not carring anything!\n");
    }
    else
    {
        printw("Inventory\n");
        printw("---------\n");

        for(int i = 0; i < int(ilist->size()); i++)
        {
            printw("%c - %s", getIndexChar(i), (*ilist)[i]->getName().c_str());
        }
    }
}

void Engine::openInventory()
{
    bool doquit = false;
    int ch = 0;

    std::vector<Item*> *inventory = m_Player->getInventory();

    while(!doquit)
    {
        clear();

        printInventory(inventory);

        ch = getch();

        if(ch == 27) doquit = true; // escape
        else if(ch == 10) doquit = true; // enter

    }
}

Item *Engine::dropItem()
{
    bool doquit = false;
    int ch = 0;

    std::vector<Item*> *inventory = m_Player->getInventory();

    while(!doquit)
    {
        clear();

        printInventory(inventory);

        if(inventory->empty())
        {
            getch();
            return NULL;
        }

        mvprintw(24, 0, "Drop what?");

        ch = getch();

        if(ch == 27) doquit = true; // escape
        else if(ch == 10) doquit = true; // enter
        else
        {
            int iindex = getIndexFromChar(ch);
            // valid item selected
            if(iindex >=0 && iindex < int(inventory->size()))
            {
                // player position
                vector2i ppos = m_Player->getPosition();

                // get and remove item from inventory
                Item *titem = (*inventory)[iindex];
                inventory->erase(inventory->begin() + iindex);

                // add item to map
                addItemToMap(m_Levels[m_CurrentLevel], titem, ppos.x, ppos.y);

                // update
                doTurn();

                return titem;
            }
        }
    }

    return NULL;
}

Actor *Engine::newActor(int aindex)
{
    if(aindex < 0 || aindex >= int(m_Actors.size()) ) return NULL;

    Actor *newactor = NULL;
    Actor *tgtactor = m_Actors[aindex];

    if(tgtactor->getType() == OBJ_ACTOR)
    {
        newactor = new Actor(*tgtactor);
    }

    return newactor;
}

bool Engine::addActorToMap(Map *tlevel, Actor *tactor, int x, int y)
{
    // level and item valid?
    if(tlevel == NULL) return false;
    if(tactor == NULL) return false;

    const std::vector<Actor*> *mapactors = tlevel->getActors();

    // destination position in bounds?
    vector2i mapdims = tlevel->getDimensions();
    if(x < 0 || y < 0 || x >= mapdims.x || y >= mapdims.y) return false;

    // set item position
    tactor->setPosition(x, y);

    // add item to map
    tlevel->addActor(tactor);

    return true;
}
/////////////////////////////////////////////////////////////////
//

int Engine::getColorPair(COLOR tcolor)
{
    if(tcolor.m_Foreground < 0 || tcolor.m_Background < 0 ||
       tcolor.m_Foreground >= MAX_COLORS || tcolor.m_Background >= MAX_COLORS)
        return 0;
    else return m_ColorTable[tcolor.m_Background][tcolor.m_Foreground];
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
