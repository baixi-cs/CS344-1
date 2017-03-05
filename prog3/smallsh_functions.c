/*******************************************************************************
** smallsh_functions.c
** Description: primary function library for smallsh
** Author: Jordan Grant (grantjo)
*******************************************************************************/
#include "smallsh_functions.h"

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
pid_t handle_fg(DynArr *parts, void (*handle_int)(int)) {
  pid_t spawn_pid = -5;
  char **args = NULL;     // char** for execvp
  // declare signal handler structs
  struct sigaction SIGINT_action = {{0}},
                   SIGTSTP_action = {{0}};
  int success,
      i;
  // fork off copy of smallsh
  spawn_pid = fork();

  switch(spawn_pid) {
    // in case of fatal error print to user and exit
    case -1:
      perror("Fatal Error");
      exit(1);
      break;
    case 0:
      // initialize SIGINT handler for child
      SIGINT_action.sa_handler = handle_int;
      // fill sa_mask to block all subsequent signals until SIGINT has returned
      sigfillset(&SIGINT_action.sa_mask);
      // set SA_RESETHAND so the handler will only occur once
      SIGINT_action.sa_flags = SA_RESETHAND;
      // IGNORE SIGTSTP
      SIGTSTP_action.sa_handler = SIG_IGN;
      // register handlers with kernel
      sigaction(SIGINT, &SIGINT_action, NULL);
      sigaction(SIGTSTP, &SIGTSTP_action, NULL);
      // addempt to redirect input, do not default to /dev/null
      success = redirect_in(parts, 0);
      // on error exit process with error
      if (success == -1)
        exit(1);
      // attempt to redirect output, do not default to /dev/null
      success = redirect_out(parts, 0);
      // on error exit process with error
      if (success == -1)
        exit(1);
      // fill args array with command in DynArr for execvp call
      args = malloc(sizeof(char*) * sizeDynArr(parts));
      for (i = 0; i < sizeDynArr(parts); i++) {
        // get current arg from DynArr
        char *buffer = getDynArr(parts, i);
        // allocate buffer in args
        args[i] = malloc(sizeof(char) * strlen(buffer));
        // copy command into args array
        strcpy(args[i], buffer);
      }
      // set last value to NULL as expected by execvp
      args[i] = NULL;
      // call execvp to replace copy of smallsh with command.
      // file redirects persist
      execvp(args[0], args);
      // print informative message and exit if execvp failed
      fprintf(stderr, "%s: %s\n", args[0], strerror(errno));
      exit(1);
      break;
    default:
      // do nothing in parent
      break;
  }
  // return childs pid
  return spawn_pid;
}

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
int handle_bg(DynArr *parts) {
  pid_t spawn_pid = -5;
  char **args = NULL;
  // declare signal handler structs
  struct sigaction SIGINT_action = {{0}},
                   SIGTSTP_action = {{0}};
  int success,
      i;
  // fork off child process
  spawn_pid = fork();

  switch(spawn_pid) {
    // case fatal error, print error and exit
    case -1:
      perror("Fatal Error");
      exit(1);
      break;
    // Handle child process
    case 0:
      // set child to ignore SIGINT and SIGTSTP
      SIGINT_action.sa_handler = SIG_IGN;
      SIGTSTP_action.sa_handler = SIG_IGN;
      // register handlers with kernel
      sigaction(SIGINT, &SIGINT_action, NULL);
      sigaction(SIGTSTP, &SIGTSTP_action, NULL);
      // redirect stdin
      success = redirect_in(parts, 1);
      // exit if failed
      if (success == -1)
        exit(1);
      // redirect stdout
      success = redirect_out(parts, 1);
      // exit if failed
      if (success == -1)
        exit(1);
      // allocate char** array for execvp
      args = malloc(sizeof(char*) * sizeDynArr(parts));
      // loop over command parts
      for (i = 0; i < sizeDynArr(parts); i++) {
        // get current command arg
        char *buffer = getDynArr(parts, i);
        // allocate buffer in char**
        args[i] = malloc(sizeof(char) * strlen(buffer));
        // copy command arg into args array
        strcpy(args[i], buffer);
      }
      // set last value to NULL
      args[i] = NULL;
      // call execvp on child to replace copy of smallsh
      // redirects persist
      execvp(args[0], args);
      // if execvp returns exit to kill child
      exit(1);
      break;
    default:
      // do nothing in parent
      break;
  }
  // return childs pid
  return spawn_pid;
}

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
int redirect_in(DynArr *parts, int default_null) {
  // check if input redirect exists in command, if so get index
  int index = indexOfDynArr(parts, "<");
  int file_desc;
  // if output redirect is found and a file was specified
  if (index != -1 && index < sizeDynArr(parts)-1) {
    // open file readonly
    file_desc = open(getDynArr(parts,index + 1), O_RDONLY);
    if (file_desc == -1) {
      // if open failed print informative message
      fprintf(stderr, "cannot open %s for input: %s\n", getDynArr(parts, index+1), strerror(errno));
      return -1;                            // return error
    }
    // redirect stdin to file and remove stdin redirect symbols from command
    dup2(file_desc, 0);
    removeAtDynArr(parts, index);
    removeAtDynArr(parts, index);
  }
  // if stdin was not specified or file was left out
  else {
    // remove redirect symbol from command if its there
    if (index != -1)
      removeAtDynArr(parts,index);
    // redirect to /dev/null if default_null is set
    if (default_null) {
      // open /dev/null for reading
      file_desc = open("/dev/null", O_RDONLY);
      if (file_desc == -1) {
        // print informative message if failed
        fprintf(stderr, "cannot open %s for input: %s\n", "/dev/null", strerror(errno));
        return -1;                          // return error
      }
      // redirect stdin to /dev/null
      dup2(file_desc, 0);
    }
  }
  return 0;                                 // return success
}

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
int redirect_out(DynArr *parts, int default_null) {
  // check if output redirect exists in command, if so get index
  int index = indexOfDynArr(parts, ">");
  int file_desc;
  // if output redirect is found and a file was specified
  if (index != -1 && index < sizeDynArr(parts)-1) {
    // attempt to open file for writing ( create if doesnt exist and truncate if it does)
    file_desc = open(getDynArr(parts, index + 1), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file_desc == -1) {
      // if failed to open print informative message
      fprintf(stderr, "cannot open %s for output: %s\n", getDynArr(parts, index+1), strerror(errno));
      return -1;                    // return error
    }
    // if opened, redirect stdout to file and remove redirect out symbols from command
    dup2(file_desc, 1);
    removeAtDynArr(parts, index);
    removeAtDynArr(parts, index);
  }
  // if output redirect was not specified or file was left out
   else {
    // remove redirect symbol from command
    if (index != -1)
      removeAtDynArr(parts,index);
    // if default_null flag was set
    if (default_null) {
      // open dev_null for writing
      file_desc = open("/dev/null", O_WRONLY);
      if (file_desc == -1) {
        // print informative error if failed to open /dev/null
        fprintf(stderr, "cannot open %s for output: %s\n", "/dev/null", strerror(errno));
        return -1;                  // return error
      }
      // redirect stdout to /dev/null
      dup2(file_desc, 1);
    }
  }
  return 0;                         // return success
}

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
struct status_flags get_exit_sig(int child_exit) {
  struct status_flags status_flag;
  // test if process exited
  if (WIFEXITED(child_exit) != 0) {
    //If so, clear is_sig flag
    status_flag.is_sig = 0;
    // set status with WEXITSTATUS macro
    status_flag.status = WEXITSTATUS(child_exit);
    return status_flag;       // return exit status
  }
  // If not, process terminated by signal so set is_sig flag
  status_flag.is_sig = 1;
  // set signal using WTERMSIG macro
  status_flag.status = WTERMSIG(child_exit);
  return status_flag;         // return terminating signal
}

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
int parse_command(char *str, DynArr *parts, int *is_back) {
  // clear is_back
  *is_back = 0;
  int success;
  // call string_split on str and test for success
  // fails if str cannot be opened as mem stream
  success = string_split(str, parts, ' ');
  if (success == -1)
    return -1;          // return error
  // if last word is & set is_back and remove &
  if (strcmp(backDynArr(parts), "&") == 0) {
    *is_back = 1;
    removeBackDynArr(parts);
  }
  return 0;             // return success
}
/*******************************************************************************
** Function:    string_split
** Arguments:   buffer containing user input,
**              DynArr into which to tokenize the command,
**              char to use as delimiter
** Description: splits buffer by delim into DynArr
** Pre-conditions:  command is not blank or a comment
** Post-conditions: command exists in DynArr split by delim
*******************************************************************************/
int string_split(char *str, DynArr *deq, char delim) {
  // open buffer as a FILE stream
  FILE *stream = fmemopen(str, strlen(str), "r");
  // ensure stream is opened successfully
  if (stream == NULL)
    return -1;

  char *buffer = NULL,        // set getline buffer to NULL
       *c;                    // char pointer for strchr return
  // size_t variables for getdelim call memory capacity and size
  size_t buffer_cap = 0,
         buffer_size;

  // loop through stream by delim and store each word into buffer
  while ((buffer_size = getdelim(&buffer, &buffer_cap, delim, stream)) != -1) {
    // search for ending space and make null terminator
    c = strchr(buffer, ' ');
    if (c != NULL)
      *c = 0;
    // if space isnt found add 1 to buffsize because getdelim doesnt count null
    // terminator
    else
      buffer_size += 1;
    // if buffer contains '$$' expand to smallsh's pid
    while ((c = strstr(buffer, "$$")) != NULL) {
      // index of first $ is the the offset between c and buffer
      int index = c - buffer;
      // expand the $$ to represent pid
      buffer_size = expand$$(buffer, index, &buffer_cap);
    }
    // add the word to the DynArr
    addBackDynArr(deq, buffer, buffer_size);
    // free buffer and set to NULL to force getdelim to allocate space for next
    // word
    free(buffer);
    buffer = NULL;
    buffer_cap = 0;
  }
  // free the buffer allocated for the failing getdelim call
  free(buffer);
  // close the memstream
  fclose(stream);

  return 0;   //return success
}

