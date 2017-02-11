#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
//#include <unistd.h>

#define MAX_OUT 6
#define MIN_OUT 3
#define NUM_ROOMS 7

struct room {
  int name;
  int outgoing[MAX_OUT];
  int out_count;
  int room_type;
};

enum room_type { start = 0, mid, end };

int getRandomName(int takenNames[], int *count);
int contains(int *arr, int count, int val); 

int main() {
  time_t t;
  srand((unsigned)time(&t));

  struct room rooms[NUM_ROOMS];

  char roomNames[10][15] = {
    "El Dorado",
    "Shangrila",
    "Thule",
    "Cockaigne",
    "Quivira",
    "Can'-Ka No Rey",
    "Valinor",
    "Camelot",
    "Atlantis",
    "Gilead"
  },
      roomTypes[3][11] = {
    "START_ROOM",
    "MID_ROOM",
    "END_ROOM"
  },
      fileBuffer[50];

  pid_t pid = getpid();
  FILE *outFile;
  int i = 0,
      j = 0,
      takenRoomNames[NUM_ROOMS],
      takenRoomCount = 0,
      endroom,
      startroom,
      connection,
      success = 0;


  for (; i < NUM_ROOMS; i++) {
	  rooms[i].name = getRandomName((int*)&takenRoomNames, &takenRoomCount);
	  rooms[i].out_count = 0;
	  rooms[i].room_type = -1;
  }

  startroom = rand() % NUM_ROOMS;
  rooms[startroom].room_type = start;

  endroom = rand() % NUM_ROOMS;
  while (endroom == startroom) { endroom = rand() % NUM_ROOMS; }
  rooms[endroom].room_type = end;

  for (i = 0; i < NUM_ROOMS; i++)
    if (i != endroom && i != startroom)
      rooms[i].room_type = mid;

  for (i = 0; i < NUM_ROOMS; i++) {
    while (rooms[i].out_count < 3) { 
      connection = 1 + rand() % MAX_OUT;
      while (connection < 3) {connection = 1 + rand() % 6;}
      connection -= rooms[i].out_count;
      for (j = 0; j < connection; j++) {
        startroom = rand() % NUM_ROOMS;
        while (contains(rooms[i].outgoing, rooms[i].out_count, rooms[startroom].name) || 
            rooms[startroom].name == rooms[i].name) {
          startroom = rand() % NUM_ROOMS;   
        }   
        rooms[i].outgoing[rooms[i].out_count++] = rooms[startroom].name;
		if (!contains(rooms[startroom].outgoing, rooms[startroom].out_count, rooms[i].name))
			rooms[startroom].outgoing[rooms[startroom].out_count++] = rooms[i].name;
      }
    }
  }

  sprintf(fileBuffer, "grantjo.rooms.%d", (int)pid);
  mkdir(fileBuffer, 0777);
  for (i = 0; i < NUM_ROOMS; i++) {
    sprintf(fileBuffer, "./grantjo.rooms.%d/%s", (int)pid, roomNames[rooms[i].name]);
    outFile = fopen(fileBuffer, "w");

    sprintf(fileBuffer, "ROOM NAME: %s\n", roomNames[rooms[i].name]);
    fputs(fileBuffer, outFile);
    for (j = 0; j < rooms[i].out_count; j++) {
      sprintf(fileBuffer, "CONNECTION %d: %s\n", j+1, roomNames[rooms[i].outgoing[j]]);
      fputs(fileBuffer, outFile);
    }
    sprintf(fileBuffer, "ROOM TYPE: %s\n", roomTypes[rooms[i].room_type]);
    fputs(fileBuffer, outFile);

    fclose(outFile);
  }
  
  
  return 0;
}



int getRandomName(int takenNames[], int *count) {
  int random,
      found,
      i;

  if (count == 0) {
    takenNames[(*count)++] = rand() % 10;
    return takenNames[0];
  }

  do {  //while (found == 0)
    random = rand() % 10;
    found = 1;

    for(i = 0 ; i < 7; i++) {
      if (takenNames[i] == random) {
        found = 0;
        break;
      }
    }

  } while (found == 0);

  takenNames[(*count)++] = random;

  return random;
}

int contains(int *arr, int count, int val) {
  int i = 0;
  for (; i < count; i++)
    if (arr[i] == val)
      return 1;
  return 0;
}


