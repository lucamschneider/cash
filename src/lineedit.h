#ifndef CASH_LINEEDIT_H
#define CASH_LINEEDIT_H

enum {
  KEY_LEFT = 1000,
  KEY_RIGHT,
  KEY_UP,
  KEY_DOWN,
};

int cash_raw_enable(void);
int cash_raw_disable(void);
int read_key(void);

#endif // CASH_LINEEDIT_H
