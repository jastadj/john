#include "console.hpp"
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <stdarg.h>

#include "engine.hpp"
#include "tools.hpp"
#include "actor.hpp"

Console *Console::m_Instance = NULL;

//////////////////////////////////////////////////////////
//

Command::Command(int cmdtype, std::string nname, std::string ndesc, void (*nfunct)(std::vector<std::string> *cmd))
{
    m_Type = cmdtype;

    m_Name = nname;
    m_Description = ndesc;

    m_Function = nfunct;
}

bool Command::addCommand(Command *ncmd)
{
    if(m_Type != C_SUBMENU) return false;

    m_Children.push_back(ncmd);
    return true;
}

bool Command::execute(std::vector<std::string> *cmd) const
{
    if( m_Function != NULL) m_Function(cmd);
    else return false;

    return true;
}

bool Command::hasFunction() const
{
    if(m_Function == NULL) return false;

    return true;
}
//////////////////////////////////////////////////////////
//

Console::Console()
{
    m_PromptString = ">";

    //initialize commands
    initCommands();
}

Console::~Console()
{

}

Console *Console::getInstance()
{
    if(m_Instance == NULL)
    {
        m_Instance = new Console();
    }

    return m_Instance;
}

bool Console::initCommands()
{
    Command *newcmd = new Command(Command::C_CMD, "help", "Print help menu", &printHelp);
    m_CommandList.push_back(newcmd);

    newcmd = new Command(Command::C_SUBMENU, "item", "Item menu", NULL);
        newcmd->addCommand(new Command(Command::C_CMD, "list", "list items", &printItemList));
        newcmd->addCommand(new Command(Command::C_CMD, "show", "show # - show item info (see list)", &showItemInfo) );
        newcmd->addCommand(new Command(Command::C_CMD, "give", "give item # to player", &giveItemToPlayer) );
    m_CommandList.push_back(newcmd);

    newcmd = new Command(Command::C_SUBMENU, "actor", "Actor Menu", NULL);
        newcmd->addCommand(new Command(Command::C_CMD, "list", "list actors", &printActorList));
        newcmd->addCommand(new Command(Command::C_CMD, "show", "show # - show actor info (see list)", &showActorInfo));
    m_CommandList.push_back(newcmd);

    newcmd = new Command(Command::C_SUBMENU, "map", "Map menu", NULL);
		newcmd->addCommand(new Command(Command::C_CMD, "show", "Print map info", &printMap) );
		newcmd->addCommand(new Command(Command::C_CMD, "items", "Print map items", &printMapItems) );
		newcmd->addCommand(new Command(Command::C_CMD, "item", " show item #", &showMapItem));
		newcmd->addCommand(new Command(Command::C_CMD, "actors", "Print map actors", &printMapActors) );
		newcmd->addCommand(new Command(Command::C_CMD, "actor", " show actor #", &showMapActor) );
	m_CommandList.push_back(newcmd);

    newcmd = new Command(Command::C_SUBMENU, "player", "Player menu", NULL);
		newcmd->addCommand(new Command(Command::C_CMD, "show", "Print player info", &printPlayer) );
	m_CommandList.push_back(newcmd);

    newcmd = new Command(Command::C_CMD, "clip", "toggle clipping through walls", &dbgClip);
    m_CommandList.push_back(newcmd);

    newcmd = new Command(Command::C_CMD, "los", "toggle line of sight", &dbgLOS);
    m_CommandList.push_back(newcmd);

    newcmd = new Command(Command::C_CMD, "lighting", "toggle lighting", &dbgLighting);
    m_CommandList.push_back(newcmd);

    newcmd = new Command(Command::C_CMD, "test", "A test", &mytest);
    m_CommandList.push_back(newcmd);

    newcmd = new Command(Command::C_CMD, "colortest", "Show color table", &colortest);
    m_CommandList.push_back(newcmd);

    return true;
}

void Console::openConsole()
{
    bool quit = false;
    std::string command;
    int ch = 0;

    // set curses console environment
    curs_set(1);
    echo();
    scrollok(stdscr, true);

    while(!quit)
    {
        clear();

        printMessages(&m_Buffer);


        // print prompt and active input
        printw("%s%s", m_PromptString.c_str(), command.c_str());

        // get keystroke
        ch = getch();

        // handle key strokes and special keys
        if(ch == 27 || ch == 96) quit = true; // escape
        else if(ch == 8) // backspace
        {
            int csize = int(command.size());

            if(csize > 0) command.resize(csize-1);

        }
        else if (ch == 10) // return
        {

            // add command to buffer
            print(m_PromptString + command);

            // parse command
            parseCommand(command);

            // add carriage return
            //command.push_back('\n');

            // clear command string
            command.erase();
        }
        // if no terminal keys, just add character to command
        else command.push_back(char(ch));
    }
}

