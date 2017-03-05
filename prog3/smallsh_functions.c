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
  char **args = NULL;
  struct sigaction SIGINT_action = {{0}},
                   SIGTSTP_action = {{0}};
  int success,
      i;
  spawn_pid = fork();

  switch(spawn_pid) {
    case -1:
      perror("Fatal Error");
      exit(1);
      break;
    case 0:
      SIGINT_action.sa_handler = handle_int;
      sigfillset(&SIGINT_action.sa_mask);
      SIGINT_action.sa_flags = SA_RESETHAND;

      SIGTSTP_action.sa_handler = SIG_IGN;

      sigaction(SIGINT, &SIGINT_action, NULL);
      sigaction(SIGTSTP, &SIGTSTP_action, NULL);

      success = redirect_in(parts, 0);
      if (success == -1)
        exit(1);

      success = redirect_out(parts, 0);
     if (success == -1)
        exit(1);

      args = malloc(sizeof(char*) * sizeDynArr(parts));
      for (i = 0; i < sizeDynArr(parts); i++) {
        char *buffer = getDynArr(parts, i);
        args[i] = malloc(sizeof(char) * strlen(buffer));
        strcpy(args[i], buffer);
      }
      args[i] = NULL;

      execvp(args[0], args);
      fprintf(stderr, "%s: %s\n", args[0], strerror(errno));
      exit(1);
      break;
    default:
      break;
  }
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
  struct sigaction SIGINT_action = {{0}},
                   SIGTSTP_action = {{0}};
  int success,
      i;
  spawn_pid = fork();

  switch(spawn_pid) {
    case -1:
      perror("Fatal Error");
      exit(1);
      break;
    case 0:
      SIGINT_action.sa_handler = SIG_IGN;
      SIGTSTP_action.sa_handler = SIG_IGN;

      sigaction(SIGINT, &SIGINT_action, NULL);
      sigaction(SIGTSTP, &SIGTSTP_action, NULL);

      success = redirect_in(parts, 1);
      if (success == -1)
        exit(1);

      success = redirect_out(parts, 1);
      if (success == -1)
        exit(1);

      args = malloc(sizeof(char*) * sizeDynArr(parts));
      for (i = 0; i < sizeDynArr(parts); i++) {
        char *buffer = getDynArr(parts, i);
        args[i] = malloc(sizeof(char) * strlen(buffer));
        strcpy(args[i], buffer);
      }
      args[i] = NULL;

      execvp(args[0], args);
      exit(1);
      break;
    default:

      break;
  }

  return spawn_pid;
}

