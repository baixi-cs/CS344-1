// include primary library for otp programs
#include "otp_lib.h"

// global connection count used to keep track of active connections
int conn_count = 0;
int close_d = 0;
// Signal Hanlder for SIGCHLD kernel Signal
// Simply decrements open connection count then reaps child process
void handle_SIGCHLD(int signo) {
  conn_count--;
  int exit_val;
  wait(&exit_val);
}
// Signal Hanlder for SIGUSR1 kernel Signal
// Simply sets close flag
void handle_SIGUSR1(int signo) {
  close_d = 1;
}

int main(int argc, char **argv) {
  // otp_dec_d requires a port to be specified at launch
  if (argc < 2) {
    fprintf(stderr,"Usage: %s <port>\n", argv[0]);
    return 1;
  }
  // DECLARE SIGNAL HANDLERS for SIGCHLD and SIGUSR1
  struct sigaction SIGCHLD_action = {{0}},
         sigaction SIGUSR1_action = {{0}};

  // set up SIGCHLD handler to toggle foreground-only mode
  SIGCHLD_action.sa_handler = handle_SIGCHLD;
  // fill sa_mask so subsequent signals will block until the current returns
  sigfillset(&SIGCHLD_action.sa_mask);
  // set the SA_RESTART flag so that sys calls will attempt re-entry rather
  // than failing with an error when SIGCHLD is handled
  SIGCHLD_action.sa_flags = SA_RESTART;

  // set up SIGUSR1 handler to toggle foreground-only mode
  SIGUSR1_action.sa_handler = handle_SIGUSR1;
  // fill sa_mask so subsequent signals will block until the current returns
  sigfillset(&SIGUSR1_action.sa_mask);
  SIGUSR1_action.sa_flags = 0;

  // register SIGCHLD hanlder with kernel
  sigaction(SIGCHLD, &SIGCHLD_action, NULL);
  // register SIGUSR1 hanlder with kernel
  sigaction(SIGUSR1, &SIGUSR1_action, NULL);

  int success,
      listen_fd, conn_fd,
      port_num,
      buff_size;
  // used in accept calls
  socklen_t cli_addr_size;
  // used for forking child switch case
  pid_t spawn_pid;

  char *buffer = NULL,
       *c,
       // Buffers for response to client
       *unauth = "Error: otp_enc cannot use otp_dec_d#",
       *conn_exceed = "Error: Connections Exceeded#",
       *accepted = "accepted#",
       // auth code, used to confirm identity of client
       *auth_seq = "decode";
  // socket structs for server and client info
  struct sockaddr_in serv_addr,
                     cli_addr;
  // clear server struct
  memset((char *)&serv_addr, '\0', sizeof(serv_addr));
  port_num = atoi(argv[1]);                   // get port from argv
  serv_addr.sin_family = AF_INET;             // speficy full socket functionality
  serv_addr.sin_port = htons(port_num);       // convert port to network byte order
  serv_addr.sin_addr.s_addr = INADDR_ANY;     // connect on any address

  // Open a new socket
  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0){
    perror("Error Opening Socket");
    return 1;
  }
  // bind the spefified port and display error if failed
  if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Error binding port");
    return 1;
  }
  // listen for incoming connections
  listen(listen_fd, CONN_COUNT);
  // need size of client address struct for accept call
  cli_addr_size = sizeof(cli_addr);
  // Loop on accept to handle mutiple requests
  do {
    // accept incoming connection and assign its fild descriper to fd
    // also write client information to cli_addr
    conn_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_addr_size);
    if (conn_fd < 0) {
      fprintf(stderr, "Connect Error: %s\n", strerror(errno));
      continue;
    }
    else if (close_d) // break loop after signal USR1
      continue;
    // If Connection count has been exceeded tell the client, close socket,
    // and continue
    if (conn_count >= CONN_COUNT) {
       buff_size = getSockMessage(conn_fd, &buffer);
       sendSockMessage(conn_fd, conn_exceed, strlen(conn_exceed));
       close(conn_fd);
       free(buffer);
       buffer = NULL;
       continue;
    }
    // If we are under the max connections, fork off a child to handle the
    // interaction
    spawn_pid = fork();
    switch(spawn_pid) {
      case -1:
        // Error spawning child, close socket and print error to stderr
        fprintf(stderr, "otp_dec_d Fork Error! Child Process Creation Failed\n");
        close(conn_fd);
        break;
      // Interact with client in child
      case 0:
        // Read entire message from socket, print error and exit if could
        // not read socket
        buff_size = getSockMessage(conn_fd, &buffer);
        if (buff_size < 0) {
          fprintf(stderr, "otp_dec_d Error: Child Failed to Read Socket\n");
          free(buffer);
          close(conn_fd);
          return 1;
        }
        // make sure client has "Authorization". ie client is otp_dec
        // if not send Error message to client, close socket and return.
        if ( (success = authClient(auth_seq, buffer)) < 0 ) {
          sendSockMessage(conn_fd, unauth, strlen(unauth) + 1);
          free(buffer); buffer = NULL;
          close(conn_fd);
          return 1;
        }
        // Else, client is otp_dec
        else {
          // Send acknowledgement to client
          sendSockMessage(conn_fd, accepted, strlen(accepted) + 1);
          // free buffer and prepare for cipher and key
          free(buffer); buffer = NULL;
          // Read entire cipher and key from client.
          // Cipher and key should be delimited by '&'
          buff_size = getSockMessage(conn_fd, &buffer);
          // Find '&' and replace with null terminator to seperate cipher from
          // key
          c = strchr(buffer, '&');
          *c = 0; c++;
          // decode cipher in place inside buffer
          encdec(&buffer, c, 0);
          // send decoded message back to client
          sendSockMessage(conn_fd, buffer, strlen(buffer) + 1);
        }
        // Free resources and exit 0 for success.
        close(conn_fd);
        free(buffer);
        return 0;
        break;
      // Close socket and increment connections in parent
      default:
        conn_count++;
        close(conn_fd);
        break;
    }
  // loop until SIGUSR1 sets close_d
  } while ( !close_d );

  return 0;
}
