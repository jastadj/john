#ifndef CLASS_CONSOLE
#define CLASS_CONSOLE

#include "curses.h"
#include <string>
#include <vector>

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
    enum E_TYPE{TYPE_TEXT, TYPE_COLOR, TYPE_CLEARCOLOR};

    E_TYPE m_Type;
    std::string m_Text;
    int foreground;
    int background;
    bool color_bold;
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
    std::vector<ConsoleElement> m_Buffer;



public:
    static Console *getInstance();

    void openConsole();
    void print(std::string str);
    void setColor(int foreground, int background = COLOR_BLACK, bool bold=false);
    void clearColor();

    bool parseCommand(std::string tstr);

    const std::vector<Command*> *getCommands() { return &m_CommandList;}
    const Command *findCommand(std::vector<std::string> *cmd);
};

// commands
bool printMenuHelp(const Command *tcmd = NULL);
void printHelp(std::vector<std::string> *cmd);
void itemMenu(std::vector<std::string> *cmd);
void showItemInfo(std::vector<std::string> *cmd);
void printItemList(std::vector<std::string> *cmd);
void mytest(std::vector<std::string> *cmd);
void dbgClip(std::vector<std::string> *cmd);
void dbgLOS(std::vector<std::string> *cmd);
void dbgLighting(std::vector<std::string> *cmd);


#endif // CLASS_CONSOLE
