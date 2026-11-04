#include "prompt.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define COLOR_CWD "\033[1;33m"
#define COLOR_BRANCH "\033[1;35m"
#define COLOR_GRAY "\033[0;90m"
#define COLOR_RESET "\033[0m"

const char *cash_truncate_cwd(const char *cwd, int max_depth) {
  const char *start = cwd;
  int depth = 0;
  size_t len = strlen(cwd);

  for (size_t i = len; i > 0; i--) {
    if (cwd[i] == '/' && ++depth == max_depth) {
      start = cwd + i + 1;
      break;
    }
  }
  return start;
}

char *cash_get_gitbranch(void) {
  FILE *f = fopen(".git/HEAD", "r");
  if (f == NULL)
    return NULL;

  char head[256];
  if (fgets(head, sizeof(head), f) == NULL) {
    fclose(f);
    return NULL;
  }
  fclose(f);

  const char *prefix = "ref: refs/heads/";
  size_t plen = strlen(prefix);
  if (strncmp(head, prefix, plen) != 0)
    return NULL;

  char *branch = head + plen;
  branch[strcspn(branch, "\n")] = '\0';
  return strdup(branch);
}

void cash_format_cwd(const char *cwd, char *out, size_t out_size) {
  const char *home = getenv("HOME");

  if (home != NULL) {
    size_t hlen = strlen(home);
    if (strncmp(cwd, home, hlen) == 0) {
      snprintf(out, out_size, "~%s", cwd + hlen);
      return;
    }
  }
  snprintf(out, out_size, "%s", cwd);
}

void cash_print_prompt(void) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("cash");
    return;
  }

  char display_cwd[PATH_MAX];
  cash_format_cwd(cwd, display_cwd, sizeof(display_cwd));

  char *branch = cash_get_gitbranch();
  int max_depth = (branch != NULL) ? 1 : 3;
  const char *start = cash_truncate_cwd(display_cwd, max_depth);

  if (isatty(STDOUT_FILENO)) {
    printf(COLOR_CWD "%s" COLOR_RESET " ", start);
    if (branch != NULL) {
      printf("on " COLOR_BRANCH " %s" COLOR_RESET, branch);
      free(branch);
    }
    printf("\n" COLOR_GRAY "╰" COLOR_RESET " $ ");
  } else {
    printf("%s\n$ ", start);
  }
  fflush(stdout);
}
