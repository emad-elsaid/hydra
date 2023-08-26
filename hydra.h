#define ColorOff "\033[0m"
#define Red "\033[0;31m"
#define Green "\033[0;32m"
#define Yellow "\033[0;33m"
#define Blue "\033[0;34m"
#define Purple "\033[0;35m"
#define Cyan "\033[0;36m"
#define White "\033[0;37m"

// a margin to the right of the item
#define RightMargin 5
// a name used for commands that are not defined explictly
#define DefaultName "unnamed"

typedef struct Command {
  char key;
  char* name;
  char* command;

  struct Command* children;
  struct Command* next;
} Command;

Command* NewCommand(char key, char* name, char* command);
int CommandRun(Command *c) ;
void CommandAddChild(Command *c, Command *child) ;
Command *FindCommand(Command *c, char key) ;
void TreeAddCommand(Command *tree, char *keys, char *name, char *command) ;
int PrintCommand(Command *c);

char getch(void);

char* ReadFile(char* file) ;
char* ReadField(char** file, char *field);
char* ReadUntilEOL(char** file);

void ReadLine(Command* c, char** file);
void ClearLines(int count);
void LoadFile(Command *c, char *file);
void Start(Command *c);
