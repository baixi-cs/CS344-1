/* 	dynArrDeque.h : Dynamic Array with Stack, Queue, Deque implementation. */
#ifndef DYNAMIC_ARRAY_INCLUDED
#define DYNAMIC_ARRAY_INCLUDED 1

# define TYPE      char*
# define TEST

struct DynArr
{
  TYPE *data;     /* pointer to the data array */
  int size;       /* Number of elements in the array */
  int capacity;   /* capacity of the array */
  int beg;        /* Beginning of the 'floating' array */
};

typedef struct DynArr DynArr;

#ifdef TEST
int _physicalIndex (DynArr *v , int idx);
#endif

/* function used to compare two TYPE values to each other, define this in your compare.c file */
int compare(TYPE left, TYPE right);

/* Dynamic Array Functions */
void initDynArr(DynArr *v, int capacity);	
DynArr *createDynArr(int cap);

void freeDynArr(DynArr *v);
void deleteDynArr(DynArr *v);

int sizeDynArr(DynArr *v);

void addDynArr(DynArr *v, TYPE val, int size);
TYPE getDynArr(DynArr *v, int pos);
void putDynArr(DynArr *v, int pos, TYPE val, int size);
void swapDynArr(DynArr *v, int i, int  j);
void removeAtDynArr(DynArr *v, int idx);

/* Stack interface. */
int isEmptyDynArr(DynArr *v);
void pushDynArr(DynArr *v, TYPE val, int size);
TYPE topDynArr(DynArr *v);
void popDynArr(DynArr *v);

/* Bag Interface */	
int containsDynArr(DynArr *v, TYPE val);
void removeDynArr(DynArr *v, TYPE val);

/* Deque Interface */
void addFrontDynArr(DynArr *v, TYPE val, int size);
void removeFrontDynArr(DynArr *v);
void addBackDynArr(DynArr *v, TYPE val, int size);
void removeBackDynArr(DynArr *v);
TYPE frontDynArr(DynArr *v);
TYPE backDynArr(DynArr *v);

#endif
