CFLAGS = -Wall
COVFLAG = -coverage -fpic -lm

all: grantjo.adventure grantjo.buildrooms
	

grantjo.adventure: grantjo.adventure.c
	gcc -o grantjo.adventure grantjo.adventure.c -g  $(CFLAGS) -lpthread

grantjo.buildrooms: grantjo.buildrooms.c 
	gcc -o grantjo.buildrooms grantjo.buildrooms.c -g  $(CFLAGS)

grantjo.adventure-cov: grantjo.adventure.c
	gcc -o grantjo.adventure-cov grantjo.adventure.c -g  $(CFLAGS) $(COFLAGS) -lpthread

grantjo.buildrooms-cov: grantjo.buildrooms.c 
	gcc -o grantjo.buildrooms-cov grantjo.buildrooms.c -g  $(CFLAGS) $(COFLAGS)


all: playdom player testDrawCard testCardBuy badTestDrawCard testGetCost testGetWinners unittest4

clean:
	rm -rf grantjo.rooms* grantjo.buildrooms grantjo.adventure grantjo.adventure-cov grantjo.buildrooms-cov *.o *.gcov *.gcda *.gcno *.so
