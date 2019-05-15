/* Nepovezaven UDP streznik */ 
#include <stdio.h> 
#include <stdlib.h>

#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>

#include <string.h>

#define MAXLINE     4096 
#define SERVPORTNO 50000 

int main( void ) 
{ 
  int sockfd,n; 
  struct sockaddr_in servaddr,cliaddr; 
  socklen_t len; 
  char mesg[MAXLINE]; 

  if( (sockfd=socket(AF_INET,SOCK_DGRAM,0)) == -1){
    perror("socket err");
    exit(1);
  }
  bzero(&servaddr,sizeof(servaddr)); 
  servaddr.sin_family = AF_INET; 
  servaddr.sin_addr.s_addr=htonl(INADDR_ANY); 
  servaddr.sin_port=htons( SERVPORTNO ); 
  if( bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1){
    perror("bind err"); 
    exit(1);
  }
  while( 1 ){ 
    len = sizeof(cliaddr); 
    n = recvfrom(sockfd,mesg,MAXLINE,0,(struct sockaddr *)&cliaddr,&len); 
    printf("client: %s:%d\n", inet_ntoa( cliaddr.sin_addr ), ntohs( cliaddr.sin_port));
    memcpy(mesg,"s-> ",4);
    sendto(sockfd,mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr)); 
  } 
} 