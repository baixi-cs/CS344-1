/*******************************************************************************
** Program: keygen
** Project: CS344 Program 4 OTP
** Author: Jordan Grant (grantjo)
*******************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "encoding.h"

int main(int argc, char **argv) {
  // make sure usage is correct, print error and exit if no arg or arg no a number
  if (argc < 2 || !isdigit(argv[1][0])) {
    fprintf(stderr, "Usage: %s <num chars>\n", argv[0]);
    return 1;
  }
  // seed random generator
  srand(time(NULL));
  int num_chars = atoi(argv[1]),
      i = 0;
  // allocate space for key buffer
  char *key = malloc(sizeof(char) * (num_chars + 1));
  // fill buffer with random characters in input domain (char_list from encoding.h)
  for (; i < num_chars; i++) {
    int random = rand() % 27;
    key[i] = char_list[random];     // index into char domain
  }
  // add null terminator
  key[i] = 0;
  //  print to stdout with newline
  printf("%s\n", key);

  return 0;
}
