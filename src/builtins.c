#include "builtins.h"
#include <unistd.h>

int cash_exit(char **args) {
  (void)args;
  return 0;
}

int cash_cd(char **args) {
  chdir(args[1]);
  return 1;
}

builtin_t builtins[] = {
    {"cd", cash_cd},
    {"exit", cash_exit},
};

int num_builtins = sizeof(builtins) / sizeof(builtins[0]);