void Console::print(std::string str, ...)
{
    va_list v;
    va_start(v, str);
    va_end(v);

    addMessageV(&m_Buffer, str, v);
}


bool Console::parseCommand(std::string tstr)
{
    std::vector<std::string> cmd;
    size_t spos = 0;
    int cmdcount = 0;

    if(tstr.empty()) return false;

    // parse each word separated by spaces
    while(tstr.find_first_of(' ') != std::string::npos)
    {
        spos = tstr.find_first_of(' ');

        std::string newcmd = tstr.substr(0, spos);

        cmd.push_back(newcmd);

        tstr.erase(0, spos+1);
    }

    // if there is content left in the string, add it
    if(!tstr.empty())
    {
        cmd.push_back(tstr);
    }

    // get count of parsed strings
    cmdcount = int(cmd.size());

    // sift through words to find last command
    const Command *tcmd = findCommand(&cmd);
    if(tcmd == NULL) print("Invalid command!  Type 'help'");
    else
    {
        // if the command is a submenu and doesn't have a function, print submenu help
        if(tcmd->getType() == Command::C_SUBMENU && !tcmd->hasFunction())
        {
            printMenuHelp(tcmd);
        }
        // otherwise execute command function
        else tcmd->execute(&cmd);
    }

    return true;
}

const Command *Console::findCommand( std::vector<std::string> *cmd)
{
    const Command *tcmd = NULL;
    const Command *cptr = NULL;
    const std::vector<Command*> *tlist = NULL;

    if(cmd == NULL) return tcmd;
    else if(cmd->empty()) return tcmd;

    int cmdlen = int(cmd->size());

    tlist = getCommands();

    // check command string
    for(int i = 0; i < cmdlen; i++)
    {
        // if no child list to sift through, break
        if(tlist == NULL) break;
        cptr = NULL;

        // check each command in current root list that matches string
        for(int n = 0; n < int(tlist->size()); n++)
        {
            // command found
            if( (*tlist)[n]->getName() == (*cmd)[i])
            {
                cptr = (*tlist)[n];
                tcmd = cptr;
                break;
            }
        }

        // if command was found, set list to found command's children
        if(cptr != NULL)
            tlist = cptr->getChildren();
        else tlist = NULL;
    }

    if(cptr == NULL) return tcmd;

    return cptr;

}

////////////////////////////////////////////////////////////////
//

void printMessages(std::vector<ConsoleElement*> *tlist, recti *trect)
{
    Engine *eptr = Engine::getInstance();

    // offset console printing within this rect
    recti crect(0,0,80,25);

    // if a rect is not provided, assume entire screen
    if(trect != NULL) crect = *trect;

    move(crect.y, crect.x);

    //list starting position
    int i = 0;
    if( int(tlist->size()) > crect.height)
    {
        i = int(tlist->size()) - crect.height;
    }

    // print buffer
    //for(i = 0; i < int(tlist->size()); i++)
    for(i; i < int(tlist->size()); i++)
    {

        //reset colors
        attrset( COLOR_PAIR(eptr->getColorPair(COLOR(COLOR_WHITE, COLOR_BLACK, false))) | A_NORMAL);

        // get local copy of message for printing and formatting
        ConsoleElement tmsg = *(*tlist)[i];
        int argnum = 0;

        // evaluate and print each character in message line
        for(int n = 0; n < tmsg.m_Text.length(); n++)
        {
            // maximum width
            if(getcurx(stdscr) >= crect.width) break;

            // formatter found
            if(tmsg.m_Text[n] == '%')
            {
                n++;

                // bold identifier found
                if(tmsg.m_Text[n] == 'b')
                {
                    n++;
                    attron(A_BOLD);
                }

                // if color identifier found, change color
                // using indexed argument
                if(tmsg.m_Text[n] == 'c')
                {
                    attron( COLOR_PAIR(tmsg.m_Args[argnum]));
                    argnum++;
                }
            }
            // else just print a normal character
            else addch(tmsg.m_Text[n]);
        }

        addch('\n');
    }

}

bool addMessage(std::vector<ConsoleElement*> *tlist, std::string str, ...)
{
    va_list v;
    va_start(v, str);
    va_end(v);
    addMessageV(tlist, str, v);
}

