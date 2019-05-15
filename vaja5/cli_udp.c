/* Nepovezaven UDP odjemalec */ 
#include <stdio.h> 
#include <stdlib.h>

#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>

#include <string.h>


#define MAXLINE 4096 
#define SERVPORTNO 50000 

int main(int argc, char **argv) 
{ 
  int sockfd,n; 
  struct sockaddr_in servaddr; 
  char sendline[ MAXLINE ], recvline[ MAXLINE ]; 
  
  if (argc != 2){ 
    printf("usage: udpcli <IP address>\n"); 
    exit(1); 
  } 
  if( (sockfd=socket(AF_INET,SOCK_DGRAM,0)) == -1){
    perror("socket err");
    exit(1);
  } 
  bzero(&servaddr,sizeof(servaddr)); 
  servaddr.sin_family = AF_INET; 
  servaddr.sin_addr.s_addr=inet_addr(argv[1]); 
  servaddr.sin_port=htons(SERVPORTNO); 

  while (fgets(&sendline[4], MAXLINE,stdin) != NULL){
    memcpy(sendline,"c-> ", 4);
    fputs(sendline,stdout);
    n = sendto(sockfd,sendline,strlen(sendline),0, 
            (struct sockaddr *)&servaddr,sizeof(servaddr));
    if( n == -1 ){
      perror("sento err");
      exit(1);
    }
    n=recvfrom(sockfd,recvline,MAXLINE,0,NULL,NULL); 
    if( n == -1 ){
      perror("recvfrom err");
      exit(1);
    }
    recvline[n]=0; 
    fputs(recvline,stdout); 
  } 
  exit( 0 );
} 
