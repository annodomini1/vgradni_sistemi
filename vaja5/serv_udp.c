/* Nepovezaven UDP streznik */ 
#include <stdio.h> 
#include <stdlib.h>

#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXLINE     4096 
#define SERVPORTNO 50000 

int main( void ) //UDP server - Raspberry
{ 
  int sockfd,n; 
  struct sockaddr_in servaddr,cliaddr; 
  socklen_t len; 
  char mesg[MAXLINE];
  int fd, pack;
  char *buffer;
  int buffer_size = 1280*720;

  buffer = (char *)malloc(buffer_size);

  if ((fd = open("test.h264", O_RDONLY)) < 0){
    printf("napaka open\n");
    exit(1);
  }
  printf("fd: %d\n", fd);

  if( (sockfd=socket(AF_INET,SOCK_DGRAM,0)) == -1){
    perror("socket err");
    exit(1);
  }
  printf("sockfd: %d\n", sockfd);
  bzero(&servaddr,sizeof(servaddr)); 
  servaddr.sin_family = AF_INET; 
  servaddr.sin_addr.s_addr=htonl(INADDR_ANY); 
  servaddr.sin_port=htons( SERVPORTNO ); 
  if( bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1){
    perror("bind err"); 
    exit(1);
  }
  sleep(3);
  while( 1 ){ 
    while ((pack = read(fd, buffer, buffer_size)) > 0){
      printf("pack: %d\n", pack);
      sendto(sockfd,buffer,pack,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
    }
  } 
} 