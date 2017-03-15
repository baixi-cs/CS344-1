#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "headers/encoding.h"

int main(int argc, char **argv) {
  if (argc < 2 || !isdigit(argv[1][0])) {
    fprintf(stderr, "Usage: %s <num chars>\n", argv[0]);
    return 1;
  }
  srand(time(NULL));
  int num_chars = atoi(argv[1]),
      i = 0;
  char *key = malloc(sizeof(char) * (num_chars + 1));

  for (; i < num_chars; i++) {
    int random = rand() % 27;  
    key[i] = char_list[random];
  }
  key[i] = 0;

  printf("%s\n", key);

  return 0;
}
