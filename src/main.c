#include "builtins.h"
#include "prompt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define CASH_TOK_DELIM " \t\r\n\a"

int running = 1;

char *cash_expand_operators(const char *line) {
  char *out = malloc(strlen(line) * 3 + 1);
  int j = 0;
  for (int i = 0; line[i] != '\0'; i++) {
    if (line[i] == ';') {
      out[j++] = ' ';
      out[j++] = ';';
      out[j++] = ' ';
    } else {
      out[j++] = line[i];
    }
  }
  out[j] = '\0';
  return out;
}

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
    return 0;

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
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
  }

  return 0;
}

int cash_execute_cmd(char **args) {
  if (args[0] == NULL)
    return 0;

  if (args[0][0] == '^') {
    args[0]++;
    return cash_launch(args);
  }

  for (int i = 0; i < num_builtins; i++) {
    if (strcmp(args[0], builtins[i].name) == 0) {
      return builtins[i].fn(args);
    }
  }
  return cash_launch(args);
}

int cash_execute(char **args) {
  char **cmd = args;

  for (int i = 0; args[i] != NULL; i++) {
    if (strcmp(args[i], ";") == 0) {
      args[i] = NULL;
      cash_execute_cmd(cmd);
      cmd = args + i + 1;
    }
  }

  return cash_execute_cmd(cmd);
}

int main(void) {
  char *line;
  char **args;

  while (running) {
    cash_print_prompt();
    line = cash_read_line();
    char *expanded = cash_expand_operators(line);
    args = cash_parse_line(expanded);
    cash_execute(args);

    free(line);
    free(expanded);
    free(args);
  }

  return 0;
}
