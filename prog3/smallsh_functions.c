#include "smallsh_functions.h"

void handle_SIGINT(int signo) {
  raise(SIGINT);
}

void handle_SIGTSTP(int signo) {
  if (!forg) {
    char *message = "Entering foreground-only mode (& is now ignored)\n";
    write(STDOUT_FILENO, message, strlen(message));
    *forg = 1;
  }

  else {
    char *message = "Exiting foreground-only mode\n";
    write(STDOUT_FILENO, message, strlen(message));
    *forg = 0;
  }
   
}

int handle_fg(DynArr *parts) {

}

int handle_bg(DynArr *parts) {

}

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

int string_split(char *str, DynArr *deq, char delim) {
  FILE *stream = fmemopen(str, strlen(str), "r");
  if (stream == NULL)
    return -1;

  char *buffer = NULL,
       *c;
  size_t buffer_cap = 0,
         buffer_size;
  while ((buffer_size = getdelim(&buffer, &buffer_cap, delim, stream)) != -1) {
    c = strchr(buffer, ' ');
    if (c != NULL) 
      *c = 0;
    else
      buffer_size += 1;

    addBackDynArr(deq, buffer, buffer_size);

    free(buffer);
    buffer = NULL;
    buffer_cap = 0;
  }
  fclose(stream);
  return 0;
}

int change_dir(char *path) {
  int success;
  if (path == NULL)
    success = chdir(getenv("HOME"));
  else
    success = chdir(path);
  if (success < 0) {
    printf("Error: %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

void get_status(int status_flag, int is_sig) {
  if (status_flag == -5 && is_sig == -5)
    printf("No command has been executed");

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





