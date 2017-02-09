#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_OUT 6
#define MIN_OUT 3
#define NUM_ROOMS 7

struct room {
  char name[15];
  char outgoing[6][15];
  int out_count;
  int room_type;
};

enum room_type { start = 0, mid, end };

int getGameDir(const char *rootdir, char* buffer);
int fillRooms(char *gamedir, struct room *rooms); 
int indexof(char names[10][15], int count, char *val);
int contains(int *arr, int count, int val); 
int compare (const void *p1, const void *p2);

int main() {
  srand(time(NULL));

  struct room *rooms = malloc(sizeof(struct room)*NUM_ROOMS);

  FILE *inFile;
  int i = 0,
      j = 0,
      success = 0;
  char buffer[25];

  success = getGameDir("./", buffer);
  if (success == -1) {
    printf("No Room Directory!\n");
    return 1;
  }
  fillRooms(buffer, rooms);
  for (i = 0; i < NUM_ROOMS; i++) {
    printf("ROOM_NAME: %s\n",rooms[i].name);
    for (j = 0; j < rooms[i].out_count; j++) {
      printf("CONNECTION %d: %s\n", j+1, rooms[i].outgoing[j]);
    }
    printf("%d\n", rooms[i].room_type);
  }
  return 0;
}

int getGameDir(const char *rootdir, char *buffer) {
  int i = -1,
      j = 0;
  struct stat dirStat;
  DIR *dir;
  struct dirent *dirEnt;

  // open running directory 
  dir = opendir(rootdir);
  if (dir != NULL) {
    while (dirEnt = readdir(dir)) {
      if (strncmp(dirEnt->d_name, "grantjo.room", 12) == 0) {
        stat(dirEnt->d_name, &dirStat);
        j = (int)dirStat.st_mtime;
        if (j > i) { 
          strcpy(buffer, dirEnt->d_name); 
          i = j; 
        }

      }
    }
    closedir(dir); 
  }  
  return i;
}

int fillRooms(char *gamedir, struct room *rooms) {
  struct dirent *dirEnt;
  DIR *dir;
  FILE *file;
  char buffer[100];
  char *c;
  int i = 0,
      room_count = 0;
  
  for (; i < NUM_ROOMS; i++)
    rooms[i].out_count = 0;
  dir = opendir(gamedir);
  if (dir != NULL) {
    while (dirEnt = readdir(dir)) {
      if (strcmp(dirEnt->d_name, ".") != 0 &&
          strcmp(dirEnt->d_name, "..") != 0){
        sprintf(buffer, "./%s/%s", gamedir, dirEnt->d_name);
        file = fopen(buffer, "r");
        if (file == NULL ) { perror("Error\n"); return -1;}
        while(fgets(buffer, 100, file)!=NULL) {
          // remove newline from fgets
          c = strchr(buffer, '\n');
          *c = '\0';
          // get pointer to : char
          c = strchr(buffer, ':');
          // get index of colon by taking the offset from buffer
          i = c - buffer;
          // capture the room name
          if (strncmp(buffer, "ROOM NAME", i) == 0) {
            strcpy(rooms[room_count].name, c+2);
          }else if (strncmp(buffer, "CONNECTION", i-2) == 0) {
            strcpy(rooms[room_count].outgoing[rooms[room_count].out_count++], c+2);
          } else if (strncmp(buffer, "ROOM TYPE", i) == 0) {
            strcpy(buffer, c+2);
            if (strcmp(buffer, "END_ROOM")==0)
              rooms[room_count].room_type = end;
            else if (strcmp(buffer, "START_ROOM") == 0)
              rooms[room_count].room_type = start;
            else
              rooms[room_count].room_type = mid;
          } 
        }
        room_count++;
      }
    }
  }
}

int indexof(char names[6][15], int count, char *val) {
  int i = 0;
  for (; i < count; i++)
    if (strcmp(names[i], val) == 0)
      return i;
  return -1;
}

int contains(int *arr, int count, int val) {
  int i = 0;
  for (; i < count; i++)
    if (arr[i] == val)
      return 1;
  return 0;
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