/*******************************************************************************
** Function:    expand$$
** Arguments:   buffer containing word from command,
**              index of the first $ in $$,
**              pointer to buffer capacity (size_t)
** Description: expands $$ anywhere in string to pid of the process
** Pre-conditions:  $$ exists in string and the index is known
** Post-conditions: $$ expanded to process pid
*******************************************************************************/
size_t expand$$(char* str, int index, size_t *str_cap) {
  int shell_pid = (int)getpid(),          // get pid
      pid_len = num_digits(shell_pid),    // get # digits in pid
      buffer_size = strlen(str),          // store original size of str
      resize = 0;                         // flag to determine if str needs resizing
  DynArr *buffers = createDynArr(2);      // DynArr to hold non expanded segments
  // replace $$ with "\0\0"
  *(str + index) = 0;
  *(str + index + 1) = 0;
  // determine if resizing will be necessary
  if (*str_cap <= buffer_size + pid_len) {
    // if so, set new str_cap and set resize flag
    resize = 1;
    *str_cap = (strlen(str) + pid_len) * sizeof(char);\
  }
  // if $$ begins the string
  if (index == 0) {
    // add the segment after the $$ to the array for temp storage
    addBackDynArr(buffers, str + index + 2, strlen(str + index + 2));
    // resize str if necessary
    if (resize) {
      free(str);
      str = malloc(*str_cap);
    }
    // replace the beginning with the pid
    sprintf(str, "%d%s", shell_pid, backDynArr(buffers));
  }
  // if $$ is at the end of str
  else if (index + 2 == buffer_size) {
    // add the beginning section of str to DynArr for temp storage
    addBackDynArr(buffers, str, strlen(str));
    // resize str if necessary
    if (resize) {
      free(str);
      str = malloc(*str_cap);
    }
    // put the pid at the end
    sprintf(str, "%s%d", backDynArr(buffers), shell_pid);
  }
  else {
    // add section preceding and following str to DynArr for temp storage
    addBackDynArr(buffers, str, strlen(str));
    addBackDynArr(buffers, str + index + 2, strlen(str + index + 2));
    // resize str if necessary
    if (resize) {
      free(str);
      str = malloc(*str_cap);
    }
    // sandwitch pid between segments of str
    sprintf(str, "%s%d%s", frontDynArr(buffers), shell_pid, backDynArr(buffers));
  }
  // free DynArr
  deleteDynArr(buffers);
  // return new strlen
  return strlen(str);
}

