#include "lineedit.h"
#include <stdio.h>

int main(void) {
  cash_raw_enable();

  for (;;) {
    int k = read_key();
    if (k == 'q')
      break;

    switch (k) {
    case KEY_LEFT:
      printf("LEFT\r\n");
      break;
    case KEY_RIGHT:
      printf("RIGHT\r\n");
      break;
    case KEY_UP:
      printf("UP\r\n");
      break;
    case KEY_DOWN:
      printf("DOWN\r\n");
      break;
    default:
      if (k >= 32 && k < 127)
        printf("key %d ('%c')\r\n", k, k);
      else
        printf("key %d\r\n", k);
    }
    fflush(stdout);
  }

  cash_raw_disable();
  return 0;
}
