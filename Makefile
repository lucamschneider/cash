CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -fsanitize=address -std=c17

cash: src/main.c
	$(CC) $(CFLAGS) src/main.c src/builtins.c src/prompt.c src/lineedit.c -o $@

.PHONY: clean
clean:
	rm -f cash
	rm -f -r cash.dSYM/
