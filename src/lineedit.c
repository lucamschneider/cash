#include "lineedit.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

static struct termios orig_termios;

typedef struct {
  char *buf;
  size_t len;
  size_t cap;
  size_t cursor;
} LineEditor;

int cash_raw_enable(void) {
  if (tcgetattr(STDIN_FILENO, &orig_termios) != 0) {
    return -1;
  }

  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
  raw.c_iflag &= ~(IXON | ICRNL);
  raw.c_oflag &= ~(OPOST);

  raw.c_cc[VMIN] = 1;
  raw.c_cc[VTIME] = 0;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) != 0)
    return -1;
  return 0;
}

int cash_raw_disable(void) {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) != 0)
    return -1;
  return 0;
}

static int read_key(void) {
  char c;
  ssize_t n;
  do {
    n = read(STDIN_FILENO, &c, 1);
  } while (n == -1 && errno == EINTR);

  if (n != 1) {
    return -1;
  }

  if (c != '\x1b')
    return c;

  char seq[2];
  if (read(STDIN_FILENO, &seq[0], 1) != 1)
    return '\x1b';
  if (read(STDIN_FILENO, &seq[1], 1) != 1)
    return '\x1b';

  if (seq[0] == '[') {
    switch (seq[1]) {
    case 'A':
      return KEY_UP;
    case 'B':
      return KEY_DOWN;
    case 'C':
      return KEY_RIGHT;
    case 'D':
      return KEY_LEFT;
    }
  }

  return '\x1b';
}

static int editor_insert(LineEditor *e, char c) {
  if (e->len + 1 >= e->cap) {
    size_t newcap = e->cap ? e->cap * 2 : 16;
    char *tmp = realloc(e->buf, newcap);

    if (!tmp)
      return -1;

    e->buf = tmp;
    e->cap = newcap;
  }

  memmove(e->buf + e->cursor + 1, e->buf + e->cursor, e->len - e->cursor);

  e->buf[e->cursor] = c;
  e->cursor++;
  e->len++;
  e->buf[e->len] = '\0';

  return 0;
}

static int editor_backspace(LineEditor *e) {
  if (e->cursor == 0)
    return -1;

  memmove(e->buf + e->cursor - 1, e->buf + e->cursor, e->len - e->cursor);

  e->cursor--;
  e->len--;
  e->buf[e->len] = '\0';
  return 0;
}

static void editor_left(LineEditor *e) {
  if (e->cursor > 0)
    (e->cursor)--;
}

static void editor_right(LineEditor *e) {
  if (e->cursor < e->len)
    (e->cursor)++;
}

static void refresh_line(LineEditor *e) {
  char out[256];
  int n = 0;

  n += snprintf(out + n, sizeof(out) - n, "\r\x1b[K");
  n += snprintf(out + n, sizeof(out) - n, "$ ");
  n += snprintf(out + n, sizeof(out) - n, "%.*s", (int)e->len, e->buf);
  n += snprintf(out + n, sizeof(out) - n, "\r\x1b[%zuC", (size_t)2 + e->cursor);

  write(STDOUT_FILENO, out, n);
}

char *cash_read_line(void) {
  cash_raw_enable();

  LineEditor e = {0};

  for (;;) {
    refresh_line(&e);
    int k = read_key();

    switch (k) {
    case '\r':
      write(STDOUT_FILENO, "\r\n", 2);
      cash_raw_disable();
      return e.buf;

    case -1:
      free(e.buf);
      cash_raw_disable();
      return NULL;

    case 127:
    case 8:
      editor_backspace(&e);
      break;

    case KEY_LEFT:
      editor_left(&e);
      break;

    case KEY_RIGHT:
      editor_right(&e);
      break;

    default:
      if (k >= 32 && k < 127)
        editor_insert(&e, (char)k);
      break;
    }
  }
}

#ifdef LINEEDIT_TEST
#include <stdio.h>

int main(void) {
  cash_raw_enable();
  char *line = cash_read_line();
  cash_raw_disable();

  printf("\ngot: [%s]\n", line ? line : "(null)");
  free(line);
  return 0;
}
#endif
