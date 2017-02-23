#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "smallsh_functions.h"

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
    return 1;
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


