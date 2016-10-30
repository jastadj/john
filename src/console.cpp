#include "console.hpp"
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "engine.hpp"
#include "tools.hpp"

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

    Engine *eptr = Engine::getInstance();

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

void Console::print(std::string str, COLOR tcolor, std::string textend)
{
    addMessage(&m_Buffer, str, tcolor, textend);
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

    if(trect != NULL) crect = *trect;

    move(crect.y, crect.x);

    // print buffer
    for(int i = 0; i < int(tlist->size()); i++)
    {
        // get local copy of message for printing and formatting
        ConsoleElement tmsg = *(*tlist)[i];

        // configure message color
        chtype attr = A_NORMAL;
        if(tmsg.m_Color.m_Bold) attr = A_BOLD;;
        attr = attr | COLOR_PAIR( eptr->getColorPair(tmsg.m_Color) );

        // get dimensions of text to see if it fits within rect
        int xpos = getcurx(stdscr);
        int ypos = getcury(stdscr);
        int maxwidth = crect.width - xpos;
        int textlen = tmsg.m_Text.length();

        // if text doesn't fit within length, trim off the end
        if(textlen + xpos >= maxwidth)
        {
            int clip = xpos + textlen - maxwidth;
            tmsg.m_Text.erase( tmsg.m_Text.end()-clip, tmsg.m_Text.end());
        }

        attron( attr);
        printw("%s%s", tmsg.m_Text.c_str(), tmsg.m_TextEnd.c_str());
        attroff(attr);
    }

}

void addMessage(std::vector<ConsoleElement*> *tlist, std::string str, COLOR tcolor, std::string textend)
{
    // create a console event for text
    ConsoleElement *newelement = new ConsoleElement;
    newelement->m_Text = std::string(str);
    newelement->m_TextEnd = textend;
    newelement->m_Color = tcolor;

    tlist->push_back(newelement);
}

////////////////////////////////////////////////////////////////
//
bool printMenuHelp(const Command *tcmd)
{
    Console *console = Console::getInstance();
    const std::vector<Command*> *cmdlist = NULL;

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
    if(tcmd == NULL) menutitle = std::string("main menu");
    else menutitle = std::string(tcmd->getName() + " menu");
    std::string menutitlesub;
    for(int i = 0; i < menutitle.length(); i++) menutitlesub.append("-");

    console->print(menutitle, COLOR(COLOR_MAGENTA, COLOR_BLACK, true));
    console->print(menutitlesub, COLOR(COLOR_MAGENTA, COLOR_BLACK, false));

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

void colortest(std::vector<std::string> *cmd)
{
    Console *console = Console::getInstance();
    Engine *eptr = Engine::getInstance();

    for(int i = 0; i < MAX_COLORS; i++)
    {
        for(int n = 0; n < MAX_COLORS; n++)
        {
            COLOR tcolor(n, i, false);
            int colorpair = eptr->getColorPair(tcolor);

            std::stringstream css;
            css << std::setfill('0') << std::setw(2) << colorpair;

            console->print(css.str(), COLOR(n, i, false), "");
        }
        console->print("");
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
