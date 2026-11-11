#include "lineedit.h"
#include <termios.h>
#include <unistd.h>

static struct termios orig_termios;

void cash_raw_enable(void) {
  if (tcgetattr(STDIN_FILENO, &orig_termios) != 0) {
    return;
  }

  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
  raw.c_iflag &= ~(IXON | ICRNL);
  raw.c_oflag &= ~(OPOST);

  raw.c_cc[VMIN] = 1;
  raw.c_cc[VTIME] = 0;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void cash_raw_disable(void) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
