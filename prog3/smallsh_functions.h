#ifndef SMALLSH_FUNC_H
#define SMALLSH_FUNC_H

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include "./headers/dynamicArrayDeque.h"
#include "./headers/dynamicArray.h"

struct status_flags 
{
  int status;
  int is_sig;
};


// BOTH HANDLE FG and GB return pid of child
pid_t handle_fg(DynArr *parts, void (*handle_int)(int));
pid_t handle_bg(DynArr *parts);
int redirect_in(DynArr *parts, int default_null);
int redirect_out(DynArr *parts, int default_null);
char** fill_exec_args(DynArr *parts);
void free_args_at_exit(int status, void *args);
struct status_flags get_exit_sig(int child_exit);
int parse_command(char *str, DynArr *parts, int *is_back);
int string_split(char *str, DynArr *deq, char delim);
int num_digits(int pid);
int change_dir(char *path);
void exit_kill(Arr *pids);
void get_status(int status, int is_sig);
int indexOf(char *str, char val);
int trim_string(char *out, int size, char *str);
#endif
