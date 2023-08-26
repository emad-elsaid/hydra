#include "hydra.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

Command* NewCommand(char key, char* name, char* command) {
  Command* cmd = (Command*) calloc(1, sizeof(Command));
  cmd->key = key;
  cmd->name = name;
  cmd->command = command;

  return cmd;
}

int CommandRun(Command *c) {
  if (c != NULL && c->command != NULL)
    return fprintf(stdout, "%s", c->command);

  return 0;
}

void CommandAddChild(Command *c, Command *child) {
  if (c->children == NULL) {
    c->children = child;
    return;
  }

  if(c->children->key > child->key) {
    child->next = c->children;
    c->children = child;
    return;
  }

  Command *lastChild = c->children;
  while (lastChild->next != NULL && lastChild->next->key <= child->key){
    lastChild = lastChild->next;
  }

  child->next = lastChild->next;
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
  ioctl(STDERR_FILENO, TIOCGWINSZ, &terminal);
  int width = terminal.ws_col;

  // Keep track of how many characters printed
  int lines = 0;

  if (c->name) {
    fprintf(stderr, "%s%s:%s\n", Blue, c->name, ColorOff);
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
      fprintf(stderr, "%s%c%s %s➔%s %s+%-*s%s", Yellow, child->key, ColorOff, Purple,
             ColorOff, Blue, maxLineWidth, child->name, ColorOff);
    } else {
      fprintf(stderr, "%s%c%s %s➔%s  %-*s", Yellow, child->key, ColorOff, Purple,
             ColorOff, maxLineWidth, child->name);
    }

    if (currentItem % itemsPerRow == 0) {
      fprintf(stderr, "\n");
      lines ++;
    }

    child = child->next;
  }

  fprintf(stderr, "\n");
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
    fprintf(stderr, "Found incorrect end after %s, found: %c", field, * *file);
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
    fprintf(stderr, "\033[A\r\33[2K");
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
    if( CommandRun(c) > 0 ){
      return;
    };
  }
}
