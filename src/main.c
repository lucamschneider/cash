#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  size_t bufsize = 0;
  char *buffer = NULL;

  printf("$ ");
  ssize_t n = getline(&buffer, &bufsize, stdin);

  if (n == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);
    } else {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }
  printf("$ read [%zd chars]: %s\n", n, buffer);

  free(buffer);
  return 0;
}
