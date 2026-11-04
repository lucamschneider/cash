#include "prompt.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

char *cash_truncate_cwd(char *cwd) {
  char *start = cwd;
  int depth = 0;
  size_t len = strlen(cwd);

  for (size_t i = len; i > 0; i--) {
    if (cwd[i] == '/' && ++depth == 3) {
      start = cwd + i + 1;
      break;
    }
  }
  return start;
}

char *cash_get_gitbranch() {
  struct stat st;
  if (stat(".git", &st) != 0)
    return NULL;

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

void cash_print_prompt(void) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("cash");
    return;
  }

  const char *start = cash_truncate_cwd(cwd);

  if (isatty(STDOUT_FILENO)) {
    printf("\033[1;33m%s\033[0m ", start);
    char *branch = cash_get_gitbranch();
    if (branch != NULL) {
      printf("on \033[1;35m %s\033[0m", cash_get_gitbranch());
    }
    printf("\n╰ $ ");
  } else {
    printf("%s\n$ ", start);
  }
  fflush(stdout);
}
