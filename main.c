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

void GroupAddChild(Group *g, Group *child) {
  Group *lastChild = g->children;
  if (lastChild == NULL) {
    g->children = child;
    return;
  }

  while (lastChild->next != NULL)
    lastChild = lastChild->next;
  lastChild->next = child;
}

void TreeAddChild(char* keys, Group* tree, Group* child) {
  printf("TreeAddChild: %s -> %s\n", keys, child->name);
}

void TreeAddCommand(char *keys, Group* tree, Command* child) {
  printf("TreeAddcommand: %s -> %s -> %s\n", keys, child->name, child->command);
}

void GroupAddCommand(Group *g, Command *cmd) {
  Command *lastCommand = g->commands;
  if (lastCommand == NULL) {
    g->commands = cmd;
    return;
  }

  while (lastCommand->next != NULL)
    lastCommand = lastCommand->next;
  lastCommand->next = cmd;
}

Group *FindGroup(Group *g, char key) {
  Group *child = g->children;
  while (child != NULL && child->key != key)
    child = child->next;
  return child;
}

Command *FindCommand(Group *g, char key) {
  Command *child = g->commands;
  while (child != NULL && child->key != key)
    child = child->next;
  return child;
}

void PrintGroup(Group *g) {
  if( g->name )
    printf("%s%s:%s\n", Blue, g->name, ColorOff);


  Group* child = g->children;
  while(child) {
    printf("%s%c%s %s➔%s %s+%s%s\t\t",
           Yellow, child->key, ColorOff,
           Purple, ColorOff,
           Blue, child->name, ColorOff);
    child = child->next;
  }

  Command* command = g->commands;
  while(command) {
    printf("%s%c%s %s➔%s %s\t\t",
           Yellow, command->key, ColorOff,
           Purple, ColorOff,
           command->name);
    command = command->next;
  }

  printf("\n");
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

char* ReadFile(char* file) {
  FILE* f = fopen(file, "r");
  if( f == NULL ) {
    perror("Failed to open file");
    exit(EXIT_FAILURE);
  }

  fseek(f, 0, SEEK_END);
  long fileSize = ftell(f);
  rewind(f);

  char* content = (char*) malloc(fileSize+1);
  fread(content, fileSize, 1, f);
  content[fileSize] = 0;
  fclose(f);

  return content;
}

char* NextSeparator(char* file) {
  while( *file != ',' && *file != '\n' && *file != 0 ) file++;
  return file;
}

char* NextEndOfLine(char* file) {
  while( *file != '\n' && *file != 0 ) file++;
  return file;
}


void LoadFile(Group *g, char* file) {
  char* content = ReadFile(file);

  bool eof = false;
  int line = 0;

  while(!eof) {
    char* key = content;
    char* keySep = NextSeparator(key);

    if( *keySep == '\n' ) {
      printf("Found new line after key while a name is expected: %d", line);
      exit(EXIT_FAILURE);
    } else if ( *keySep == 0 ) {
      printf("Found end of file after key while a name is expected: %d", line);
      exit(EXIT_FAILURE);
    }

    *keySep = 0;

    char* name = keySep+1;
    char* nameSep = NextSeparator(name);

    if( *nameSep == '\n' ) {
      *nameSep = 0;
      Group* childGroup = NewGroup(*(keySep-1), name);
      TreeAddChild(key, g, childGroup);
      line++;

    } else if ( *nameSep == 0 ) {
      Group* childGroup = NewGroup(*(keySep-1), name);
      TreeAddChild(key, g, childGroup);
      eof = true;

    }else {
      char* command = nameSep+1;
      char* commandSep = NextEndOfLine(command);
      if( *commandSep != '\n' && *commandSep != 0 ){
        printf("Didn't find new line nor end of file after command: %d", line);
        exit(EXIT_FAILURE);
      }

      if( *commandSep == 0 ) eof = true;

      *commandSep = 0;
      Command* childCommand = NewCommand(*(commandSep-1), name, command);
      TreeAddCommand(key, g, childCommand);
      line++;

      content = commandSep+1;
    }
  }
}

int main(int argc, char** argv) {
  if( argc == 1 ) {
    printf("Must provide at least one hydra file");
    return EXIT_FAILURE;
  }

  Group* currentGroup = NewGroup(0, 0);

  for(int i=1; i<argc; i++) LoadFile(currentGroup, argv[i]);

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

  return EXIT_SUCCESS;
}
