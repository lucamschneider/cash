CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -fsanitize=address -std=c17

# catppuccin mocha palette 
THEME = 1e1e2e,cdd6f4,45475a,f38ba8,a6e3a1,f9e2af,89b4fa,f5c2e7,94e2d5,bac2de,585b70,f38ba8,a6e3a1,f9e2af,89b4fa,f5c2e7,94e2d5,a6adc8

cash: src/main.c
	$(CC) $(CFLAGS) src/main.c src/builtins.c src/prompt.c src/lineedit.c -o $@

.PHONY: demo
demo: cash
	asciinema rec demo.cast --overwrite -c "python3 demo_driver.py"
	agg --theme "$(THEME)" demo.cast demo.gif

.PHONY: clean
clean:
	rm -f cash
	rm -f -r cash.dSYM/
