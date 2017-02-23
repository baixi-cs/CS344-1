#include "headers/smallsh_lib.h"

int main() {
  const char symbols[3] = {'<', '>', '&'};
  DynArr *history = createDynArr(20);
  printf("%c %c %c\n", symbols[0], symbols[1], symbols[2]);
  deleteDynArr(history);
  return 0;
}
