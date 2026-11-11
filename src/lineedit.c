#include "lineedit.h"
#include <errno.h>
#include <termios.h>
#include <unistd.h>

static struct termios orig_termios;

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
