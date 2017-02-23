CFLAGS=-Wall
CTEST=-g -Wall -coverage -fpic -lm

H_PATH=./headers
O_PATH=./object
SRC_PATH=./src
LIB_PATH=./lib

LIB_TARGET=${LIB_PATH}/smallsh_lib.a
DEQUE_TARGET=${O_PATH}/dynamicArrayDeque.o
FUNC_TARGET=${O_PATH}/smallsh_functions.o

all: smallsh

smallsh: smallsh.c ${LIB_TARGET} ${H_PATH}/smallsh_lib.h
	gcc $< ${LIB_TARGET} -o $@ ${CFLAGS}
	
${LIB_TARGET}: ${DEQUE_TARGET} ${FUNC_TARGET}
	ar rcs $@ $^

${DEQUE_TARGET}: ${SRC_PATH}/dynamicArrayDeque.c ${H_PATH}/dynamicArrayDeque.h
	gcc -I${H_PATH} -c -o $@ $< ${CFLAGS}

${FUNC_TARGET}: ${SRC_PATH}/smallsh_functions.c ${H_PATH}/smallsh_functions.h
	gcc -I${H_PATH} -c -o $@ $< ${CFLAGS}

test_deque: ${SRC_PATH}/test_dyn_arr.c ${SRC_PATH}/dynamicArrayDeque.c ${H_PATH}/dynamicArrayDeque.h
	gcc -I${H_PATH} -o $@ ${SRC_PATH}/test_dyn_arr.c ${SRC_PATH}/dynamicArrayDeque.c ${CTEST}

clean:
	rm -f ${FUNC_TARGET} ${DEQUE_TARGET} ${LIB_TARGET} smallsh test_deque *.gcda *.gcno *.gcov
