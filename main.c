#include "hydra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
    fprintf(stderr,
        "Usage: hydra FILE [FILE...]\n"
        "\n"
        "Environment variables:\n"
        "  HYDRA_COLOR_TITLE    Color of the category header    (default: blue)\n"
        "  HYDRA_COLOR_KEY      Color of the key shortcut char  (default: yellow)\n"
        "  HYDRA_COLOR_ARROW    Color of the arrow symbol       (default: purple)\n"
        "  HYDRA_COLOR_COMMAND  Color of parent command names   (default: blue)\n"
        "\n"
        "Accepted color values: black, red, green, yellow, blue, purple, magenta,\n"
        "                       cyan, white, off (no color)\n"
    );
    return EXIT_SUCCESS;
  }

  if( argc == 1 ) {
    fprintf(stderr, "Must provide at least one hydra file");
    return EXIT_FAILURE;
  }

  Command* tree = NewCommand(0, 0, 0);
  for(int i=1; i<argc; i++) LoadFile(tree, argv[i]);

  Start(tree);

  return EXIT_SUCCESS;
}
