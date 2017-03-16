#include "otp_lib.h"

int conn_count = 0;

void handle_SIGCHLD(int signo) {
  conn_count--;
  int exit_val;
  wait(&exit_val);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr,"Usage: %s <port>\n", argv[0]);
    return 1;
  }
  // DECLARE SIGNAL HANDLERS for SIGTSTP and SIGINT
  struct sigaction SIGCHLD_action = {{0}};
    
  // set up SIGTSTP handler to toggle foreground-only mode
  SIGCHLD_action.sa_handler = handle_SIGCHLD;
  // fill sa_mask so subsequent signals will block until the current returns
  sigfillset(&SIGCHLD_action.sa_mask);
  // set the SA_RESTART flag so that sys calls will attempt re-entry rather
  // than failing with an error when SIGTSTP is handled
  SIGCHLD_action.sa_flags = SA_RESTART;
  // register SIGCHLD hanlder with kernel
  sigaction(SIGCHLD, &SIGCHLD_action, NULL);

  int j = 0,
      success,
      close_d = 0,
      listen_fd, conn_fd,
      port_num,
      buff_size;

  socklen_t cli_addr_size;

  pid_t spawn_pid;

  char *buffer = NULL,
       *c,
       *unauth = "Error: otp_enc cannot use otp_dec_d#",
       *conn_exceed = "Error: Connections Exceeded#",
       *auth_seq = "decode",
       *accepted = "accepted#";

  struct sockaddr_in serv_addr, 
                     cli_addr;

  memset((char *)&serv_addr, '\0', sizeof(serv_addr));
  port_num = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port_num);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  
  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0){
    perror("Error Opening Socket");
    return 1;
  }

  if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Error binding port");
    return 1;
  }

  listen(listen_fd, CONN_COUNT);

  cli_addr_size = sizeof(cli_addr);
  do {
    conn_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_addr_size);
    if (conn_fd < 0) {
      fprintf(stderr, "Connect Error: %s\n", strerror(errno));
      continue;
    }
    if (conn_count >= CONN_COUNT) {
       buff_size = getSockMessage(conn_fd, &buffer);
       sendSockMessage(conn_fd, conn_exceed, strlen(conn_exceed));
       close(conn_fd);
       free(buffer);
       buffer = NULL;
       continue;
    }
    spawn_pid = fork();

    switch(spawn_pid) {
      case -1:
        break;
      case 0:
        buff_size = getSockMessage(conn_fd, &buffer);
        if (buff_size < 0) {
          free(buffer);
          close(conn_fd);
          return 1;
        }
        j = 0;
        if ( (success = authClient(auth_seq, buffer)) < 0 ) {
          sendSockMessage(conn_fd, unauth, strlen(unauth) + 1);
          close(conn_fd);
          return 1;  
        } else {
          sendSockMessage(conn_fd, accepted, strlen(accepted) + 1);
          free(buffer); buffer = NULL;
          buff_size = getSockMessage(conn_fd, &buffer);
          c = strchr(buffer, '&');
          *c = 0; c++;
          encdec(&buffer, c, 0);
          sendSockMessage(conn_fd, buffer, strlen(buffer) + 1);
          close(conn_fd);
          free(buffer);
          return 0;           
        }
        free(buffer);
        break;
      default:
        conn_count++;
        close(conn_fd);
        break;
    }

  } while ( !close_d );

  return 0;
}
