// include otp library header file
#include "otp_lib.h"
// include domain of chars for encryption in char_list array
#include "encoding.h"

/*******************************************************************************
** Function: encdec
** Paramaters: char** for string to be enc/dec
**             char* for the key buffer
**             int flag: set = encode, clear = decode
** Description: performs one time pad encoding and decoding. enc/dec is done
**              in place in the str paramater.
** Return: encoded/decoded string in str
*******************************************************************************/
void encdec(char **str, char *key, int enc) {
  // get size of plaintext/cipher
  int size = strlen(*str),
      i = 0;
  // loop over each char in str
  for (; i < size; i++) {
    int s, k, ind;
    // if char is space assign 26, else subtract 'A' for index in char_list
    s = ((*str)[i] < 'A') ? 26 : (*str)[i] - 'A';
    k = (key[i] < 'A') ? 26 : key[i] - 'A';
    // if enc sum plaintext char and key char
    // else, subtract key char from cipher char
    // modulus result from 27 to get index in char_list
    ind = mod((enc) ? s + k : s - k, 27);
    // assign encoded/decoded char to str[i]
    (*str)[i] = char_list[ind];
  }
  // add sigil to the end
  (*str)[i] = '#';
}

/*******************************************************************************
** Function: mod
** Paramaters: int numerator of modulus operation
**             int denominator of modulus operation
** Description: performs true modulus operation as c mod is more of a remainder
** Return: result of num modulus denom
*******************************************************************************/
int mod(int num, int denom) {
  // get remainder
  int remain = num % denom;
  // if remainder < 0 add it back to denom, else return remainder
  return (remain < 0) ? remain + denom : remain;
}

/*******************************************************************************
** Function: authClient
** Paramaters: char* authorization string
**             char* message from client to authorize connection
** Description: searches for auth_str in message.
** Return: If found 0, else -1
*******************************************************************************/
int authClient(char *auth_str, char *read) {
  char *c;
  // search for auth_str in read. return -1 if not found
  if ((c = strstr(read, auth_str)) == NULL)
    return -1;
  return 0;
}

/*******************************************************************************
** Function: getSockMessage
** Paramaters: int file descripter of socket connection
**             char** buffer to read into
** Description: reads entire message terminated with '#' sigil from socket.
** Return: -1 if error, number of bytes read if successful
*******************************************************************************/
int getSockMessage(int fd, char **buffer) {
  // if buffer is NULL allocate with default size and initialize all to null
  if (*buffer == NULL)
    *buffer = calloc(BUFFER_START_SIZE, sizeof(char));
  // set initial buffer capacity and size
  int buff_cap = BUFFER_START_SIZE,
      buff_size = 0;
  // char pointer for searching buffer
  char *c;
  // loop until full message is read
  do {  // while ( (c = strchr(*buffer, '#')) == NULL);
    // allocate buffer for recv
    char *temp = calloc(BUFFER_START_SIZE + 1, sizeof(char));
    int chars_recv;
    // read message from buffer
    chars_recv = recv(fd, temp, BUFFER_START_SIZE, 0);
    // if recv error print message and return -1
    if (chars_recv < 0) {
      fprintf(stderr, "Error: %s\n", strerror(errno));
      return -1;
    }
    // if buffer doesnt have enough capacity resize and update buff_cap
    if (buff_size + chars_recv >= buff_cap) {
      buff_cap *= 3;
      resizeBuffer(buffer, buff_cap);
    }
    // add bytes read to buffer and update buff_size
    strcat(*buffer, temp);
    buff_size += chars_recv;
    // free temp buffer
    free(temp);
    // loop until message termination sigil is found
  } while ( (c = strchr(*buffer, '#')) == NULL);
  // replace sigil with null terminator
  *c = 0;
  // return size of buffer
  return buff_size;
}

/*******************************************************************************
** Function: sendSockMessage
** Paramaters: int file descripter of socket connection
**             char** buffer to send
**             int size of buffer to send
** Description: sends size characters into socket.
** Return: -1 if error, number of bytes sent if successful
*******************************************************************************/
int sendSockMessage(int fd, char *buff, int size) {
  int sent = 0,         // # bytes sent
      to_send = size,   // # bytes yet to send
      retval = 0;       // return value from send()
  // loop until size bytes are sent
  while (sent < size) {
    // send remaining bytes to socket
    retval = send(fd, buff+sent, to_send, 0);
    // if there is a send error print message and return
    if ( retval < 0 ) {
      fprintf(stderr, "Error: %s\n", strerror(errno));
      return -1;
    }
    // update number of bytes sent and number of bytes left to send
    sent += retval;
    to_send -= retval;
  }
  // return sent
  return sent;
}

/*******************************************************************************
** Function: resizeBuffer
** Paramaters: char** buffer to resize
**             int new capacity size
** Description: reallocates buffer to size new_cap.
**              buffer contents remain and empty bytes are set to null char
** Return: resized buffer in char**, void
*******************************************************************************/
void resizeBuffer(char **buffer, int new_cap) {
  // allocate a buffer of size new_cap full of null chars
  char *temp = calloc(new_cap,sizeof(char));
  // copy buffer into temp
  strcpy(temp, *buffer);
  // free old buffer
  free(*buffer);
  // assign new buffer to old pointer
  *buffer = temp;
}

/*******************************************************************************
** Function: isValidChars
** Paramaters: char* buffer to check
** Description: assures only valid characters are sent for one time pad encoding
**              and decoding
** Return: 0 for invalid string, 1 for valid string
*******************************************************************************/
int isValidChars(char* input) {
  int size = strlen(input),
      i = 0,
      j;
  // loop over input
  for (; i < size; i++) {
    // flag for valid char found
    int found = 0;
    // loop over char_list
    for (j = 0; j < CHAR_LEN; j++) {
      // if char from input is found in char list, set found and break
      if (input[i] == char_list[j]) {
        found = 1;
        break;
      }
    }
    // if found is not set, invalid string return 0
    if (!found)
      return 0;
  }
  // return 1 for valid
  return 1;
}
