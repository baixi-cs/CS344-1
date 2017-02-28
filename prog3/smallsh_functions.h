#ifndef SMALLSH_FUNC_H
#define SMALLSH_FUNC_H

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "headers/dynamicArrayDeque.h"

#define MAX_HIST 20

struct status_flags 
{
  int status;
  int is_sig;
}

void handle_SIGINT(int signo);
void handle_SIGSTP(int signo);
// BOTH HANDLE FG and GB return pid of child
int handle_fg(DynArr *parts);
int handle_bg(DynArr *parts);
int parse_command(char *str, DynArr *parts, int *is_back);
int string_split(char *str, DynArr *deq, char delim);
int change_dir(char *path);
void get_status(int status, int is_sig);
int indexOf(char *str, char val);
int trim_string(char *out, int size, char *str);

#endif
