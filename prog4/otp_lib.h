#ifndef OTP_LIB_H
#define OTP_LIB_H

#include <ctype.h>            // contains functions to text chars
#include <errno.h>            // defines errno
#include <netinet/in.h>       // network structs and constants
#include <stdio.h>            // I/O library functions
#include <stdlib.h>           // General typedef's, Constants, and lib functions
#include <string.h>           // library functions to manipulate strings
#include <sys/socket.h>       // socket types and functions
#include <sys/wait.h>         // contains wait functions
#include <unistd.h>           // contains std unix sys calls, like fork

#define CONN_COUNT 5          // max connection count
#define BUFFER_START_SIZE 500 // start size of resv buffer

/*******************************************************************************
** Function: encdec
** Paramaters: char** for string to be enc/dec
**             char* for the key buffer
**             int flag: set = encode, clear = decode
** Description: performs one time pad encoding and decoding. enc/dec is done
**              in place in the str paramater.
** Return: encoded/decoded string in str
*******************************************************************************/
void encdec(char **str, char *key, int enc);

/*******************************************************************************
** Function: mod
** Paramaters: int numerator of modulus operation
**             int denominator of modulus operation
** Description: performs true modulus operation as c mod is more of a remainder
** Return: result of num modulus denom
*******************************************************************************/
int mod(int num, int denom);

/*******************************************************************************
** Function: authClient
** Paramaters: char* authorization string
**             char* message from client to authorize connection
** Description: searches for auth_str in message.
** Return: If found 0, else -1
*******************************************************************************/
int authClient(char *auth_str, char *read);

/*******************************************************************************
** Function: getSockMessage
** Paramaters: int file descripter of socket connection
**             char** buffer to read into
** Description: reads entire message terminated with '#' sigil from socket.
** Return: -1 if error, number of bytes read if successful
*******************************************************************************/
int getSockMessage(int fd, char **buffer);

/*******************************************************************************
** Function: sendSockMessage
** Paramaters: int file descripter of socket connection
**             char** buffer to send
**             int size of buffer to send
** Description: sends size characters into socket.
** Return: -1 if error, number of bytes sent if successful
*******************************************************************************/
int sendSockMessage(int fd, char *to_send, int size);

/*******************************************************************************
** Function: resizeBuffer
** Paramaters: char** buffer to resize
**             int new capacity size
** Description: reallocates buffer to size new_cap.
**              buffer contents remain and empty bytes are set to null char
** Return: resized buffer in char**, void
*******************************************************************************/
void resizeBuffer(char **buffer, int new_cap);

/*******************************************************************************
** Function: isValidChars
** Paramaters: char* buffer to check
** Description: assures only valid characters are sent for one time pad encoding
**              and decoding
** Return: 0 for invalid string, 1 for valid string
*******************************************************************************/
int isValidChars(char *input);

#endif
