#include "hydra.c"
#include <string.h>
#include <assert.h>

void TestStart(char *name) {
  setbuf(stdout, NULL);
  printf("-- Testing %s\n", name);
}

void TestNewCommand() {
  TestStart("NewCommand");

  Command *cmd = NewCommand('k', "name", "command");
  assert(cmd != NULL);
  assert(cmd->key == 'k');
  assert(cmd->name != NULL && strcmp(cmd->name, "name") == 0);
  assert(cmd->command != NULL && strcmp(cmd->command, "command") == 0);
  assert(cmd->children == NULL);
  assert(cmd->next == NULL);
}

void TestCommandAddChild() {
  TestStart("CommandAddChild");

  Command *parent = NewCommand(0, 0, 0);
  Command *firstChild = NewCommand('a', 0, 0);
  Command *secondChild = NewCommand('b', 0, 0);
  Command *thirdChild = NewCommand('c', 0, 0);

  CommandAddChild(parent, thirdChild);
  CommandAddChild(parent, firstChild);
  CommandAddChild(parent, secondChild);
  assert(parent->children == firstChild);
  assert(parent->children->next == secondChild);
  assert(parent->children->next->next == thirdChild);
}

int main() {
  TestNewCommand();
  TestCommandAddChild();
}
