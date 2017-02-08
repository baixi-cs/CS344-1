#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_OUT 6
#define MIN_OUT 3

struct room {
  int name;
  int outgoing[MAX_OUT];
  int out_count;
  int room_type;
};

enum room_type { start = 0, mid, end };

int getRandomName(int takenNames[], int *count);
int compare (const void *p1, const void *p2);

int main() {
  srand(time(NULL));

  struct room rooms[7];

  char roomNames[10][11] = {
    "El Dorado\0\0",
    "ShangriLa\0\0",
    "Thule\0\0\0\0\0\0",
    "Cockaigne\0\0",
    "Quivira\0\0\0\0",
    "CanKaNoRey\0",
    "Valinor\0\0\0\0",
    "Camelot\0\0\0\0",
    "Atlantis\0\0\0",
    "Gilead\0\0\0\0\0"
  },
      roomTypes[3][11] = {
    "START_ROOM\0",
    "MID_ROOM\0\0\0",
    "END_ROOM\0\0\0"
  },
      folderName[20];

  pid_t pid = getpid();
  FILE handle;
  int i = 0,
      takenRoomNames[7],
      takenRoomCount = 0,
      endroom,
      startroom,
      success = 0;


  for ( ; i < 7; i++ ) 
    rooms[i].name = getRandomName((int*)&takenRoomNames,&takenRoomCount);

  startroom = rand() % 7;
  rooms[startroom].room_type = start;

  endroom = rand() % 7;
  while (endroom != startroom) { endroom = rand() % 7; }
  rooms[endroom].room_type = end;

  for (i = 0; i < 7; i++)
    if (i != endroom && i != startroom)
      rooms[i].room_type = mid;

  sprintf(folderName, "grantjo.rooms.%d", (int)pid);
  //success = mkdir(folderName, 0755);
  
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

int compare (const void *p1, const void *p2) {
  int int1 = *(int*)p1,
      int2 = *(int*)p2;
  if ( int1 > int2 )
    return 1;
  else if ( int1 < int2 )
    return -1;

  return 0;
}
