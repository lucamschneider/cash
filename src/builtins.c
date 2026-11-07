#include "builtins.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "colors.h"

#define LS_MAX_WIDTH 70
#define LS_COL_OVERHEAD 13
#define SECS_PER_HOUR 3600
#define SECS_PER_DAY 86400

#define KB (1024LL)
#define MB (1024LL * 1024)
#define GB (1024LL * 1024 * 1024)
#define TB (1024LL * 1024 * 1024 * 1024)

typedef struct {
  int total;
  int row;
  int name;
  int size;
  int modified;
} ls_width_t;

typedef enum { BORDER_TOP, BORDER_BOTTOM } border_pos_t;

static void cash_ls_get_widths(ls_width_t *widths, int entry_count, int cols);
static void cash_ls_print_row(struct dirent *entry, struct stat *st,
                              ls_width_t widths, int *row);
static void cash_ls_print_border(ls_width_t *widths, border_pos_t pos);

int cash_exit(char **args) {
  (void)args;
  running = 0;
  return 0;
}

int cash_cd(char **args) {
  const char *path = args[1] != NULL ? args[1] : getenv("HOME");
  if (path == NULL) {
    fprintf(stderr, "cash: cd: $HOME not set\n");
    return 1;
  }
  if (chdir(path) != 0) {
    perror("cash");
    return 1;
  }
  return 0;
}

void cash_ls_get_widths(ls_width_t *widths, int entry_count, int cols) {
  widths->total = cols >= LS_MAX_WIDTH ? LS_MAX_WIDTH : cols;

  widths->row = snprintf(NULL, 0, "%d", entry_count - 1);

  int available_width = widths->total - widths->row - LS_COL_OVERHEAD;
  widths->name = available_width * 5 / 10;
  widths->size = available_width * 2 / 10;
  widths->modified = available_width - widths->name - widths->size;
}

static void format_size(long long bytes, char *buf, int len) {
  if (bytes < KB)
    snprintf(buf, len, "%lld B", bytes);
  else if (bytes < MB)
    snprintf(buf, len, "%.1f KB", bytes / (double)KB);
  else if (bytes < GB)
    snprintf(buf, len, "%.1f MB", bytes / (double)MB);
  else if (bytes < TB)
    snprintf(buf, len, "%.1f GB", bytes / (double)GB);
  else
    snprintf(buf, len, "%.1f TB", bytes / (double)TB);
}

static void format_modified(time_t mtime, char *buf, int len) {
  time_t now = time(NULL);
  long diff = (long)difftime(now, mtime);
  if (diff < SECS_PER_HOUR)
    snprintf(buf, len, "%ldm ago", diff / 60);
  else if (diff < SECS_PER_DAY)
    snprintf(buf, len, "%ldh ago", diff / 3600);
  else {
    struct tm *tm = localtime(&mtime);
    if (tm == NULL) {
      snprintf(buf, len, "unknown");
    } else {
      strftime(buf, len, "%d.%m.%Y %H:%M", tm);
    }
  }
}

static time_t get_mtime(struct stat *st) {
#ifdef __APPLE__
  return st->st_mtimespec.tv_sec;
#else
  return st->st_mtime;
#endif /* ifdef __APPLE__ */
}

void cash_ls_print_row(struct dirent *entry, struct stat *st, ls_width_t widths,
                       int *row) {
  char time_buf[32];
  format_modified(get_mtime(st), time_buf, sizeof(time_buf));
  char size_buf[32];
  format_size((long long)st->st_size, size_buf, sizeof(size_buf));

  printf(COLOR_GRAY "│ %-*d │" COLOR_RESET " %-*s " COLOR_GRAY
                    "│ %*s │ %*s │" COLOR_RESET "\n",
         widths.row, (*row)++, widths.name, entry->d_name, widths.size,
         size_buf, widths.modified, time_buf);
}

void cash_ls_print_border(ls_width_t *widths, border_pos_t pos) {
  int row_end = widths->row + 2;
  int name_end = row_end + 1 + widths->name + 2;
  int size_end = name_end + 1 + widths->size + 2;

  const char *corner_l = pos == BORDER_TOP ? "╭" : "╰";
  const char *corner_r = pos == BORDER_TOP ? "╮" : "╯";
  const char *separator = pos == BORDER_TOP ? "┬" : "┴";

  printf(COLOR_GRAY "%s" COLOR_RESET, corner_l);
  for (int i = 0; i < widths->total - 2; i++) {
    if (i == row_end || i == name_end || i == size_end) {
      printf(COLOR_GRAY "%s" COLOR_RESET, separator);
      continue;
    }
    printf(COLOR_GRAY "─" COLOR_RESET);
  }
  printf(COLOR_GRAY "%s" COLOR_RESET "\n", corner_r);
}

int cash_ls(char **args) {
  int show_hidden = args[1] != NULL && strcmp(args[1], "-a") == 0;

  struct dirent **entries;
  int n = scandir(".", &entries, NULL, alphasort);
  if (n < 0) {
    perror("cash: ls");
    return 1;
  }

  struct winsize w;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) < 0 || w.ws_col == 0)
    w.ws_col = 80;
  int cols = w.ws_col;

  int visible = 0;
  for (int i = 0; i < n; i++) {
    if (show_hidden || entries[i]->d_name[0] != '.')
      visible++;
  }

  ls_width_t widths;
  cash_ls_get_widths(&widths, visible, cols);

  cash_ls_print_border(&widths, BORDER_TOP);

  int row = 0;
  struct stat st;

  for (int i = 0; i < n; i++) {
    if (entries[i]->d_name[0] == '.' && !show_hidden) {
      free(entries[i]);
      continue;
    }

    if (lstat(entries[i]->d_name, &st) != 0) {
      free(entries[i]);
      continue;
    }

    cash_ls_print_row(entries[i], &st, widths, &row);
    free(entries[i]);
  }
  free(entries);

  cash_ls_print_border(&widths, BORDER_BOTTOM);

  return 0;
}

int cash_help(char **args) {
  (void)args;
  printf(COLOR_LOGO_1 "                   __\n" COLOR_RESET);
  printf(COLOR_LOGO_2 "  _________ ______/ /_\n" COLOR_RESET);
  printf(COLOR_LOGO_3 " / ___/ __ `/ ___/ __ \\\n" COLOR_RESET);
  printf(COLOR_LOGO_4 "/ /__/ /_/ (__  ) / / /\n" COLOR_RESET);
  printf(COLOR_LOGO_5 "\\___/\\__,_/____/_/ /_/\n" COLOR_RESET);
  printf(COLOR_GRAY "╰ $ Cash Ain't SH\n\n" COLOR_RESET);
  printf("  cd [dir]   change directory (default: $HOME)\n");
  printf("  ls [-a]    list directory contents\n");
  printf("  help       show this help\n");
  printf("  exit       exit cash\n");
  printf("\n");
  return 0;
}

builtin_t builtins[] = {
    {"cd", cash_cd},
    {"exit", cash_exit},
    {"ls", cash_ls},
    {"help", cash_help},
};

int num_builtins = sizeof(builtins) / sizeof(builtins[0]);
