#include "prompt.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "colors.h"

#define CWD_MAX_DEPTH 3

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

char *cash_get_gitbranch(const char *cwd, char *git_root, size_t root_size) {
  char tmp[PATH_MAX];
  strncpy(tmp, cwd, sizeof(tmp) - 1);
  tmp[sizeof(tmp) - 1] = '\0';

  FILE *f = NULL;
  while (1) {
    char githead[PATH_MAX];
    snprintf(githead, sizeof(githead), "%s/.git/HEAD", tmp);
    f = fopen(githead, "r");
    if (f != NULL) {
      strncpy(git_root, tmp, root_size - 1);
      git_root[root_size - 1] = '\0';
      break;
    }

    char *slash = strrchr(tmp, '/');
    if (slash == NULL || slash == tmp)
      return NULL;
    *slash = '\0';
  }

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
  char git_root[PATH_MAX];
  char *branch = cash_get_gitbranch(cwd, git_root, sizeof(git_root));

  const char *start;

  if (branch != NULL) {
    const char *repo_name = strrchr(git_root, '/') + 1;
    const char *rel = cwd + strlen(git_root);

    snprintf(display_cwd, sizeof(display_cwd), "%s%s", repo_name, rel);
    start = cash_truncate_cwd(display_cwd, CWD_MAX_DEPTH);
  } else {
    start = cash_truncate_cwd(display_cwd, CWD_MAX_DEPTH);
  }

  if (isatty(STDOUT_FILENO)) {
    printf("\n" COLOR_CWD "%s" COLOR_RESET " ", start);
    if (branch != NULL) {
      printf("on " COLOR_BRANCH " %s " COLOR_RESET, branch);
      free(branch);
    }
    printf("\n" COLOR_GRAY "╰" COLOR_RESET " $ ");
  } else {
    printf("%s\n$ ", start);
  }
  fflush(stdout);
}
