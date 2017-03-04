#include "smallsh_functions.h"

enum built_in_commands { CD = 0, EXIT, STATUS };
int forg = 0;
int sigstop = 0;

void handle_SIGINT(int signo) {
  raise(SIGINT);
}

void handle_SIGTSTP(int signo) {
  if (!forg) {
    char *message = "\nEntering foreground-only mode (& is now ignored)\n: ";
    write(STDOUT_FILENO, message, strlen(message));
    forg = 1;
  }
  else {
    char *message = "\nExiting foreground-only mode\n: ";
    write(STDOUT_FILENO, message, strlen(message));
    forg = 0;
  }
}

int main() {

  //DECLARE SIGNAL HANDLERS
  struct sigaction SIGINT_action = {{0}},
                   SIGTSTP_action = {{0}};

  SIGINT_action.sa_handler = SIG_IGN;

  SIGTSTP_action.sa_handler = handle_SIGTSTP;
  sigfillset(&SIGTSTP_action.sa_mask);
  SIGTSTP_action.sa_flags = SA_RESTART;

  sigaction(SIGINT, &SIGINT_action, NULL);
  sigaction(SIGTSTP, &SIGTSTP_action, NULL);

  DynArr *ind_command = createDynArr(6);
  //DynArr *history = createDynArr(20);
  DynArr *commands = createDynArr(3);
  addBackDynArr(commands, "cd", 3);
  addBackDynArr(commands, "exit", 5);
  addBackDynArr(commands, "status", 7);

  Arr *bg_pids = newArr(20);

  char *buffer = NULL,
       *trimmed = NULL,
       *c;

  int exit_flag = 0,
      is_background = 0,
      trim_size,
      built_in,
      child_exit = -5,
      i;

  pid_t child_pid = -5;
  struct status_flags status_flag;
  status_flag.status = -5;
  status_flag.is_sig = -5;

  size_t buffer_size = 0,
         buffer_capacity = 0;

  do { // while ( !exit )
    printf(": ");
    fflush(stdout);
    buffer_size = getline(&buffer, &buffer_capacity, stdin);
    
    if ((c = strchr(buffer, '\n')) != NULL)
      *c = 0;
    // readline(buffer, &buffer_capacity, ": ", history);
    trimmed = malloc(sizeof(char) * (buffer_size + 1));

    trim_size = trim_string(trimmed, buffer_size, buffer);
    if (trim_size == -1 || trimmed[0] == '#') {
      if (trimmed != NULL) {
        free(trimmed);
        trimmed = NULL;
      }
      free(buffer);
      buffer = NULL;
      buffer_capacity = 0;
      continue;
    }
    
    parse_command(trimmed, ind_command, &is_background);
    if ((built_in = indexOfDynArr(commands, frontDynArr(ind_command))) != -1) {
      switch (built_in) {
        case CD:
          status_flag.status = change_dir(getDynArr(ind_command, 1));
          status_flag.is_sig = 0;
          break;
        case EXIT:
          exit_kill(bg_pids);
          exit_flag = 1;
          break;
        case STATUS:
          get_status(status_flag.status, status_flag.is_sig);
          break;
        default:
          break;
      }
    } 
    else {
      if (!is_background || forg) {
        child_pid = handle_fg(ind_command, &handle_SIGINT);
        child_pid = waitpid(child_pid, &child_exit, 0);
        status_flag = get_exit_sig(child_exit);  
        if (status_flag.is_sig) 
          printf("terminated by signal %d\n", status_flag.status); 
      } 
      else {
        child_pid = handle_bg(ind_command);
        addArr(bg_pids, (int)child_pid);
        printf("Background pid is %d\n", (int)child_pid);
      }
    }

    if (!isEmptyArr(bg_pids)) {
      for (i = 0; i < sizeArr(bg_pids); i++) {   
        pid_t temp = waitpid((pid_t)getArr(bg_pids, i), &child_exit, WNOHANG);
        if (temp != 0) {
          struct status_flags stat = get_exit_sig(child_exit);
          printf("Background pid %d is done: %s %d\n", (int)temp,
              (stat.is_sig) ? "terminated by signal " : "exit status ", stat.status);
          removeArr(bg_pids, getArr(bg_pids,i));
          i--;
        }
      }
    }

    clearDynArr(ind_command);    
    free(trimmed);
    free(buffer);
    trimmed = NULL;
    buffer = NULL;

  } while ( !exit_flag );

  deleteArr(bg_pids);
  deleteDynArr(ind_command);
  deleteDynArr(commands);
  //deleteDynArr(history);
  return 0;
}
