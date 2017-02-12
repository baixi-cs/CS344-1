#include <dirent.h>
#include <limits.h>
#include <pthread.h>
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

pthread_mutex_t locks[2];
pthread_t threads[2];

struct room {
  char name[15];
  char outgoing[6][15];
  int out_count;
  int room_type;
};

enum room_type { start = 0, mid, end };

void* doWriteTimeThread(void *close);
void* doReadTimeThread(void *close);
int getGameDir(const char *rootdir, char* buffer);
int fillRooms(char *gamedir, struct room *rooms, int *start_room); 
int indexofRoom(struct room *rooms, int count, char *val);
int indexof(char names[6][15], int count, char *val);

int main() {
  srand(time(NULL));

  struct room *rooms = malloc(sizeof(struct room)*NUM_ROOMS);

  FILE *inFile;
  int i = 0,
      j = 0,
      success = 0,
      current_room,
      rooms_visited[500],
      visit_count = 0,
      close = 0;

  char buffer[50],
       *readBuffer = NULL,
       *c;
  size_t readSize = 0;

  for (i = 0; i < 2; i++) {
    if(pthread_mutex_init(&locks[i], NULL) != 0){
      printf("Failed to initialize mutex %d\n", i+1);
      free(rooms);
      return 1;
    }
    pthread_mutex_lock(&locks[i]);
  }

  success = pthread_create(&threads[0], NULL, &doWriteTimeThread, &close);
  if (success != 0) {
    printf("Failed to create thread: %s\n", strerror(success));
    free(rooms);
    return 1;
  }

  success = pthread_create(&threads[1], NULL, &doReadTimeThread, &close);
  if (success != 0) {
    printf("Failed to create thread: %s\n", strerror(success));
    free(rooms);
    return 1;
  }

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
    if (success != INT_MAX) {
      printf("CURRENT LOCATION: %s\n", rooms[current_room].name);
      printf("POSSIBLE CONNECTIONS:");
      for (i = 0; i < rooms[current_room].out_count; i++) {
        if (i+1 != rooms[current_room].out_count)
          printf(" %s,", rooms[current_room].outgoing[i]);
        else
          printf(" %s.", rooms[current_room].outgoing[i]);
      }
    }
    printf("\nWHERE TO? >");
    success = getline(&readBuffer, &readSize, stdin);
    c = strchr(readBuffer, '\n');
    *c = '\0';
    if (strcmp(readBuffer, "time") == 0) {
      pthread_mutex_unlock(&locks[0]);
      usleep(50);
      pthread_mutex_lock(&locks[0]);
      pthread_mutex_unlock(&locks[1]);
      usleep(50);
      pthread_mutex_lock(&locks[1]);
      success = INT_MAX;
    }
    else if(indexof(rooms[current_room].outgoing, rooms[current_room].out_count, readBuffer) < 0) 
      printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
    else {
      success = indexofRoom(rooms, NUM_ROOMS, readBuffer);
      if (success < 0) continue;
      current_room = success;
      rooms_visited[visit_count++] = success;
      printf("\n");
    }
    free(readBuffer);
    readBuffer = NULL;
    readSize = 0;

  } while (rooms[current_room].room_type != end);

  printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
  printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", visit_count-1);

  for (i = 1; i < visit_count; i++)
    printf("%s\n", rooms[rooms_visited[i]].name);

  close = 1;
  pthread_mutex_unlock(&locks[0]);
  pthread_mutex_unlock(&locks[1]);

  for (i = 0; i < 2; i++)
    if (pthread_join(threads[i], NULL) != 0)
      perror("Failed Thread Join\n");

  pthread_mutex_destroy(&locks[0]);
  pthread_mutex_destroy(&locks[1]);

  free(rooms);
  return 0;
}

void* doWriteTimeThread(void *close) {
  int *i_close = (int*)close;
  char buffer[50];
  time_t rawtime;
  struct tm *timeinfo;
  FILE *timeFile;
  while (!*i_close) {
    pthread_mutex_lock(&locks[0]);
    if (*i_close)
      break;
    timeFile = fopen("./currentTime.txt", "w");
    if (timeFile == NULL) {
      perror("Error\n");
    } else {
      time ( &rawtime );
      timeinfo = localtime ( &rawtime );
      strftime(buffer, 50, "%I:%M%p, %A, %B %d, %Y", timeinfo);

      buffer[5] = tolower(buffer[5]);
      buffer[6] = tolower(buffer[6]);

      if (timeinfo->tm_hour < 10 || (timeinfo->tm_hour > 12 && timeinfo->tm_hour < 22))
        memmove(buffer, buffer+1, strlen(buffer));

      fputs(buffer, timeFile);
      fclose(timeFile);
    }
    pthread_mutex_unlock(&locks[0]);
    usleep(50);
  }
 return NULL; 
}

void* doReadTimeThread(void *close) {
  int *i_close = (int*) close;
  char buffer[50];
  FILE *timeFile;

  while (!*i_close) {
    pthread_mutex_lock(&locks[1]);
    if (*i_close)
      break;
    timeFile = fopen("./currentTime.txt", "r");
    if (timeFile == NULL) {
      perror("Error\n");
    } else {

      fgets(buffer, 50, timeFile);

      printf("\n%s\n", buffer);

      fclose(timeFile);
    }

    pthread_mutex_unlock(&locks[1]);
    usleep(50);
  } 
  return NULL:
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


