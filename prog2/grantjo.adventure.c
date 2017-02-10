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
int fillRooms(char *gamedir, struct room *rooms, int *start_room); 
int indexofRoom(struct room *rooms, int count, char *val);
int indexof(char names[6][15], int count, char *val);
int contains(int *arr, int count, int val); 
int compare (const void *p1, const void *p2);

int main() {
  srand(time(NULL));

  struct room *rooms = malloc(sizeof(struct room)*NUM_ROOMS);

  FILE *inFile;
  int i = 0,
      j = 0,
      success = 0,
      current_room,
      rooms_visited[500],
      visit_count = 0;
  char buffer[50],
       *c;

  success = getGameDir("./", buffer);
  if (success == -1) {
    printf("No Room Directory!\n");
    return 1;
  }
  success = fillRooms(buffer, rooms, &current_room);
  if (success == -1) {
    printf("Error Reading Room Files!\n");
    return 1;
  }

  rooms_visited[visit_count++] = current_room;

  do { //while (rooms[current_room].room_type != end)
    printf("CURRENT LOCATION: %s\n", rooms[current_room].name);
    printf("POSSIBLE CONNECTIONS:");
    for (i = 0; i < rooms[current_room].out_count; i++) {
      if (i+1 != rooms[current_room].out_count)
        printf(" %s,", rooms[current_room].outgoing[i]);
      else
        printf(" %s.", rooms[current_room].outgoing[i]);
    }
    printf("\nWHERE TO? >");
    fgets(buffer, 50, stdin);
    c = strchr(buffer, '\n');
    *c = '\0';
    if(indexof(rooms[current_room].outgoing, rooms[current_room].out_count, buffer) < 0) 
      printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
    else {
      success = indexofRoom(rooms, NUM_ROOMS, buffer);
      if (success < 0) continue;
      current_room = success;
      rooms_visited[visit_count++] = success;
      printf("\n");
    }

  } while (rooms[current_room].room_type != end);
  
  printf("Much Success! You have made your way to the end!\n\n");
  printf("Here was your path to greatness:\nStart: %s\n", rooms[rooms_visited[0]].name);
  
  for (i = 1; i < visit_count-1; i++)
    printf("STEP %d: %s\n", i, rooms[rooms_visited[i]].name);

  printf("FINALY, STEP %d: %s\n\nTHE END\n", visit_count-1, rooms[rooms_visited[i]].name);

  free(rooms);
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

int fillRooms(char *gamedir, struct room *rooms, int *start_room) {
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
            else if (strcmp(buffer, "START_ROOM") == 0) {
              rooms[room_count].room_type = start;
              *start_room = room_count;
            }
            else
              rooms[room_count].room_type = mid;
          } 
        }
        fclose(file);
        room_count++;
      }
    }
    closedir(dir);
  }
}

int indexofRoom(struct room *rooms, int count, char *val) {
  int i = 0;
  for (; i < count; i++)
    if (strcmp(rooms[i].name, val) == 0)
      return i;
  return -1;
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
