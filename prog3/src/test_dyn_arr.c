#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "dynamicArrayDeque.h"

int asserttrue(int condition, char *message) {
  if (!condition) {
    printf("Error: %s\n", message);
    return 1;
  }
  return 0;
}

int main (int argc, const char * argv[]) {
  srand(time(NULL));
  struct DynArr *myDeque;
  myDeque =  createDynArr(3);
  removeFrontDynArr(myDeque);
  removeBackDynArr(myDeque);
  frontDynArr(myDeque);
  backDynArr(myDeque);
  int i = 0;
  for (i = 0; i < 100; i++) {
    int random = rand() % 101;
    int chars = 5 + rand() % 30;
    int j = 0;
    char str[36];
    for (j = 0; j < chars; j++)
      str[j] = rand() % 90 + 33;
    str[j] = 0;

    if (random < 15)
      removeFrontDynArr(myDeque);   
    else if (random < 30)
      removeBackDynArr(myDeque);
    else if (random < 65) 
      addBackDynArr(myDeque, str, strlen(str));
    else
      addFrontDynArr(myDeque, str, strlen(str));

    char *func,
      *direct;
    func = backDynArr(myDeque);
    if (func != NULL) {
      direct = myDeque->data[_physicalIndex(myDeque,myDeque->size - 1)];

      printf("Back - Expecting %s and got %s\n", func,direct);
      asserttrue(strcmp(func, direct) == 0, "Back Returns Incorrect");
    }
    func = frontDynArr(myDeque);
    if (func != NULL) {
      direct = myDeque->data[myDeque->beg];

      printf("Front - Expecting %s and got %s\n", func,direct);
      asserttrue(strcmp(func, direct)==0, "Front Returns Incorrect");
    }
    int num; 
    num = (!isEmptyDynArr(myDeque)) ? rand() % myDeque->size : -1;
    func = getDynArr(myDeque, num);
    if (func != NULL && num >= 0) {
      direct = myDeque->data[_physicalIndex(myDeque,num)];

      printf("Get - Expecting %s and got %s\n", func, direct);
      asserttrue(strcmp(func, direct) == 0, "Get Returns Incorrect");
    }
    printf("\n");
  }

  printDynArr(myDeque);

  deleteDynArr(myDeque);
  return(0);

}
