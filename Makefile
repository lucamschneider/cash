CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -fsanitize=address -std=c17

msh: src/main.c
	$(CC) $(CFLAGS) src/main.c -o $@

.PHONY: clean
clean:
	rm -f msh
	rm -f -r msh.dSYM/
