#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

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

Command* NewCommand(char key, char* name, char* command) {
  Command* cmd = (Command*) calloc(1, sizeof(Command));
  cmd->key = key;
  cmd->name = name;
  cmd->command = command;

  return cmd;
}

void CommandRun(Command *c) {
  if (c != NULL && c->command != NULL)
    system(c->command);
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
  Command *c = FindCommand(tree, *keys);

  if (*(keys + 1) == 0) {
    if( c == NULL ) {
      CommandAddChild(tree, NewCommand(*keys, name, command));
    } else { // if command already exists update name and command fields
      c->name = name;
      c->command = command;
    }
    return;
  }

  if (c == NULL) {
    c = NewCommand(*keys, DefaultName, 0);
    CommandAddChild(tree, c);
  }

  TreeAddCommand(c, keys + 1, name, command);
}

// Returns number of printed lines
int PrintCommand(Command *c) {

  struct winsize terminal;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);
  int width = terminal.ws_col;

  // Keep track of how many characters printed
  int lines = 0;

  if (c->name) {
    printf("%s%s:%s\n", Blue, c->name, ColorOff);
    lines++;
  }

  // Find longest item
  int maxLineWidth = 0;
  Command *child = c->children;
  while (child) {
    int lineWidth = strlen(child->name);
    if (lineWidth > maxLineWidth)
      maxLineWidth = lineWidth;

    child = child->next;
  }

  maxLineWidth += RightMargin;
  if (maxLineWidth > width)
    maxLineWidth = width;

  int itemsPerRow =
      width /
      (maxLineWidth + 5); // 5 is extra character printed before each item

  child = c->children;
  int currentItem = 0;
  while (child) {
    currentItem++;

    if (child->children != 0) {
      printf("%s%c%s %s➔%s %s+%-*s%s", Yellow, child->key, ColorOff, Purple,
             ColorOff, Blue, maxLineWidth, child->name, ColorOff);
    } else {
      printf("%s%c%s %s➔%s  %-*s", Yellow, child->key, ColorOff, Purple,
             ColorOff, maxLineWidth, child->name);
    }

    if (currentItem % itemsPerRow == 0) {
       printf("\n");
       lines ++;
    }

    child = child->next;
  }

  printf("\n");
  lines ++;

  return lines;
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

  char* content = (char*) calloc(fileSize+1, sizeof(char));
  fread(content, fileSize, 1, f);
  fclose(f);

  return content;
}

char* ReadField(char** file, char *field) {
  char* key = *file;
  while(**file != ',' && **file != '\n' && **file != 0) (*file)++;

  if( **file != ',') {
    printf("Found incorrect end after %s, found: %c", field, * *file);
    exit(EXIT_FAILURE);
  }

  **file = 0;
  (*file)++;

  return key;
}

// Read until end of line character
char* ReadUntilEOL(char** file) {
  char* s = *file;
  while(**file != '\n' && **file != 0) (*file)++;

  if(**file == '\n') {
    **file = 0;
    (*file)++;
  }

  return s;
}

void ReadLine(Command* c, char** file) {
  char* key = ReadField(file, "key");
  char* name = ReadField(file, "name");
  char* command = ReadUntilEOL(file);

  TreeAddCommand(c, key, name, command);
}

void ClearLines(int count) {
  // make sure we print directly to stdout without bufferring
  // This allow us to clear lines without waiting for new line
  // without it executing `system` will output lines then our output will go after
  setbuf(stdout, NULL);

  for (int i = 0; i < count; i++)
    printf("\033[A\r\33[2K");
}

void LoadFile(Command *c, char *file) {
  char *content = ReadFile(file);
  while (*content != 0)
    ReadLine(c, &content);
}

void Start(Command *c) {
  while (c != NULL && c->children != NULL) {
    int lastPrintedLines = PrintCommand(c);

    c = FindCommand(c, getch());
    ClearLines(lastPrintedLines);
    CommandRun(c);
  }
}

int main(int argc, char **argv) {
  if( argc == 1 ) {
    printf("Must provide at least one hydra file");
    return EXIT_FAILURE;
  }

  Command* tree = NewCommand(0, 0, 0);
  for(int i=1; i<argc; i++) LoadFile(tree, argv[i]);

  Start(tree);

  return EXIT_SUCCESS;
}
