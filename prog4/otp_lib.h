#ifndef OTP_LIB_H
#define OTP_LIB_H

#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "headers/dynamicArray.h"

#define CONN_COUNT 5
#define BUFFER_START_SIZE 500

typedef struct t_data t_data;
typedef struct thread thread;

void encdec(char **str, char *key, int enc);
int mod(int num, int denom);
int authClient(char *auth_str, char *read);
int getSockMessage(int fd, char **buffer);
int sendSockMessage(int fd, char *to_send, int size);
void resizeBuffer(char **buffer, int new_cap);
int isValidChars(char *input);

#endif