/*******************************************************************************
** Function:    redirect_in
** Arguments:   DynArr of command parts(see src/dynamicArrayDeque.c),
                flag to determine if stdin should point to null if no direct
                redirect specified.
** Description: checks if input should be redirected. If so it opens the file
**              and performs the redirection, If not and default_null is set
**              it opens /dev/null and redirects, else no redirection is made
** Pre-conditions:  command has been tokenized into DynArr and is not built in
** Post-conditions: child process stdin is redirected to the specified file,
**                  /dev/null, or remains unchanged
*******************************************************************************/
int redirect_in(DynArr *parts, int default_null) {
  int index = indexOfDynArr(parts, "<");
  int file_desc;
  if (index != -1 && index < sizeDynArr(parts)-1) {
    file_desc = open(getDynArr(parts,index + 1), O_RDONLY);
    if (file_desc == -1) {
      fprintf(stderr, "cannot open %s for input: %s\n", getDynArr(parts, index+1), strerror(errno));
      return -1;
    }
    dup2(file_desc, 0);
    removeAtDynArr(parts, index);
    removeAtDynArr(parts, index);
  }
  else {
    if (index != -1)
      removeAtDynArr(parts,index);
    if (default_null) {
      file_desc = open("/dev/null", O_RDONLY);
      if (file_desc == -1) {
        fprintf(stderr, "cannot open %s for input: %s\n", "/dev/null", strerror(errno));
        return -1;
      }
      dup2(file_desc, 0);
    }
  }
  return 0;
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
  int index = indexOfDynArr(parts, ">");
  int file_desc;
  if (index != -1 && index < sizeDynArr(parts)-1) {
    file_desc = open(getDynArr(parts, index + 1), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file_desc == -1) {
      fprintf(stderr, "cannot open %s for output: %s\n", getDynArr(parts, index+1), strerror(errno));
      return -1;
    }
    dup2(file_desc, 1);
    removeAtDynArr(parts, index);
    removeAtDynArr(parts, index);
  }
   else {
    if (index != -1)
      removeAtDynArr(parts,index);
    if (default_null) {
      file_desc = open("/dev/null", O_WRONLY);
      if (file_desc == -1) {
        fprintf(stderr, "cannot open %s for output: %s\n", "/dev/null", strerror(errno));
        return -1;
      }
      dup2(file_desc, 1);
    }
  }
  return 0;
}
/*
char** fill_exec_args(DynArr *parts) {
  int in = indexOfDynArr(parts, "<"),
      out = indexOfDynArr(parts, ">"),
      end = sizeDynArr(parts),
      i = 0;
  char *buffer,
       **args = malloc(sizeof(char*) + end);

  if ((in > out || in < 0) && out > 0)
    end = out;
  else if ((out > in || out < 0) && in > 0)
    end = in;

  for (i = 0; i < end; i++) {
    buffer = getDynArr(parts, i);
    args[i] = malloc(sizeof(char) * (strlen(buffer)+1));
    strcpy(args[i], buffer);
  }
  args[i] = NULL;
  return args;
}*/

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
  if (WIFEXITED(child_exit) != 0) {
    status_flag.is_sig = 0;
    status_flag.status = WEXITSTATUS(child_exit);
    return status_flag;
  }
  status_flag.is_sig = 1;
  status_flag.status = WTERMSIG(child_exit);
  return status_flag;
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
  *is_back = 0;
  int success;

  success = string_split(str, parts, ' ');
  if (success == -1)
    return -1;
  if (strcmp(backDynArr(parts), "&") == 0) {
    *is_back = 1;
    removeBackDynArr(parts);
  }

  return 0;
}
/*******************************************************************************
** Function:    strint_split
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
      int index = c - buffer;
      buffer_size = expand$$(buffer, index, &buffer_cap);
    }

    addBackDynArr(deq, buffer, buffer_size);

    free(buffer);
    buffer = NULL;
    buffer_cap = 0;
  }
  free(buffer);

  fclose(stream);
  return 0;
}

size_t expand$$(char* str, int index, size_t *str_cap) {
  int shell_pid = (int)getpid(),          // get pid
      pid_len = num_digits(shell_pid),
      buffer_size = strlen(str),
      resize = 0;
  DynArr *buffers = createDynArr(2);
  *(str + index) = 0;
  *(str + index + 1) = 0;

  if (*str_cap <= buffer_size + pid_len) {
    resize = 1;
    *str_cap = (strlen(str) + pid_len) * sizeof(char);\
  }
  if (index == 0) {
    addBackDynArr(buffers, str + index + 2, strlen(str + index + 2));
    if (resize) {
      free(str);
      str = malloc(*str_cap);
    }
    sprintf(str, "%d%s", shell_pid, backDynArr(buffers));
  }
  else if (index + 2 == buffer_size) {
    addBackDynArr(buffers, str, strlen(str));
    if (resize) {
      free(str);
      str = malloc(*str_cap);
    }
    sprintf(str, "%s%d", backDynArr(buffers), shell_pid);
  }
  else {
    addBackDynArr(buffers, str, strlen(str));
    addBackDynArr(buffers, str + index + 2, strlen(str + index + 2));
    if (resize) {
      free(str);
      str = malloc(*str_cap);
    }
    sprintf(str, "%s%d%s", frontDynArr(buffers), shell_pid, backDynArr(buffers));
  }
  return strlen(str);
}

int num_digits( int pid ) {
  return (pid < 10) ? 1 : 1 + num_digits(pid / 10);
}

int change_dir(char *path) {
  int success;
  if (path == NULL)
    success = chdir(getenv("HOME"));
  else
    success = chdir(path);
  if (success < 0) {
    printf("Error: %s\n", strerror(errno));
    return 1;
  }
  return 0;
}

void exit_kill(Arr *pids) {
  int i = 0,
      child_exit = 0;
  pid_t curr_pid;
  for (; i < sizeArr(pids); i++) {
    curr_pid = (pid_t)getArr(pids, i);
    kill(curr_pid, SIGKILL);
    waitpid(curr_pid, &child_exit, 0);
    removeArr(pids, (int)curr_pid);
  }
}

void get_status(int status_flag, int is_sig) {
  if (status_flag == -5 && is_sig == -5) {
    printf("No command has been executed\n");
    return;
  }

  if(is_sig)
    printf("terminated by signal %d\n", status_flag);
  else
    printf("exit value %d\n", status_flag);
}


int indexof(char *str, char val, int start) {
  int len = strlen(str),
      i = start;
  for (; i < len; i++)
    if (str[i] == val)
      return i;
  return -1;
}



int trim_string(char *out, int size, char *str) {
  if(size == 0)
    return 0;
  const char *end;
  size_t out_size;
  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
  {
    *out = 0;
    return -1;
  }

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end))
    end--;
  end++;

  // Set output size to minimum of trimmed string length and buffer size minus 1
  out_size = (end - str) < size-1 ? (end - str) : size-1;

  // Copy trimmed string and add null terminator
  memcpy(out, str, out_size);
  out[out_size] = 0;

  return out_size;
}

/*size_t readline(char *buffer, size_t *buff_cap, char *prompt, DynArr *history) {
  size_t buff_size;
  int curr_history = sizeDynArr(history) - 1;
  char *buff2 = NULL,
 *ch;
 *buff_cap = 0;

 printf(prompt);
 fflush(stdout);

 while (1) {
 int c = getchar();
 if (c == '\027' && !isEmptyDynArr(history)) {
 getchar();
 c = getchar();

 if (curr_history < 0)
 curr_history = sizeDynArr(history) - 1;
 else if (curr_history == sizeDynArr(history))
 curr_history = 0;

 switch(c) {
 case 'A':
 printf("\r%s%s", prompt, getDynArr(history, curr_history));
 curr_history--;
 break;
 case 'B':
 printf("\r%s%s", prompt, getDynArr(history, curr_history));
 curr_history++;
 break;
 default:
 break;
 }
 }
 else {
 buff_size = getline(&buff2, buff_cap, stdin);
 ch = strchr(buff2, '\n');
 if (ch != NULL)
 *ch = 0;
 buffer = malloc(sizeof(char) * (buff_size + 1));
 buffer[0] = c;
 buffer[1] = 0;
 strcat(buffer, buff2);
 free(buff2);
 break;
 }
 }
 return buff_size;
 }*/
