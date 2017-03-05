CFLAGS=-Wall -g
CTEST=-g -Wall -coverage -fpic -lm

H_PATH=./headers
O_PATH=./object
SRC_PATH=./src

DEQUE_TARGET=${O_PATH}/dynamicArrayDeque.o
ARRAY_TARGET=${O_PATH}/dynamicArray.o
FUNC_TARGET=${O_PATH}/smallsh_functions.o

all: smallsh

smallsh: smallsh.c ${FUNC_TARGET} ${DEQUE_TARGET} ${ARRAY_TARGET}
	gcc $^ -o $@ ${CFLAGS}
	
${DEQUE_TARGET}: ${SRC_PATH}/dynamicArrayDeque.c ${H_PATH}/dynamicArrayDeque.h
	gcc -I${H_PATH} -c -o $@ $< ${CFLAGS}

${ARRAY_TARGET}: ${SRC_PATH}/dynamicArray.c ${H_PATH}/dynamicArray.h
	gcc -I${H_PATH} -c -o $@ $< ${CFLAGS}

${FUNC_TARGET}: smallsh_functions.c smallsh_functions.h
	gcc -c -o $@ $< ${CFLAGS}

test_deque: ${SRC_PATH}/test_dyn_arr.c ${SRC_PATH}/dynamicArrayDeque.c ${H_PATH}/dynamicArrayDeque.h
	gcc -I${H_PATH} -o $@ ${SRC_PATH}/test_dyn_arr.c ${SRC_PATH}/dynamicArrayDeque.c ${CTEST}

test_array: ${SRC_PATH}/testDynArray.c ${SRC_PATH}/dynamicArray.c ${H_PATH}/dynamicArray.h
	gcc -I${H_PATH} -o $@ ${SRC_PATH}/testDynArray.c ${SRC_PATH}/dynamicArray.c ${CTEST}

clean:
	rm -f ${FUNC_TARGET} ${DEQUE_TARGET} ${ARRAY_TARGET} smallsh test_deque test_array junk junk2 *.gcda *.gcno *.gcov
