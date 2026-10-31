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
      tokens = realloc(tokens, cap * sizeof(char *));
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
  int status;
  pid_t pid = fork();

  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("cash");
      exit(EXIT_FAILURE);
    }
  } else if (pid < 0) {
    perror("cash");
  } else {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return WEXITSTATUS(status);
}

int main(void) {
  char *line;
  char **args;
  int status = 1;

  do {
    printf("$ ");
    fflush(stdout);
    line = cash_read_line();
    args = cash_parse_line(line);
    status = cash_launch(args);

    free(line);
    free(args);
  } while (status);

  return 0;
}
