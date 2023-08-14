#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define ColorOff "\033[0m"
#define Red "\033[0;31m"
#define Green "\033[0;32m"
#define Yellow "\033[0;33m"
#define Blue "\033[0;34m"
#define Purple "\033[0;35m"
#define Cyan "\033[0;36m"
#define White "\033[0;37m"

typedef struct Command {
  char key;
  char* name;
  char* command;

  struct Command* next;
} Command;

Command* NewCommand(char key, char* name, char* command) {
  Command* cmd = (Command*) malloc(sizeof(Command));
  cmd->key = key;
  cmd->name = name;
  cmd->command = command;
  cmd->next = NULL;

  return cmd;
}

typedef struct Group {
  char key;
  char* name;

  struct Group* children;
  Command* commands;

  struct Group* next;
} Group;

Group* NewGroup(char key, char* name) {
  Group *grp = (Group*) malloc(sizeof(Group));
  grp->key = key;
  grp->name = name;
  grp->children=NULL;
  grp->commands=NULL;
  grp->next=NULL;

  return grp;
}

void PrintGroup(Group* g) {
  printf("%s%s%s\n", Blue, g->name, ColorOff);

  Group* child = g->children;
  while(child) {
    printf("%s%c%s ➔ +%s\t\t", Yellow, child->key, ColorOff, child->name);
    child = child->next;
  }

  Command* command = g->commands;
  while(command) {
    printf("%s%c%s ➔ %s\t\t", Yellow, command->key, ColorOff, command->name);
    command = command->next;
  }

  printf("\n");
}

void GroupAddChild(Group* g, Group* child) {
  Group* lastChild = g->children;
  if(lastChild == NULL){
      g->children = child;
      return;
  }

  while(lastChild->next!=NULL) lastChild = lastChild->next;
  lastChild->next = child;
}

void GroupAddCommand(Group* g, Command* cmd) {
  Command* lastCommand = g->commands;
  if( lastCommand == NULL ) {
    g->commands = cmd;
    return;
  }

  while(lastCommand->next!=NULL) lastCommand = lastCommand->next;
  lastCommand->next = cmd;
}

Group *FindGroup(Group *g, char key) {
  Group *child = g->children;
  while (child != NULL && child->key != key)
    child = child->next;
  return child;
}

Command *FindCommand(Group *g, char key) {
  Command* child = g->commands;
  while( child != NULL && child->key != key) child = child->next;
  return child;
}

// Copied from: https://stackoverflow.com/a/912796/458436
char getch(void) {
  struct termios old = {0};

  if (tcgetattr(0, &old) < 0) perror("tcsetattr()");

  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;

  if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");

  char buf = 0;
  if (read(0, &buf, 1) < 0) perror("read()");

  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;

  if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("tcsetattr ~ICANON");
  return (buf);
}

Group* BuildRoot(void) {
  Group *root = NewGroup('-', "Root");

  Group *git = NewGroup('g', "Git");
  GroupAddChild(root, git);

  Command *git_status = NewCommand('s', "Status", "git status");
  GroupAddCommand(git, git_status);

  return root;
}

int main() {
  Group* currentGroup = BuildRoot();

  while(true) {
    PrintGroup(currentGroup);

    char choice = getch();
    Group* nextGroup = FindGroup(currentGroup, choice);
    Command* nextCommand = FindCommand(currentGroup, choice);

    if ( nextGroup != NULL ) {
      currentGroup = nextGroup;
    } else if ( nextCommand != NULL ) {
      system(nextCommand->command);
      break;
    } else {
      break;
    }
  }
  return 0;
}
