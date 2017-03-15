#include "otp_lib.h"

int main(int argc, char **argv) {
  if (argc < 4 || !isdigit((int)argv[3][0])) {
    fprintf(stderr, "Usage: %s <plaintext file> <key file> <port>\n", argv[0]);
    return 1;
  }

  FILE *file;

  int sock_fd, 
      port,
      key_size,
      plain_size;

  size_t buff_size,
         key_buff_cap = 0,
         plain_buff_cap = 0;;

  struct sockaddr_in serv_addr;

  struct hostent *server_host;

  char *key_buff = NULL,
       *plain_buff = NULL,
       *host = "localhost",
       *auth = "encode#",
       *c;

  file = fopen(argv[1], "r");
  if (file == NULL) {
    fprintf(stderr, "Error Opening %s: %s\n", argv[1], strerror(errno));
    return 1;
  }
  buff_size =  getline(&plain_buff, &plain_buff_cap, file);
  if (buff_size < 0) {
    fprintf(stderr, "Read Error %s: %s\n", argv[1], strerror(errno));
    return 1;
  }
  fclose(file);
  c = strchr(plain_buff, '\n'); *c = 0;

  if (!isValidChars(plain_buff)) {
    fprintf(stderr, "Error: plaintext must contain valid chars ([A-Z] or ' ')\n");
    free(plain_buff);
    return 1;
  }

  *c = '&';
  

  file = fopen(argv[2], "r");
  if (file == NULL) {
    fprintf(stderr, "Error Opening %s: %s\n", argv[2], strerror(errno));
    return 1;
  }
  buff_size =  getline(&key_buff, &key_buff_cap, file);
  if (buff_size < 0) {
    fprintf(stderr, "Read Error %s: %s\n", argv[2], strerror(errno));
    return 1;
  }
  fclose(file);
  c = strchr(key_buff, '\n'); *c = '#';

  key_size = strlen(key_buff);
  plain_size = strlen(plain_buff);

  if (key_size < plain_size) {
    fprintf(stderr, "Error: key should not be smaller than plaintext\n");
    return 1;
  }

  if (plain_size + key_size + 1 >= plain_buff_cap)
    resizeBuffer(&plain_buff, plain_size + key_size + 1);
 
  strcat(plain_buff, key_buff);
  
  memset((char*)&serv_addr, '\0', sizeof(serv_addr));
  port = atoi(argv[3]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  server_host = gethostbyname(host);
  if (server_host == NULL) {
    fprintf(stderr, "Error: Could not resolve %s\n", host);
    return 1;
  }
  memcpy((char*)&serv_addr.sin_addr.s_addr, (char*)server_host->h_addr, server_host->h_length);
  
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    fprintf(stderr, "Error: %s\n", strerror(errno));
    return 1;
  }

  if (connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    fprintf(stderr, "Error: %s\n", strerror(errno));
    return 1;
  }

  sendSockMessage(sock_fd, auth, strlen(auth) + 1);

  free(key_buff);
  key_buff = NULL;

  key_size = getSockMessage(sock_fd, &key_buff);
  if ((c = strstr(key_buff, "Error")) != NULL) {
    fprintf(stderr, "Received %s\n", key_buff);
    free(key_buff);
    free(plain_buff);
    return 1;
  } 

  sendSockMessage(sock_fd, plain_buff, strlen(plain_buff) + 1);

  free(key_buff);
  key_buff = NULL;

  key_size = getSockMessage(sock_fd, &key_buff);

  printf("%s\n", key_buff);

  free(key_buff);
  free(plain_buff);
  close(sock_fd);

  return 0;

}
