#include "otp_lib.h"
#include "headers/encoding.h"

void encdec(char **str, char *key, int enc) {
  int size = strlen(*str),
      i = 0;
  
  for (; i < size; i++) {
    int s, k, ind;
    s = ((*str)[i] < 65) ? 26 : (*str)[i] - 65;
    k = (key[i] < 65) ? 26 : key[i] - 65;
    ind = mod((enc) ? s + k : s - k, 27);
    
    (*str)[i] = char_list[ind];
  }
  (*str)[i] = '#';

}

int mod(int num, int denom) {
  int remain = num % denom;
  return (remain < 0) ? remain + denom : remain;
}

int authClient(char *auth_str, char *read) {
  char *c;

  if ((c = strstr(read, auth_str)) == NULL)
    return -1;
  
  return 0;
}

int getSockMessage(int fd, char **buffer) {
  if (*buffer == NULL) 
    *buffer = malloc(sizeof(char) * BUFFER_START_SIZE);
  int buff_cap = BUFFER_START_SIZE,
      buff_size = 0;
  char *c;

  memset(*buffer, '\0', buff_cap);

  do {
    char *temp = calloc(BUFFER_START_SIZE + 1, sizeof(char));
    int chars_recv;
    chars_recv = recv(fd, temp, BUFFER_START_SIZE, 0);
    if (chars_recv < 0) {
      fprintf(stderr, "Error: %s\n", strerror(errno));
      return -1;
    }
    if (buff_size + chars_recv >= buff_cap) {
      buff_cap *= 3;
      resizeBuffer(buffer, buff_cap);
      //*buffer = realloc(*buffer, buff_cap);
    }

    strcat(*buffer, temp);
    buff_size += chars_recv;

    free(temp);
  } while ( (c = strchr(*buffer, '#')) == NULL);

  *c = 0;

  return buff_size;
}

int sendSockMessage(int fd, char *buff, int size) {
  int sent = 0,
      to_send = size,
      retval = 0;

  while (sent < size) {
    retval = send(fd, buff+sent, to_send, 0);
    if ( retval < 0 ) {
      fprintf(stderr, "Error: %s\n", strerror(errno));
      return -1;
    }  
    sent += retval;
    to_send -= retval;
  }
  return sent;
}

void resizeBuffer(char **buffer, int new_cap) {
  char *temp = calloc(new_cap,sizeof(char));
  memset(temp, '\0', new_cap);
  strcpy(temp, *buffer);
  free(*buffer);
  *buffer = temp;
}

int isValidChars(char* input) {
  int size = strlen(input),
      i = 0,
      j;
  for (; i < size; i++) {
     int found = 0;
     for (j = 0; j < CHAR_LEN; j++) {
        if (input[i] == char_list[j]) {
          found = 1;
          break;
        }
     }
     if (!found)
       return 0;
  }
  return 1;
}
