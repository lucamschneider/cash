#include "prompt.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void cash_print_prompt(void) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("cash");
    return;
  }

  const char *start = cwd;
  int depth = 0;
  size_t len = strlen(cwd);

  for (size_t i = len; i > 0; i--) {
    if (cwd[i] == '/' && ++depth == 3) {
      start = cwd + i + 1;
      break;
    }
  }

  if (isatty(STDOUT_FILENO)) {
    printf("\033[1;32m%s\033[0m\n$ ", start);
  } else {
    printf("%s\n$ ", start);
  }
  fflush(stdout);
}

char cash_truncate_cwd(char cwd) { return start; }
