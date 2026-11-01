#include "builtins.h"

int cash_exit(char **args) {
  (void)args;
  return 0;
}

builtin_t builtins[] = {
    {"exit", cash_exit},
};

int num_builtins = sizeof(builtins) / sizeof(builtins[0]);
