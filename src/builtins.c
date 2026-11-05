#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int cash_exit(char **args) {
  (void)args;
  return 0;
}

int cash_cd(char **args) {
  const char *path = args[1] != NULL ? args[1] : getenv("HOME");
  if (path == NULL) {
    fprintf(stderr, "cash: cd: $HOME not set\n");
    return 1;
  }
  if (chdir(path) != 0) {
    perror("cash");
  }
  return 1;
}

builtin_t builtins[] = {
    {"cd", cash_cd},
    {"exit", cash_exit},
};

int num_builtins = sizeof(builtins) / sizeof(builtins[0]);