/*******************************************************************************
** Function:    num_digits
** Arguments:   int representing process id
** Description: expands $$ anywhere in string to pid of the process id
*******************************************************************************/
int num_digits( int pid ) {
  // if pid < 10 only 1 digit (base case)
  // else divide by 10 until base is found adding 1 for each digit
  return (pid < 10) ? 1 : 1 + num_digits(pid / 10);
}

/*******************************************************************************
** Function:    change_dir
** Arguments:   path of directory to change to (relative path accepted)
** Description: changes current working directory to that specified by path or
**              changes to $HOME if no path is specified
** Pre-conditions:  path is valid or set to NULL
** Post-conditions: current working directory is path or $HOME
*******************************************************************************/
int change_dir(char *path) {
  int success;
  // no path specified, move to $HOME
  if (path == NULL)
    success = chdir(getenv("HOME"));
  // move to path
  else
    success = chdir(path);
  // if chdir returns err, print an informative message
  if (success < 0) {
    printf("Error: %s\n", strerror(errno));
    return 1;
  }
  return 0;     //return success
}

/*******************************************************************************
** Function:    exit_kill
** Arguments:   kills all bg processes on exit command
** Description: raises SIGKILL on each bg process in pids and waits for its
**              termination.
** Pre-conditions:  exit command has been called
** Post-conditions: all bg processes of smallsh are terminated
*******************************************************************************/
void exit_kill(Arr *pids) {
  int child_exit = 0;
  pid_t curr_pid;
  // loop until array of pids is empty
  while (!isEmptyArr(pids)) {
    // get the last pid in the list
    curr_pid = (pid_t)topArr(pids);
    // raise SIGKILL on it
    kill(curr_pid, SIGKILL);
    // wait for it to terminat
    waitpid(curr_pid, &child_exit, 0);
    // remove it from the list
    popArr(pids);
  }
}

