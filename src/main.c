#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *cash_read_line(void);
char **cash_parse_line(char *line);

int main(void) {
  printf("$ ");
  char *lines = cash_read_line();
  char **args = cash_parse_line(lines);

  printf("$ args:\n");
  for (size_t i = 0; args[i] != NULL; i++) {
    printf("┝ %s\n", args[i]);
  }

  free(lines);
  return 0;
}

char *cash_read_line(void) {
  size_t bufsize = 0;
  char *buffer = NULL;

  ssize_t n = getline(&buffer, &bufsize, stdin);

  if (n == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);
    } else {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }

  return buffer;
}

char **cash_parse_line(char *line) {
  char **tokens = NULL;
  size_t n = 0, cap = 0;
  char *tok = strtok(line, " \t\r\n\a");

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
    tok = strtok(NULL, " \t\r\n\a");
  }
}
