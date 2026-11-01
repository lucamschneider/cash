#include "builtins.h"
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define CASH_TOK_DELIM " \t\r\n\a"

char *cash_read_line(void) {
  size_t bufsize = 0;
  char *buffer = NULL;

  ssize_t n = getline(&buffer, &bufsize, stdin);

  if (n == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);
    } else {
      perror("cash");
      exit(EXIT_FAILURE);
    }
  }

  return buffer;
}

char **cash_parse_line(char *line) {
  char **tokens = NULL;
  size_t n = 0, cap = 0;
  char *tok = strtok(line, CASH_TOK_DELIM);

  while (1) {
    if (n == cap) {
      cap = (cap == 0) ? 64 : cap * 2;
      char **tmp = realloc(tokens, cap * sizeof(char *));
      if (tmp == NULL) {
        free(tokens);
        perror("cash");
        exit(EXIT_FAILURE);
      }
      tokens = tmp;
    }
    tokens[n] = tok;
    n++;

    if (tok == NULL) {
      return tokens;
    }
    tok = strtok(NULL, CASH_TOK_DELIM);
  }
}

int cash_launch(char **args) {
  if (args[0] == NULL)
    return 1;

  int status;
  pid_t pid = fork();

  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("cash");
      exit(EXIT_FAILURE);
    }
  } else if (pid < 0) {
    perror("cash");
    return 1;
  } else {
    pid_t ret;
    do {
      ret = waitpid(pid, &status, WUNTRACED);
    } while (ret != -1 && !WIFEXITED(status) && !WIFSIGNALED(status));

    if (ret == -1) {
      perror("cash");
      return 1;
    }
  }

  return 1;
}

int cash_execute(char **args) {
  if (args[0] == NULL) {
    return 1;
  }
  for (int i = 0; i < num_builtins; i++) {
    if (strcmp(args[0], builtins[i].name) == 0) {
      return builtins[i].fn(args);
    }
  }
  return cash_launch(args);
}

void cash_print_prompt() {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("cash");
    return;
  }

  if (isatty(STDOUT_FILENO)) {
    printf("\033[1;32m%s\033[0m\n$ ", cwd);
  } else {
    printf("%s\n$ ", cwd);
  }
  fflush(stdout);
}

int main(void) {
  char *line;
  char **args;
  int status = 1;

  do {
    cash_print_prompt();
    line = cash_read_line();
    args = cash_parse_line(line);
    status = cash_execute(args);

    free(line);
    free(args);
  } while (status);

  return 0;
}
