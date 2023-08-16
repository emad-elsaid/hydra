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

  struct Command* children;
  struct Command* next;
} Command;

Command* NewCommand(char key, char* name, char* command) {
  Command* cmd = (Command*) malloc(sizeof(Command));
  cmd->key = key;
  cmd->name = name;
  cmd->command = command;
  cmd->children = NULL;
  cmd->next = NULL;

  return cmd;
}

void CommandAddChild(Command *c, Command *child) {
  Command *lastChild = c->children;
  if (lastChild == NULL) {
    c->children = child;
    return;
  }

  while (lastChild->next != NULL)
    lastChild = lastChild->next;
  lastChild->next = child;
}

Command *FindCommand(Command *c, char key) {
  Command *child = c->children;
  while (child != NULL && child->key != key)
    child = child->next;
  return child;
}

void TreeAddCommand(Command *tree, char *keys, char *name, char *command) {
  if (*(keys + 1) == 0) {
    CommandAddChild(tree, NewCommand(*keys, name, command));
    return;
  }

  Command *c = FindCommand(tree, *keys);
  if (c == NULL) {
    c = NewCommand(*keys, "unnamed", 0);
    CommandAddChild(tree, c);
  }

  TreeAddCommand(c, keys + 1, name, command);
}

void PrintCommand(Command *c) {
  if (c->name)
    printf("%s%s:%s\n", Blue, c->name, ColorOff);

  Command *child = c->children;
  while (child) {
    if (child->children != 0) {
      printf("%s%c%s %s➔%s %s+%s%s\t\t", Yellow, child->key, ColorOff, Purple,
             ColorOff, Blue, child->name, ColorOff);
    }else{
      printf("%s%c%s %s➔%s %s\t\t", Yellow, child->key, ColorOff, Purple,
             ColorOff, child->name);
    }
    child = child->next;
  }

  printf("\n");
}

// Copied from: https://stackoverflow.com/a/912796/458436
char getch(void) {
  struct termios old = {0};

  if (tcgetattr(0, &old) < 0) perror("tcsetattr()");

  tcflag_t oldflags = old.c_lflag;

  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;

  if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");

  char buf = 0;
  if (read(0, &buf, 1) < 0) perror("read()");

  old.c_lflag = oldflags;

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

void ReadLine(Command* c, char** file) {
  char* key = ReadKey(file);
  char* name = ReadName(file);
  char* command = ReadCommand(file);

  TreeAddCommand(c, key, name, command);
}

void LoadFile(Command *c, char* file) {
  char* content = ReadFile(file);
  while( *content != 0 ) ReadLine(c, &content);
}

int main(int argc, char** argv) {
  if( argc == 1 ) {
    printf("Must provide at least one hydra file");
    return EXIT_FAILURE;
  }

  Command* currentCommand = NewCommand(0, 0, 0);

  for(int i=1; i<argc; i++) LoadFile(currentCommand, argv[i]);

  while(true) {
    PrintCommand(currentCommand);

    char choice = getch();
    Command* nextCommand = FindCommand(currentCommand, choice);

    if (nextCommand->command != NULL)
      system(nextCommand->command);

    if (nextCommand->children == NULL)
      break;

    currentCommand = nextCommand;
  }

  return EXIT_SUCCESS;
}
