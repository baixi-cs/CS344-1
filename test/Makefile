CFLAGS=-Wall -g
CTEST=-g -Wall -coverage -fpic -lm


all: otp_enc_d otp_enc otp_dec_d otp_dec keygen

otp_enc_d: otp_enc_d.c otp_lib.c
	gcc $^ -o $@ ${CFLAGS}
	
otp_enc: otp_enc.c otp_lib.c 
	gcc $^ -o $@ ${CFLAGS}

otp_dec_d: otp_dec_d.c otp_lib.c 
	gcc $^ -o $@ ${CFLAGS}

otp_dec: otp_dec.c otp_lib.c 
	gcc $^ -o $@ ${CFLAGS}

keygen: keygen.c 
	gcc $^ -o $@ ${CFLAGS}

clean:
	rm -f  keygen otp_enc_d otp_enc otp_dec_d otp_dec results *.gcda *.gcno *.gcov
