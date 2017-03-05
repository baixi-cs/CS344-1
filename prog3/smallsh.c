// include primary function library for smallsh
#include "smallsh_functions.h"

// enum to reprsent built in commands
enum built_in_commands { CD = 0, EXIT, STATUS };
// flag for forground only mode
int forg = 0;

// sig interupt handler for foreground commands
// SA_RESET will be set so SIGINT will kill on the second raise
void handle_SIGINT(int signo) {
  raise(SIGINT);
}

// SIGTSTP HANDLER
// Toggles forground only mode and writes a descriptive message to the console
// if "forg" is set to 1 smallsh will always run in the forground
void handle_SIGTSTP(int signo) {
  // if forg is not set, write a message saying that smallsh is in forground
  // only mode and set forg
  if (!forg) {
    char *message = "\nEntering foreground-only mode (& is now ignored)\n: ";
    write(STDOUT_FILENO, message, strlen(message));
    forg = 1;
  }
  // else write exting forground only and clear forg
  else {
    char *message = "\nExiting foreground-only mode\n: ";
    write(STDOUT_FILENO, message, strlen(message));
    forg = 0;
  }
}

int main() {

  // DECLARE SIGNAL HANDLERS for SIGTSTP and SIGINT
  struct sigaction SIGINT_action = {{0}},
                   SIGTSTP_action = {{0}};
  // Ignore SIGINT signal in parent process
  SIGINT_action.sa_handler = SIG_IGN;

  // set up SIGTSTP handler to toggle foreground-only mode
  SIGTSTP_action.sa_handler = handle_SIGTSTP;
  // fill sa_mask so subsequent signals will block until the current returns
  sigfillset(&SIGTSTP_action.sa_mask);
  // set the SA_RESTART flag so that sys calls will attempt re-entry rather
  // than failing with an error when SIGTSTP is handled
  SIGTSTP_action.sa_flags = SA_RESTART;
  // register SIGINT and SIGTSTP handlers with sigint
  sigaction(SIGINT, &SIGINT_action, NULL);
  sigaction(SIGTSTP, &SIGTSTP_action, NULL);

  /******  Check out src/dynamicArrayDeque.c for implementation  ******/
  // dynamic array to store individual commands
  DynArr *ind_command = createDynArr(6);
  // dynamic array to store built in command names
  DynArr *commands = createDynArr(3);
  // Add commands in order of enumeration
  addBackDynArr(commands, "cd", 3);
  addBackDynArr(commands, "exit", 5);
  addBackDynArr(commands, "status", 7);
  /****** see src/dynamicArray.c for implementation ******/
  // Dynamic array to store background pids
  Arr *bg_pids = newArr(20);
  // buffers to handle  user input
  char *buffer = NULL,  // set to null to trigger malloc in getline
       *trimmed = NULL,
       // pointer to char for general string manipulation
       *c;

  int exit_flag = 0,        // flag for exiting smallsh
      is_background = 0,    // flag to check if command is background
      trim_size,            // store size after trimming precedeing/trailing whitespace
      built_in,             // stores index of built in commands
      child_exit = -5,      // stores child exit value
      i;                    // general loop variable

  // stores child pid
  pid_t child_pid = -5;
  // stores last termination signal/exit value of foreground command
  struct status_flags status_flag;
  status_flag.status = -5;          // starts at -5 to see if a command has
  status_flag.is_sig = -5;          // been handled yet
  // getline wants size_t values for return size and capacity
  // set to 0 to trigger malloc call within getline
  size_t buffer_size = 0,
         buffer_capacity = 0;

  do { // while ( !exit )
    // print prompt and flush stdout to force it to be written to the console
    printf(": ");
    fflush(stdout);
    // get user input with getline
    buffer_size = getline(&buffer, &buffer_capacity, stdin);
    // replace newline in buffer with null terminator
    if ((c = strchr(buffer, '\n')) != NULL)
      *c = 0;
    // allocate buffer for trimmed string
    trimmed = malloc(sizeof(char) * buffer_size);
    // trim trailing and preceding whitespace from user input
    trim_size = trim_string(trimmed, buffer_size, buffer);
    // if trimmed string is empty or a comment was entered
    if (trim_size == -1 || trimmed[0] == '#') {
      // free trimmed if it was a comment
      if (trimmed != NULL) {
        free(trimmed);
        trimmed = NULL;
      }
    }
    // command is not empty or a comment so handle it
    else {
      // parse command into dynamic array and set is_background
      parse_command(trimmed, ind_command, &is_background);
      // If command is built in handle it
      if ((built_in = indexOfDynArr(commands, frontDynArr(ind_command))) != -1) {
        switch (built_in) {
          // If command is CD attempt to change directory and set status
          case CD:
            change_dir(getDynArr(ind_command, 1));
            break;
          // If command is EXIT kill all background commands and set exit flag
          case EXIT:
            exit_kill(bg_pids);
            exit_flag = 1;
            break;
          // if command is status print status to console
          case STATUS:
            get_status(status_flag.status, status_flag.is_sig);
            break;
          default:
            break;
        }
      }
      // handle non built in command
      else {
        // if is_background is_background is cleared or forgrund only is set
        if (!is_background || forg) {
          // handle foreground command, pass SIGINT handler for child
          child_pid = handle_fg(ind_command, &handle_SIGINT);
          // wait for completion of child process
          child_pid = waitpid(child_pid, &child_exit, 0);
          // get exit signal
          status_flag = get_exit_sig(child_exit);
          // print message if child was terminated by signal
          if (status_flag.is_sig)
            printf("terminated by signal %d\n", status_flag.status);
        }
        // handle background command
        else {
          // call handle background function
          child_pid = handle_bg(ind_command);
          // add child pid to bg_pids
          addArr(bg_pids, (int)child_pid);
          // print background processes pid
          printf("Background pid is %d\n", (int)child_pid);
        }
      }
    }
    // If background process list is not empty
    if (!isEmptyArr(bg_pids)) {
      // loop over background processes checking for exit
      for (i = 0; i < sizeArr(bg_pids); i++) {
        // call wait pid on pid with WNOHANG flag so it does not hault
        pid_t temp = waitpid((pid_t)getArr(bg_pids, i), &child_exit, WNOHANG);
        // if temp !=0 then process has exited
        if (temp != 0) {
          // get exit status or terminatig signal
          struct status_flags stat = get_exit_sig(child_exit);
          // print informative message about exit status or signal
          printf("Background pid %d is done: %s %d\n", (int)temp,
              (stat.is_sig) ? "terminated by signal " : "exit status ", stat.status);
          // remove background array and decrement loop counter because removal will
          // move back all subsequent entries
          removeArr(bg_pids, getArr(bg_pids,i));
          i--;
        }
      }
    }
    // clear the last command
    clearDynArr(ind_command);
    // free trimmed if it hasnt been already
    if (trimmed != NULL)
      free(trimmed);
    // clear buffer
    free(buffer);
    // set both buffers to NULL
    trimmed = NULL;
    buffer = NULL;

    // loop until exit_flag is set
  } while ( !exit_flag );

  // free dynamic arrays
  deleteArr(bg_pids);
  deleteDynArr(ind_command);
  deleteDynArr(commands);
  return 0;
}
