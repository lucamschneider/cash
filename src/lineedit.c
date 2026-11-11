#include "lineedit.h"
#include <errno.h>
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

int read_key(void) {
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

int editor_insert(LineEditor *e, char c) {
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
  (e->cursor)++;

  (e->len)++;
  e->buf[e->len] = '\0';

  return 0;
}

#ifdef LINEEDIT_TEST
#include <stdio.h>

int main(void) {
  LineEditor e = {0};

  editor_insert(&e, 'h');
  editor_insert(&e, 'e');
  editor_insert(&e, 'l');
  editor_insert(&e, 'l');
  editor_insert(&e, 'o');
  printf("[%s] len= %zu cursor= %zu\n", e.buf, e.len, e.cursor);

  e.cursor = 1;
  editor_insert(&e, 'x');
  printf("[%s] len= %zu cursor= %zu\n", e.buf, e.len, e.cursor);

  free(e.buf);
  return 0;
}
#endif
