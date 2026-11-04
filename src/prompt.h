#ifndef CASH_PROMPT_H
#define CASH_PROMPT_H

void cash_print_prompt(void);
const char *cash_truncate_cwd(const char *cwd);
char *cash_get_gitbranch();

#endif // CASH_PROMPT_H
