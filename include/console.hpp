#ifndef CLASS_CONSOLE
#define CLASS_CONSOLE

#ifdef NCURSES
#include <ncurses.h>
#else
#include "curses.h"
#endif

#include <string>
#include <vector>
#include "color.hpp"

// forward decl
class recti;

class Command
{
private:
    std::string m_Name;
    std::string m_Description;
    int m_Type;

    void (*m_Function)(std::vector<std::string> *cmd);

    std::vector<Command*> m_Children;
public:
    Command(int cmdtype, std::string nname, std::string ndesc, void (*nfunct)(std::vector<std::string> *cmd) = NULL);
    ~Command();

    // command type, is it a normal command or a submenu type command
    enum {C_CMD, C_SUBMENU};

    std::string getName() const { return m_Name;}
    std::string getDesc() const { return m_Description;}
    int getType() const {return m_Type;}

    bool addCommand(Command *ncommand);
    const std::vector< Command*> *getChildren() const { return &m_Children;}
    bool execute(std::vector<std::string> *cmd) const;
    bool hasFunction() const;
};

struct ConsoleElement
{
    std::string m_Text;
    std::vector<int> m_Args;
};

class Console
{
private:
    // singleton
    Console();
    ~Console();
    static Console *m_Instance;

    bool initCommands();
    std::vector<Command*> m_CommandList;

    std::string m_PromptString;
    std::vector<ConsoleElement*> m_Buffer;



public:
    static Console *getInstance();

    void openConsole();
    void print(std::string str, ...);

    bool parseCommand(std::string tstr);

    const std::vector<Command*> *getCommands() { return &m_CommandList;}
    const Command *findCommand(std::vector<std::string> *cmd);
};

void printMessages(std::vector<ConsoleElement*> *tlist, recti *trect = NULL);
bool addMessage(std::vector<ConsoleElement*> *tlist, std::string str, ...);
bool addMessageV(std::vector<ConsoleElement*> *tlist, std::string str, va_list v);

// commands
bool printMenuHelp(const Command *tcmd = NULL);
void printHelp(std::vector<std::string> *cmd);
void itemMenu(std::vector<std::string> *cmd);
void showItemInfo(std::vector<std::string> *cmd);
void printItemList(std::vector<std::string> *cmd);
void mytest(std::vector<std::string> *cmd);
void colortest(std::vector<std::string> *cmd);
void printPlayer(std::vector<std::string> *cmd);
void dbgClip(std::vector<std::string> *cmd);
void dbgLOS(std::vector<std::string> *cmd);
void dbgLighting(std::vector<std::string> *cmd);


#endif // CLASS_CONSOLE
