/* Nepovezaven UDP odjemalec */
#include <stdio.h>
#include <stdlib.h>
 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
 
//terminal: gst-launch-1.0 -v filesrc location=video ! h264parse ! avdec_h264 ! autovideosink sync=false
 
 
#define MAXLINE 4096
#define SERVPORTNO 50000
 
int main(int argc, char **argv) //client PC
{
  int sockfd,n;
  struct sockaddr_in servaddr;
  char sendline[ MAXLINE ], recvline[ MAXLINE ];
  int fd, pack;
  char *buffer;
  int buffer_size = 1280*720*4;
 
  buffer = (char *)malloc(buffer_size);
 
  if ((fd = open("videoFIFO", O_RDWR) < 0)){
    printf("napaka open\n");
    exit(1);
  }
  printf("fd: %d\n", fd);
 
  if (argc != 1){
    printf("usage: udpcli <IP address>\n");
    exit(1);
  }
  if( (sockfd=socket(AF_INET,SOCK_DGRAM,0)) == -1){
    perror("socket err");
    exit(1);
  }
  printf("sockfd: %d\n", sockfd);
  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr=inet_addr("192.168.1.100");
  servaddr.sin_port=htons(SERVPORTNO);
 
  pack = recvfrom(sockfd, buffer,buffer_size,0,NULL,NULL);
  printf("pack: %d\n", pack);
 
  // while( (pack = recvfrom(sockfd, buffer,buffer_size,0,NULL,NULL)) > 0){
  //   printf("pack: %d\n", pack);
  //   if(write(fd, buffer, pack) < 0){
  //     printf("write err\n");
  //     exit(1);
  //   }
  // }
 
  exit( 0 );
}