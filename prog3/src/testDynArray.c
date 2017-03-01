/* testArray.c
 * This file is used for testing the dynamicArray.c file. 
 * This test suite is by no means complete or thorough.
 * More testing is needed on your own.
 */
#include <stdio.h>
#include <stdlib.h>
#include "dynamicArray.h"


void assertTrue(int predicate, char *message) 
{
	printf("%s: ", message);
	if (predicate)
		printf("PASSED\n");
	else
		printf("FAILED\n");
}


// this main function contains some
int main(int argc, char* argv[]){

	Arr *dyn;
	dyn = newArr(2);

	printf("\n\nTesting addArr...\n");
	addArr(dyn, 3);
	addArr(dyn, 4);
	addArr(dyn, 10);
	addArr(dyn, 5);
	addArr(dyn, 6);
	
	printf("The array's content: [3,4,10,5,6]\n");
	assertTrue(EQ(getArr(dyn, 0), 3), "Test 1st element == 3");
	assertTrue(EQ(getArr(dyn, 1), 4), "Test 2nd element == 4");
	assertTrue(EQ(getArr(dyn, 2), 10), "Test 3rd element == 10");
	assertTrue(EQ(getArr(dyn, 3), 5), "Test 4th element == 5");
	assertTrue(EQ(getArr(dyn, 4), 6), "Test 5th element == 6");
	assertTrue(sizeArr(dyn) == 5, "Test size = 5");
	
	printf("\n\nTesting putArr...\nCalling putArr(dyn, 2, 7)\n");
	putArr(dyn, 2, 7); 
	printf("The array's content: [3,4,7,5,6]\n");
	assertTrue(EQ(getArr(dyn, 2), 7), "Test 3rd element == 7");
	assertTrue(sizeArr(dyn) == 5, "Test size = 5");
	
	printf("\n\nTesting swapArr...\nCalling swapArr(dyn, 2, 4)\n");
	swapArr(dyn, 2, 4);
	printf("The array's content: [3,4,6,5,7]\n");
	assertTrue(EQ(getArr(dyn, 2), 6), "Test 3rd element == 6");
	assertTrue(EQ(getArr(dyn, 4), 7), "Test 5th element == 7");
	
	printf("\n\nTesting removeAtArr...\nCalling removeAtArr(dyn, 1)\n");
	removeAtArr(dyn, 1);
	printf("The array's content: [3,6,5,7]\n");
	assertTrue(EQ(getArr(dyn, 0), 3), "Test 1st element == 3");
	assertTrue(EQ(getArr(dyn, 3), 7), "Test 4th element == 7");
	assertTrue(sizeArr(dyn) == 4, "Test size = 4");
	
	printf("\n\nTesting stack interface...\n");
	printf("The stack's content: [3,6,5,7] <- top\n");
	assertTrue(!isEmptyArr(dyn), "Testing isEmptyArr");
	assertTrue(EQ(topArr(dyn), 7), "Test topArr == 7");
	
	popArr(dyn);
	printf("Poping...\nThe stack's content: [3,6,5] <- top\n");
	assertTrue(EQ(topArr(dyn), 5), "Test topArr == 5");
	
	pushArr(dyn, 9);
	printf("Pushing 9...\nThe stack's content: [3,6,5,9] <- top\n");
	assertTrue(EQ(topArr(dyn), 9), "Test topArr == 9");
	
	printf("\n\nTesting bag interface...\n");
	printf("The bag's content: [3,6,5,9]\n");
	assertTrue(containsArr(dyn, 3), "Test containing 3");
	assertTrue(containsArr(dyn, 6), "Test containing 6");
	assertTrue(containsArr(dyn, 5), "Test containing 5");
	assertTrue(containsArr(dyn, 9), "Test containing 9");
	assertTrue(!containsArr(dyn, 7), "Test not containing 7");
	
	removeArr(dyn, 3);
	printf("Removing 3...\nThe stack's content: [6,5,9]\n");
	assertTrue(!containsArr(dyn, 3), "Test not containing 3");
	
	return 0;
}
