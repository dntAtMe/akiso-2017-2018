#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_USR_NAME 30
#define MAX_CLIENTS 50

typedef struct
{
 int client_fd;
 int client_id;
 char client_name[30];
} client_t;

client_t *clients[MAX_CLIENTS];

char usernames[MAX_CLIENTS][MAX_USR_NAME];
int userfds[MAX_CLIENTS];
fd_set masterfds;

int create_socket(char *hostname, int port);
int set_username(int client_fd, int client_id, char *usernames);
int send_message_from_client(int client_id);

int print_usernames(int out, int clients_amount)
{
 char output[1024];
 output[0] = '\0';
 strcat(output, "Logged users: \n") ;

 for(int i = 0; i < clients_amount; i++)
 {
   //if (usernames[i][0] != '\0')
   strcat(output, usernames[i]);
   char id[20];
   sprintf(id, ", ID : %d\n\n", i);
   strcat(output, id);
    //printf("%d: %s\n", i, usernames[i]);
 }

 write(out, output, strlen(output) + 1);
}


int main(int argc, char **argv)
{
 int port;
 int clients_amount = 0;
 int socket_fd;
 int client_fd;

 char *hostname;

 fd_set readfds;

 if (argc != 3)
 {
  printf("usage: chat_server host port\n");
  return -1;
 }

 hostname = argv[1];
 port = atoi(argv[2]);

 socket_fd = create_socket(hostname, port);

 listen(socket_fd, 1);
 FD_ZERO(&masterfds);
 FD_SET(socket_fd, &masterfds);
 FD_SET(0, &masterfds);

 while(1)
 {
  //memcpy(&readfds, &masterfds, sizeof(fd_set));
  readfds = masterfds;
  select(FD_SETSIZE, &readfds, NULL, NULL, NULL);

  if (FD_ISSET(socket_fd, &readfds))
  {
   client_fd = accept(socket_fd, NULL, NULL);
   FD_SET(client_fd, &masterfds);

   userfds[clients_amount] = client_fd;
   printf("client fd: %d\n", userfds[clients_amount]);
   set_username(client_fd, clients_amount, usernames[clients_amount]);
   print_usernames(client_fd, clients_amount);

   printf("client username: %s\n", usernames[clients_amount]);
   clients_amount++;
  }

    for (int i = 0; i < clients_amount; i++)
    {
      if (userfds[i] && FD_ISSET(userfds[i], &readfds))
      {
        send_message_from_client(i);
      }
    }

 }

}

//TODO: HANDLE CLIENT EXITING (( WHEN READ REEACHES 0 ,CLOSE SOCKET))
int send_message_from_client(int client_id)
{
 char msg[100];
 char target[3];
 int n;
 if ( (n = read(userfds[client_id], msg, 100) ) == 0)
 {
  close(userfds[client_id]);
  FD_CLR(userfds[client_id], &masterfds);
  userfds[client_id] = 0;
  usernames[client_id][0] = '\0';
 }
 msg[n] = '\0';
 printf("new message: %s\n", msg);
 int i = 0;
 while (msg[i] != ' ')
 {
   target[i] = msg[i];
   i++;
 }
 target[i] = '\0';
 int target_id = atoi(target);
if (target_id == client_id)
 return 0;

 printf("target %d %d\n", target_id, userfds[target_id]);
 char new_msg[100];
 new_msg[0] = '\0';
 sprintf(new_msg, "%d[%s]:", client_id, usernames[client_id]);
 strcat(new_msg, msg + i);
 strcat(new_msg, "\n");
 printf("length: %d\n", strlen(new_msg));
 write(userfds[target_id], new_msg, strlen(new_msg) + 1);

 return 0;
}

int create_socket(char *hostname, int port)
{
 struct sockaddr_in sn;
 int s;
 struct hostent *he;

 if (!(he = gethostbyname(hostname)))
 {
  fprintf(stderr, "gethostbyname()");
  return -1;
 }

 bzero((char *)&sn, sizeof(sn));
 sn.sin_family = AF_INET;
 sn.sin_port = htons((short) port);
 sn.sin_addr = *(struct in_addr *)(he->h_addr_list[0]);

 if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
 {
  perror("socket()");
 }
 if (bind(s, (struct sockaddr *)&sn, sizeof(sn)) == -1)
 {
  perror("bind()");
 }

 return s;
}

int set_username(int client_fd, int client_id, char *usernames)
{
 char s[30];
 int i;

 i = 0;
 while(read(client_fd, s+i, 1) != 0)
 {
  if (s[i] == '\0')
  {
   break;
  }
  usernames[i] = s[i];
  i++;
 }
 usernames[i] = '\0';
 return 0;
}