/*******************************************************************************
** Function:    get_status
** Arguments:   int of status returned, int flag set if the command was
**              terminated by a signal
** Description: prints an informative message about exit value or terminating
**              signal of last forground command
** Pre-conditions:  foreground command was called or both args set to -5
** Post-conditions: message printed to console
*******************************************************************************/
void get_status(int status_flag, int is_sig) {
  // If no foreground command has been called alert user
  if (status_flag == -5 && is_sig == -5) {
    printf("No command has been executed\n");
    return;
  }
  // if command was terminated by a signal print signal
  if(is_sig)
    printf("terminated by signal %d\n", status_flag);
  // else, print exit value
  else
    printf("exit value %d\n", status_flag);
}

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
int trim_string(char *out, int size, char *str) {
  if(size == 0)
    return 0;
  const char *end;
  size_t out_size;
  // Trim leading space
  while(isspace((unsigned char)*str)) str++;
  // entire string was whitespace
  if(*str == 0)
    return -1;

  // Trim trailing space
  end = str + strlen(str) - 1;                      // start at end
  while(end > str && isspace((unsigned char)*end))  // move back until non whitespace found
    end--;
  end++;                                            // add space for null term.

  // Set output size to minimum of trimmed string length and buffer size minus 1
  out_size = (end - str) < size-1 ? (end - str) : size-1;

  // Copy trimmed string and add null terminator
  memcpy(out, str, out_size);
  out[out_size] = 0;

  return out_size;
}
