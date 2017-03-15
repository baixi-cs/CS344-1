CFLAGS=-Wall -g
CTEST=-g -Wall -coverage -fpic -lm

H_PATH=./headers
O_PATH=./object
SRC_PATH=./src

ARRAY_TARGET=${O_PATH}/dynamicArray.o

all: otp_enc_d otp_enc otp_dec_d otp_dec keygen

otp_enc_d: otp_enc_d.c otp_lib.c ${ARRAY_TARGET}
	gcc $^ -o $@ ${CFLAGS}
	
otp_enc: otp_enc.c otp_lib.c ${ARRAY_TARGET}
	gcc $^ -o $@ ${CFLAGS}

otp_dec_d: otp_dec_d.c otp_lib.c ${ARRAY_TARGET}
	gcc $^ -o $@ ${CFLAGS}

otp_dec: otp_dec.c otp_lib.c ${ARRAY_TARGET}
	gcc $^ -o $@ ${CFLAGS}

keygen: keygen.c 
	gcc $^ -o $@ ${CFLAGS}

${ARRAY_TARGET}: ${SRC_PATH}/dynamicArray.c ${H_PATH}/dynamicArray.h
	gcc -I${H_PATH} -c -o $@ $< ${CFLAGS}

test_array: ${SRC_PATH}/testDynArray.c ${SRC_PATH}/dynamicArray.c ${H_PATH}/dynamicArray.h
	gcc -I${H_PATH} -o $@ ${SRC_PATH}/testDynArray.c ${SRC_PATH}/dynamicArray.c ${CTEST}

clean:
	rm -f ${ARRAY_TARGET} keygen otp_enc_d otp_enc otp_dec_d otp_dec test_array *.gcda *.gcno *.gcov
