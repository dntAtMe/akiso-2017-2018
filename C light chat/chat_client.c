  #include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>


#define MAX_MSG_SIZE 100
#define MAX_USR_NAME 15

int request_connection(char *hostname, int port);
int readwrite(int in, int out);

int readwrite(int in, int out)
{
 char s[100];
 int i = 0;
 while(read(in, s+i, 1) != 0)
 {
  if (s[i] == '\n' || s[i] == '\0')
  {
   write(out, s, i+1);
   return i+1;
  }
  i++;
 }

 return 0;
}

int main(int argc, char **argv)
{
 char *name;

 int port;
 int socket_fd;
 int result;

 char *hostname;
 char msg[100];

 struct hostent *hostinfo;
 struct sockaddr_in socketinfo;

 fd_set readfds, masterfds;

 if (argc != 4)
 {
  fprintf(stderr, "usage: chat_client username host port\n");
  return -1;
 }

 name = argv[1];
 hostname = argv[2];
 port = atoi(argv[3]);

 socket_fd = request_connection(hostname, port);
 FD_ZERO(&masterfds);
 FD_SET(socket_fd, &masterfds);
 FD_SET(0, &masterfds);
 int i =  0;
 //while(name[i] != '\0')
	// printf("%c\n", name[++i]);
// printf("%d\n", strlen(name));
 write(socket_fd, name, strlen(name) + 1);

 while(1)
 {
  memcpy(&readfds, &masterfds, sizeof(fd_set));

  if (select(socket_fd + 1, &readfds, NULL, NULL, NULL) < 0)
  {
   fprintf(stderr, "on select()");
   return -1;
  }

  if (FD_ISSET(socket_fd, &readfds))
  {
    readwrite(socket_fd, 1);
  } else if (FD_ISSET(0, &readfds))
  {
    int n = read(0, msg, 100);

    write(socket_fd, msg, n);
  }
 }

}

int request_connection(char *hostname, int port)
{
 struct sockaddr_in sn;
 int s, ok;
 struct hostent *he;

 if (!(he = gethostbyname(hostname)))
 {
  fprintf(stderr, "on gethostbyname()");
  return -1;
 }

 ok = 0;

 while (!ok)
 {
  sn.sin_family = AF_INET;
  sn.sin_port = htons( (unsigned short)port);
  sn.sin_addr.s_addr = *(uint32_t *)(he->h_addr_list[0]);

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
   fprintf(stderr, "on socket()");
   return -1;
  }

  ok = connect(s, (struct sockaddr *)&sn, (socklen_t)sizeof(sn)) != -1;
  if (!ok) sleep(1);
 }

 return s;
}
