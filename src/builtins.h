#ifndef CASH_BUILTINS_H
#define CASH_BUILTINS_H

typedef int (*builtin_fn)(char **args);

typedef struct {
  char *name;
  builtin_fn fn;
} builtin_t;

extern builtin_t builtins[];
extern int num_builtins;

int cash_exit(char **args);

#endif // CASH_BUILTINS_H
