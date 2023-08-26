#include "hydra.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if( argc == 1 ) {
    fprintf(stderr, "Must provide at least one hydra file");
    return EXIT_FAILURE;
  }

  Command* tree = NewCommand(0, 0, 0);
  for(int i=1; i<argc; i++) LoadFile(tree, argv[i]);

  Start(tree);

  return EXIT_SUCCESS;
}
