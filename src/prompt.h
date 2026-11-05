#include <stddef.h>

#ifndef CASH_PROMPT_H
#define CASH_PROMPT_H

void cash_print_prompt(void);
const char *cash_truncate_cwd(const char *cwd, int max_depth);
char *cash_get_gitbranch(const char *cwd, char *git_root, size_t root_size);
const char *cash_set_home(const char *cwd);

#endif // CASH_PROMPT_H
