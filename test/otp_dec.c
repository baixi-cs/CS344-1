/*******************************************************************************
** Program: otp_dec
** Project: CS344 Program 4 OTP
** Author: Jordan Grant (grantjo)
*******************************************************************************/

// include primary library for otp programs
#include "otp_lib.h"
#include <netdb.h>

int main(int argc, char **argv) {
  // check for proper number of arguments and that the specified port is a number
  if (argc < 4 || !isdigit((int)argv[3][0])) {
    fprintf(stderr, "Usage: %s <cipher file> <key file> <port>\n", argv[0]);
    return 1;
  }
  // declare a file stream
  FILE *file;

  int sock_fd,                  // holds socket file descripter
      port,                     // holds port
      key_size,                 // holds key file size
      cipher_size;              // holds cipher size

  size_t buff_size,             // holds returned char count from getline
         key_buff_cap = 0,      // buffer capacity of key
         cipher_buff_cap = 0;   // buffer capacity of cipher

  // holds information for server connection
  struct sockaddr_in serv_addr;
  // storage struct to write host information to
  struct hostent *server_host;

  char *key_buff = NULL,        // buffer for key
       *cipher_buff = NULL,      // buffer for cipher
       *host = "localhost",     // hostname
       *auth = "decode#",       // authorization code
       *c;                      // general use char pointer
  // open cipher file for reading, exit 1 and print message on error
  file = fopen(argv[1], "r");
  if (file == NULL) {
    fprintf(stderr, "Error Opening %s: %s\n", argv[1], strerror(errno));
    return 1;
  }
  // read cipher from file and print error and exit if failed
  buff_size =  getline(&cipher_buff, &cipher_buff_cap, file);
  if (buff_size < 0) {
    fclose(file);
    fprintf(stderr, "Read Error %s: %s\n", argv[1], strerror(errno));
    return 1;
  }
  // if cipher is empty or only contains newline print error and exit
  else if (strlen(cipher_buff) <= 1) {
    free(cipher_buff);
    fprintf(stderr, "Error: '%s'is Empty\n", argv[1]);
    fclose(file);
    return 1;
  }
  fclose(file);                           // close file
  c = strchr(cipher_buff, '\n'); *c = 0;   // replace newline with null char
  // If file contains invalid characters print error and exit
  if (!isValidChars(cipher_buff)) {
    fprintf(stderr, "Error: input contains bad characters\n");
    free(cipher_buff);
    return 1;
  }
  // replace location where newline was with '&' to deliminate payload to server
  *c = '&';
  // open key file for writing and print message on error
  file = fopen(argv[2], "r");
  if (file == NULL) {
    fprintf(stderr, "Error Opening %s: %s\n", argv[2], strerror(errno));
    return 1;
  }
  // read key from file and print error/exit on failure
  buff_size =  getline(&key_buff, &key_buff_cap, file);
  if (buff_size < 0) {
    fprintf(stderr, "Read Error %s: %s\n", argv[2], strerror(errno));
    fclose(file);
    return 1;
  }
  // if file is empty or contains only newline print error and exit
  else if (strlen(key_buff) <= 1) {
    fprintf(stderr, "Error: '%s'is Empty\n", argv[2]);
    free(key_buff);
    free(cipher_buff);
    fclose(file);
    return 1;
  }
  fclose(file);                         // close key file
  c = strchr(key_buff, '\n'); *c = '#'; // replace newline with '#' payload sigil
  // get the size of key and cipher
  key_size = strlen(key_buff);
  cipher_size = strlen(cipher_buff);
  // make sure key is larger, else print error and exit
  if (key_size < cipher_size) {
    free(key_buff);
    free(cipher_buff);
    fprintf(stderr, "Error: key '%s' is too short\n", argv[2]);
    return 1;
  }
  // check if cipher_buff is large enough to hold key + buffer
  // if not resize buffer to fit both strings
  if (cipher_size + key_size + 1 >= cipher_buff_cap)
    resizeBuffer(&cipher_buff, cipher_size + key_size + 1);
  // add key to the end of plain buffer
  // string is now delimited like this: [plain text]&[key]#
  strcat(cipher_buff, key_buff);
  free(key_buff);
  key_buff = NULL;
  // clear serv_addr with null chars
  memset((char*)&serv_addr, '\0', sizeof(serv_addr));
  port = atoi(argv[3]);             // convert port to integer
  serv_addr.sin_family = AF_INET;   // Set socket to full functionality
  serv_addr.sin_port = htons(port); // change port to network byte ordering
  // get host information of localhost and print error/exit on failure
  server_host = gethostbyname(host);
  if (server_host == NULL) {
    fprintf(stderr, "Error: Could not resolve %s\n", host);
    return 1;
  }
  // write hostname into serv_addr to create socket
  memcpy((char*)&serv_addr.sin_addr.s_addr, (char*)server_host->h_addr, server_host->h_length);
  // Make a call to socket to get a filedescripter
  // On error, print a message and exit
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    free(cipher_buff);
    fprintf(stderr, "Error: %s\n", strerror(errno));
    return 1;
  }
  // connect to server using socket file descripter
  // print error and exit 2 on failure to connect
  if (connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    close(sock_fd);
    free(cipher_buff);
    fprintf(stderr, "Error: could not contact otp_enc_d on port %d\n", port);
    return 2;
  }
  // send Authorization message to server
  sendSockMessage(sock_fd, auth, strlen(auth) + 1);

  // get return message from server, print error and exit if Error message or
  // unauthorized
  key_size = getSockMessage(sock_fd, &key_buff);
  if ((c = strstr(key_buff, "Error")) != NULL) {
    fprintf(stderr, "Received %s\n", key_buff);
    close(sock_fd);
    free(key_buff);
    free(cipher_buff);
    return 1;
  }
  // send full key and cipher to server for decryption
  sendSockMessage(sock_fd, cipher_buff, strlen(cipher_buff) + 1);
  // free read buffer and set to NULL
  free(key_buff);
  key_buff = NULL;
  // get full response from server into key_buff
  key_size = getSockMessage(sock_fd, &key_buff);
  // print returned data to stdout
  printf("%s\n", key_buff);
  // free resources before exit
  free(key_buff);
  free(cipher_buff);
  close(sock_fd);
  // exit success
  return 0;

}
