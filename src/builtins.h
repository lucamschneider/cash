#ifndef CASH_BUILTINS_H
#define CASH_BUILTINS_H

extern int running;

typedef int (*builtin_fn)(char **args);

typedef struct {
  char *name;
  builtin_fn fn;
} builtin_t;

extern builtin_t builtins[];
extern int num_builtins;

int cash_exit(char **args);
int cash_cd(char **args);
int cash_ls(char **args);

#endif // CASH_BUILTINS_H
