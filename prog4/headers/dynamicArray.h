/* 	dynamicArray_a1.h : Dynamic Array implementation. */
#ifndef ARRAY_INCLUDED
#define ARRAY_INCLUDED 1

#ifndef __T1
#define __T1
# define T1      int
# define T1_SIZE sizeof(int)
# endif

# ifndef LT
# define LT(A, B) ((A) < (B))
# endif

# ifndef EQ
# define EQ(A, B) ((A) == (B))
# endif

struct Arr
{
  T1 *data;             /* pointer to data array */
  int size;             /* Number of elements in the array */
  int capacity;         /* capacity of the array */
};

typedef struct Arr Arr;

/* Dynamic Array Functions */
void initArr(Arr *v, int capacity);	
Arr *newArr(int cap);

void freeArr(Arr *v);
void deleteArr(Arr *v);

int sizeArr(Arr *v);

void addArr(Arr *v, T1 val);
T1 getArr(Arr *v, int pos);
void putArr(Arr *v, int pos, T1 val);
void swapArr(Arr *v, int i, int  j);
void removeAtArr(Arr *v, int idx);

/* Stack interface. */
int isEmptyArr(Arr *v);
void pushArr(Arr *v, T1 val);
T1 topArr(Arr *v);
void popArr(Arr *v);

/* Bag Interface */	
/* Note addArr is already declared above*/
int containsArr(Arr *v, T1 val);
void removeArr(Arr *v, T1 val);

#endif
