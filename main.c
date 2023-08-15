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

void TreeAddGroup(Group *tree, char *keys, char *name) {
  if( *(keys+1) == 0 ) {
    GroupAddChild(tree, NewGroup(*keys, name));
    return;
  }

  Group* g = FindGroup(tree, *keys);
  if( g == NULL ) {
    // TODO create the group with unnamed name if it doesn't exist instead of failing
    printf("Can't find a group with '%c' key", *keys);
    exit(EXIT_FAILURE);
  }

  TreeAddGroup(g, keys + 1, name);
}

void TreeAddCommand(Group *tree, char *keys, char *name, char *command) {
  if (*(keys + 1) == 0) {
    GroupAddCommand(tree, NewCommand(*keys, name, command));
    return;
  }

  Group *g = FindGroup(tree, *keys);
  if( g == NULL ) {
    // TODO create the group with unnamed name if it doesn't exist instead of failing
    printf("Can't find a group with '%c' key", *keys);
    exit(EXIT_FAILURE);
  }

  TreeAddCommand(g, keys+1, name, command);
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

char* ReadKey(char** file) {
  char* key = *file;
  while(**file != ',' && **file != '\n' && **file != 0) (*file)++;

  if( **file != ',') {
    printf("Found incorrect end after key, found: %c", **file);
    exit(EXIT_FAILURE);
  }

  **file = 0;
  (*file)++;

  return key;
}

char *ReadName(char **file) {
  char *name = *file;
  while (**file != ',' && **file != '\n' && **file != 0)
    (*file)++;

  if (**file != ',') {
    printf("Found incorrect end after name, found: %c", **file);
    exit(EXIT_FAILURE);
  }

  **file = 0;
  (*file)++;

  return name;
}

char* ReadCommand(char** file) {
  char* command = *file;
  while(**file != '\n' && **file != 0) (*file)++;

  if(**file == '\n') {
    **file = 0;
    (*file)++;
  }

  return command;
}

void ReadLine(Group* g, char** file) {
  char* key = ReadKey(file);
  char* name = ReadName(file);
  char* command = ReadCommand(file);

  if( *command == 0 ) {
    TreeAddGroup(g, key, name);
  } else {
    TreeAddCommand(g, key, name, command);
  }
}

void LoadFile(Group *g, char* file) {
  char* content = ReadFile(file);
  while( *content != 0 ) ReadLine(g, &content);
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
