#include "hydra.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char **argv)
{
  if (argc == 1)
  {
    fprintf(stderr, "Must provide at least one hydra file");
    return EXIT_FAILURE;
  }

  bool recursive_mode = false;
  char *folder_name = NULL;

  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--recursive") == 0)
    {
      recursive_mode = true;
      folder_name = (char *)malloc(strlen(argv[i + 1]) * sizeof(char));
      if (folder_name == NULL)
      {
        perror("malloc failed");
        return EXIT_FAILURE;
      }
      strcpy(folder_name, argv[i + 1]);
    }
  }

  Command *tree = NewCommand(0, 0, 0);

  if (recursive_mode)
  {
    DIR *folder;
    struct dirent *entry;
    int files = 0;

    folder = opendir(folder_name);
    if (folder == NULL)
    {
      perror("Unable to read directory");
      return EXIT_FAILURE;
    }

    while ((entry = readdir(folder)))
    {
      files++;

      size_t path_len = strlen(folder_name) + strlen(entry->d_name) + 1;
      char *path = (char *)malloc(path_len * sizeof(char));
      if (path == NULL)
      {
        perror("malloc failed");
        return EXIT_FAILURE;
      }

      snprintf(path, path_len + 1, "%s/%s", folder_name, entry->d_name);
      if (entry->d_name[0] != '.')
        LoadFile(tree, path);
    }

    closedir(folder);
  }
  else
  {

    for (int i = 1; i < argc; i++)
      LoadFile(tree, argv[i]);
  }

  Start(tree);

  return EXIT_SUCCESS;
}
