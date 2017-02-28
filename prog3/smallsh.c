#include "smallsh_functions.h"

enum built_in_commands { CD = 0, EXIT, STATUS };
int forg = 0;

int main() {

  //DECLARE SIGNAL HANDLERS
  struct sigaction SIGINT_action = {0},
                   SIGTSTP_action = {0};

  SIGINT_action.sa_handler = SIG_IGN;

  SIGTSTP_action.sa_handler = handle_SIGTSTP;
  sigfillset(&SIGTSTP_action.sa_mask);
  SIGTSTP_action.sa_flags = 0;
 
  sigaction(SIGINT, &SIGINT_action, NULL);
  sigaction(SIGTSTP, &SIGSTP_action, NULL);

  DynArr *ind_command = createDynArr(6);
  DynArr *history = createDynArr(20);
  DynArr *commands = createDynArr(3);
  addBackDynArr(commands, "cd", 3);
  addBackDynArr(commands, "exit", 5);
  addBackDynArr(commands, "status", 7);

  char *buffer = NULL,
       *trimmed = NULL,
       *c;

  int exit_flag = 0,
      is_background = 0,
      trim_size,
      built_in;
  struct status_flags status_flag;
  status_flag.status = -5;
  status_flag.is_sig = -5;

  size_t buffer_size,
         buffer_capacity = 0;

  do { // while ( !exit )
    printf(": ");
    fflush(stdout);
    buffer_size = getline(&buffer, &buffer_capacity, stdin);
    if ((c = strchr(buffer, '\n')) != NULL)
      *c = 0;
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
    if (sizeDynArr(history) == MAX_HIST)
      removeFrontDynArr(history);
    addBackDynArr(history, trimmed, trim_size);
    parse_command(trimmed, ind_command, &is_background);
    
    if ((built_in = indexOfDynArr(commands, frontDynArr(ind_command))) != -1) {
      switch (built_in) {
        case CD:
          status_flag.status = change_dir(getDynArr(ind_command, 1));
          status_flag.is_sig = 0;
          break;
        case EXIT:
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
        // handle forground process
      } 
      else {
        // handle background process
      }
    }

    clearDynArr(ind_command);    
    free(trimmed);
    free(buffer);
    trimmed = NULL;
    buffer = NULL;

  } while ( !exit_flag );
    
  deleteDynArr(ind_command);
  deleteDynArr(commands);
  deleteDynArr(history);
  return 0;
}
