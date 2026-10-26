#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  size_t bufsize = 0;
  char *buffer = NULL;

  printf("msh> ");
  ssize_t n = getline(&buffer, &bufsize, stdin);
  if (n == -1) {
    fprintf(stderr, "\nmsh> err: couldnt getline");
    exit(1);
  }
  printf("msh> read [%zd chars]: %s\n", n, buffer);

  free(buffer);
  return 0;
}
