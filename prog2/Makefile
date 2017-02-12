TEST_FLAGS = -Wall -coverage -fpic -lm -g

all: grantjo.adventure grantjo.buildrooms
	
test: tadv tbuild

grantjo.adventure: grantjo.adventure.c
	gcc -o grantjo.adventure grantjo.adventure.c  -lpthread

grantjo.buildrooms: grantjo.buildrooms.c 
	gcc -o grantjo.buildrooms grantjo.buildrooms.c

tadv: grantjo.adventure.c
	gcc -o tadv grantjo.adventure.c  $(TEST_FLAGS) -lpthread

tbuild: grantjo.buildrooms.c 
	gcc -o tbuild grantjo.buildrooms.c  $(TEST_FLAGS)

clean:
	rm -rf currentTime.txt grantjo.rooms* grantjo.buildrooms grantjo.adventure tadv tbuild *.o *.gcov *.gcda *.gcno *.so
