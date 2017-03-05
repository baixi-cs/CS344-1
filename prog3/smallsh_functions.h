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

// include headers for custom Dynamic Array ADTs
// Deque, Stack, and Bag inteface used to store lists of char* strings
#include "./headers/dynamicArrayDeque.h"
// Stack and Bag inteface for arrays of integers (used for pid list)
#include "./headers/dynamicArray.h"

// struct used to store information on how a process terminated
struct status_flags
{
  int status;     // value from terminated process
  int is_sig;     // flag to represent if it was a signal or exit value
};

/*******************************************************************************
** Function:    handle_fg
** Arguments:   DynArr of command parts(see src/dynamicArrayDeque.c),
**              pointer to SIGINT handler
** Description: forks a child process, registers childs signal handlers,
**              performs necessary redirection of stdin and stdout, and calls
**              execvp to run command. Handles execvp error if it returns
** Pre-conditions:  command has been tokenized into DynArr and is not built in
** Post-conditions: child process replaced by command process or execvp error
**                  is handled. pid of child is returned
*******************************************************************************/
pid_t handle_fg(DynArr *parts, void (*handle_int)(int));

/*******************************************************************************
** Function:    handle_bg
** Arguments:   DynArr of command parts(see src/dynamicArrayDeque.c)
** Description: forks a child process, registers childs signal handlers,
**              performs necessary redirection of stdin/out or redirects to
**              /dev/null, and calls execvp to run command. Handles execvp error
**              if it returns
** Pre-conditions:  command has been tokenized into DynArr and is not built in
** Post-conditions: child process replaced by command process or execvp error
**                  is handled. pid of child is returned
*******************************************************************************/
pid_t handle_bg(DynArr *parts);

/*******************************************************************************
** Function:    redirect_in
** Arguments:   DynArr of command parts(see src/dynamicArrayDeque.c),
**              flag to determine if stdin should point to null if no direct
**              redirect specified.
** Description: checks if input should be redirected. If so it opens the file
**              and performs the redirection, If not and default_null is set
**              it opens /dev/null and redirects, else no redirection is made
** Pre-conditions:  command has been tokenized into DynArr and is not built in
** Post-conditions: child process stdin is redirected to the specified file,
**                  /dev/null, or remains unchanged
*******************************************************************************/
int redirect_in(DynArr *parts, int default_null);

/*******************************************************************************
** Function:    redirect_out
** Arguments:   DynArr of command parts(see src/dynamicArrayDeque.c),
**              flag to determine if stdout should point to null if no direct
**              redirect specified.
** Description: checks if output should be redirected. If so it opens the file
**              and performs the redirection, If not and default_null is set
**              it opens /dev/null and redirects, else no redirection is made
** Pre-conditions:  command has been tokenized into DynArr and is not built in
** Post-conditions: child process stdout is redirected to the specified file,
**                  /dev/null, or remains unchanged
*******************************************************************************/
int redirect_out(DynArr *parts, int default_null);

/*******************************************************************************
** Function:    get_exit_sig
** Arguments:   integer value of child exit status
** Description: checks if input should be redirected. If so it opens the file
**              and performs the redirection, If not and default_null is set
**              it opens /dev/null and redirects, else no redirection is made
** Pre-conditions:  waitpid returns non-zero
** Post-conditions: returns exit status or terminating signal
**                  in struct status_flags
*******************************************************************************/
struct status_flags get_exit_sig(int child_exit);

/*******************************************************************************
** Function:    parse_command
** Arguments:   buffer containing user input,
**              DynArr into which to tokenize the command,
**              int flag to represent if command should be background
** Description: splits buffer by word (space) into DynArr and checks if the last
**              word is '&'. If so, it sets the is_back flag and removes &
** Pre-conditions:  command is not blank or a comment
** Post-conditions: command exists in DynArr, less bg signal if present
*******************************************************************************/
int parse_command(char *str, DynArr *parts, int *is_back);

/*******************************************************************************
** Function:    string_split
** Arguments:   buffer containing user input,
**              DynArr into which to tokenize the command,
**              char to use as delimiter
** Description: splits buffer by delim into DynArr
** Pre-conditions:  command is not blank or a comment
** Post-conditions: command exists in DynArr split by delim
*******************************************************************************/
int string_split(char *str, DynArr *deq, char delim);

/*******************************************************************************
** Function:    expand$$
** Arguments:   buffer containing word from command,
**              index of the first $ in $$,
**              pointer to buffer capacity (size_t)
** Description: expands $$ anywhere in string to pid of the process
** Pre-conditions:  $$ exists in string and the index is known
** Post-conditions: $$ expanded to process pid
*******************************************************************************/
size_t expand$$(char* str, int index, size_t *str_cap);

/*******************************************************************************
** Function:    num_digits
** Arguments:   int representing process id
** Description: expands $$ anywhere in string to pid of the process id
*******************************************************************************/
int num_digits(int pid);

/*******************************************************************************
** Function:    change_dir
** Arguments:   path of directory to change to (relative path accepted)
** Description: changes current working directory to that specified by path or
**              changes to $HOME if no path is specified
** Pre-conditions:  path is valid or set to NULL
** Post-conditions: current working directory is path or $HOME
*******************************************************************************/
int change_dir(char *path);

/*******************************************************************************
** Function:    exit_kill
** Arguments:   kills all bg processes on exit command
** Description: raises SIGKILL on each bg process in pids and waits for its
**              termination.
** Pre-conditions:  exit command has been called
** Post-conditions: all bg processes of smallsh are terminated
*******************************************************************************/
void exit_kill(Arr *pids);

/*******************************************************************************
** Function:    get_status
** Arguments:   int of status returned, int flag set if the command was
**              terminated by a signal
** Description: prints an informative message about exit value or terminating
**              signal of last forground command
** Pre-conditions:  foreground command was called or both args set to -5
** Post-conditions: message printed to console
*******************************************************************************/
void get_status(int status, int is_sig);

/*******************************************************************************
** Function:    trim_string
** Arguments:   char* buffer to hold trimmed string,
**              int size of string to trim,
**              char* string to trim
** Description: Trims preceding and trailing whitespace. returns -1 if string
**              was empty.
** Pre-conditions:  str is not NULL
** Post-conditions: string is trimmed
*******************************************************************************/
int trim_string(char *out, int size, char *str);
#endif