bool addMessageV(std::vector<ConsoleElement*> *tlist, std::string str, va_list v)
{
    // create a console event for text
    ConsoleElement *newelement = new ConsoleElement;
    newelement->m_Text = std::string(str);

    // get argument count
    size_t pos = 0;
    int argcount = 0;

    // find arguments
    while(pos != std::string::npos)
    {
        pos = str.find("%", pos);

        // argument found
        if(pos != std::string::npos)
        {
            newelement->m_Args.push_back(va_arg(v, int));
            argcount++;
            pos++;
        }
    }

    // invalid argument count
    if(argcount != int(newelement->m_Args.size()) )
    {
        delete newelement;
        printw("Invalid argument count! args=%d m_Args size=%d\n", argcount, newelement->m_Args.size());
        refresh();
        exit(2);
        return false;
    }

    // remove any new line codes
    pos = 0;
    while(pos != std::string::npos)
    {
        pos = str.find("\n", pos);

        // newline found
        if(pos != std::string::npos)
        {
            str.erase(pos);
            pos++;
        }
    }

    tlist->push_back(newelement);

    return true;
}

////////////////////////////////////////////////////////////////
//
bool printMenuHelp(const Command *tcmd)
{
    Console *console = Console::getInstance();
    const std::vector<Command*> *cmdlist = NULL;

    Engine *eptr = Engine::getInstance();

    // if target command is null, print main menu help
    if(tcmd == NULL)
    {
        cmdlist = console->getCommands();
    }
    else
    {
        if(tcmd->getType() == Command::C_SUBMENU)
        {
            cmdlist = tcmd->getChildren();
        }
        else return false;
    }

    std::string menutitle;
    if(tcmd == NULL) menutitle = std::string("%cmain menu");
    else menutitle = std::string("%c" + tcmd->getName() + " menu");
    std::string menutitlesub("%c");
    for(int i = 0; i < menutitle.length()-2; i++) menutitlesub.append("-");

    console->print(menutitle, eptr->getColorPair(COLOR(COLOR_MAGENTA, COLOR_BLACK, false)));
    console->print(menutitlesub, eptr->getColorPair(COLOR(COLOR_MAGENTA, COLOR_BLACK, false)));

    for(int i = 0; i < int(cmdlist->size()); i++)
    {
        std::stringstream helpstr;
        int cmdlen = (*cmdlist)[i]->getName().length();
        helpstr << (*cmdlist)[i]->getName();

        for(int j = 0; j < 10-cmdlen; j++) helpstr << ".";

        helpstr << (*cmdlist)[i]->getDesc();

        console->print( helpstr.str());
    }

    return true;
}

void printHelp(std::vector<std::string> *cmd)
{
    printMenuHelp(NULL);
}

void mytest(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();

    console->print("this is a test of function pointer");

}

void itemMenu(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();


}

void printItemList(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();
    Engine *eptr = Engine::getInstance();

    const std::vector<Item*> *ilist = eptr->getItemList();

    for(int i = 0; i < int(ilist->size()); i++)
    {
        std::stringstream msg;
        msg << i << " - " << (*ilist)[i]->getName();
        console->print(msg.str());
    }
}

void printPlayer(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();
    Engine *eptr = Engine::getInstance();

    const Actor *player = eptr->getPlayer();
    player->printInfo();

    std::stringstream ss;
    ss << "Player Move Count:" << eptr->getPlayerMoveCount();
    console->print(ss.str());
}

void showItemInfo(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();
    Engine *eptr = Engine::getInstance();

    int cmdlen = int(cmd->size());
    int itemnum = -1;

    const std::vector<Item*> *ilist = eptr->getItemList();

    // invalid parameters
    if(cmdlen != 3)
    {
        console->print("Invalid parameters!");
        return;
    }

    // item number out of range
    itemnum = atoi( (*cmd)[2].c_str());
    if(itemnum < 0 || itemnum >= int(ilist->size()) )
    {
        console->print("Item #" + (*cmd)[2] + " out of range!");
        return;
    }

    // print item info
    (*ilist)[itemnum]->printInfo();
}

void giveItemToPlayer(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();
    Engine *eptr = Engine::getInstance();

    int cmdlen = int(cmd->size());
    int itemnum = -1;

    const std::vector<Item*> *ilist = eptr->getItemList();

    // invalid parameters
    if(cmdlen != 3)
    {
        console->print("Invalid parameters!");
        return;
    }

    // item number out of range
    itemnum = atoi( (*cmd)[2].c_str());
    if(itemnum < 0 || itemnum >= int(ilist->size()) )
    {
        console->print("Item #" + (*cmd)[2] + " out of range!");
        return;
    }

    // give item to player
    Item *newitem = eptr->newItem(itemnum);
    eptr->getPlayer()->addItemToInventory(newitem);

    std::stringstream ss;
    ss << newitem->getName() << " added to player";
    console->print(ss.str());
}

void printActorList(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();
    Engine *eptr = Engine::getInstance();

    const std::vector<Actor*> *alist = eptr->getActorList();

    for(int i = 0; i < int(alist->size()); i++)
    {
        std::stringstream msg;
        msg << i << " - " << (*alist)[i]->getName();
        console->print(msg.str());
    }
}

void showActorInfo(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();
    Engine *eptr = Engine::getInstance();

    int cmdlen = int(cmd->size());
    int anum = -1;

    const std::vector<Actor*> *alist = eptr->getActorList();

    // invalid parameters
    if(cmdlen != 3)
    {
        console->print("Invalid parameters!");
        return;
    }

    // item number out of range
    anum = atoi( (*cmd)[2].c_str());
    if(anum < 0 || anum >= int(alist->size()) )
    {
        console->print("Actor #" + (*cmd)[2] + " out of range!");
        return;
    }

    // print item info
    (*alist)[anum]->printInfo();
}

void printMap(std::vector<std::string> *cmd)
{
    Engine *eptr = Engine::getInstance();
    eptr->getCurrentMap()->printInfo();
}

void printMapItems(std::vector<std::string> *cmd)
{
    Engine *eptr = Engine::getInstance();
    Console *console = Console::getInstance();

    std::stringstream totitems;

    const Map *tmap = eptr->getCurrentMap();
    if(!tmap) return;
    const std::vector<Item*> *titems = tmap->getItems();

    int icount = int(titems->size());

    console->print("Map Items");
    console->print("---------");

    for(int i = 0; i < icount; i++)
    {
        std::stringstream iss;
        iss << i << ":" << (*titems)[i]->getName();
        console->print( iss.str());
    }

    totitems << "Total Items:" << icount;
    console->print( totitems.str());

}

void showMapItem(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();
    Engine *eptr = Engine::getInstance();

    int cmdlen = int(cmd->size());
    int itemnum = -1;

    const std::vector<Item*> *ilist = eptr->getCurrentMap()->getItems();

    // invalid parameters
    if(cmdlen != 3)
    {
        console->print("Invalid parameters!");
        return;
    }

    // item number out of range
    itemnum = atoi( (*cmd)[2].c_str());
    if(itemnum < 0 || itemnum >= int(ilist->size()) )
    {
        console->print("Item #" + (*cmd)[2] + " out of range!");
        return;
    }

    // print item info
    (*ilist)[itemnum]->printInfo();
}

void printMapActors(std::vector<std::string> *cmd)
{
    Engine *eptr = Engine::getInstance();
    Console *console = Console::getInstance();

    std::stringstream totactors;

    const Map *tmap = eptr->getCurrentMap();
    if(!tmap) return;
    const std::vector<Actor*> *tactors = tmap->getActors();

    int acount = int(tactors->size());

    console->print("Map Actors");
    console->print("----------");

    for(int i = 0; i < acount; i++)
    {
        std::stringstream iss;
        iss << i << ":" << (*tactors)[i]->getName();
        console->print( iss.str());
    }

    totactors << "Total Items:" << acount;
    console->print( totactors.str());

}

void showMapActor(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();
    Engine *eptr = Engine::getInstance();

    int cmdlen = int(cmd->size());
    int actornum = -1;

    const std::vector<Actor*> *alist = eptr->getCurrentMap()->getActors();

    // invalid parameters
    if(cmdlen != 3)
    {
        console->print("Invalid parameters!");
        return;
    }

    // item number out of range
    actornum = atoi( (*cmd)[2].c_str());
    if(actornum < 0 || actornum >= int(alist->size()) )
    {
        console->print("Actor #" + (*cmd)[2] + " out of range!");
        return;
    }

    // print item info
    (*alist)[actornum]->printInfo();
}

void colortest(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();
    Engine *eptr = Engine::getInstance();

    //for(int i = 0; i < MAX_COLORS; i++)
    for(int i = 0; i < 1; i++)
    {
        for(int n = 0; n < MAX_COLORS; n++)
        {
            std::stringstream css;

            COLOR tcolor(n, i, false);
            int colorpair = eptr->getColorPair(tcolor);


            css << "%c" << std::setfill('0') << std::setw(2) << colorpair;

            console->print(css.str(), eptr->getColorPair(COLOR(n, i, false)));
        }
    }
}

void dbgClip(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();
    Engine *teng = Engine::getInstance();

    teng->toggleDebug(DBG_CLIP);

    if(teng->isDebug(DBG_CLIP)) console->print("noclip enabled");
    else console->print("noclip disabled");
}

void dbgLOS(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();
    Engine *teng = Engine::getInstance();

    teng->toggleDebug(DBG_LOS);

    if(teng->isDebug(DBG_LOS)) console->print("LOS disabled");
    else console->print("LOS enabled");
}

void dbgLighting(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();
    Engine *teng = Engine::getInstance();

    teng->toggleDebug(DBG_LIGHT);

    if(teng->isDebug(DBG_LIGHT)) console->print("lighting disabled");
    else console->print("lighting enabled");
}
