/* Povezaven - TCP odjemalec */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>


#define PORT_NUMBER 55000


int main( int argc, char **argv )
{ 
  int    n, sd, initClient( char * );
  char   buf[128];

  if( argc != 2 ){
    printf("Uporaba: %s ime_streznika\n", argv[0]);
    exit( 1 );
  }
  if( (sd = initClient( argv[1] )) < 0 ){
    printf("napaka init\n");  exit( 1 );
  }
  else{
    printf("tipkaj karkoli, ^D za konec\n");
    while( fgets(&buf[4], sizeof(buf), stdin ) != NULL ){
      memcpy(buf,"c-> ",4);
      printf("%s", buf); 
      if( write(sd, buf, strlen(buf)) == -1)
        perror("write err");
      if( (n = read(sd, buf, sizeof(buf))) == -1)
        perror("read err");
      buf[n] = 0;
      printf("%s", buf);
    }
    close( sd );
  }
  exit( 0 );
}
  

int initClient( char *hostName )
{
  struct sockaddr_in  servAddr;
  struct hostent     *host;
  int    sd;
  
  if( ( host = gethostbyname( hostName )) == NULL) return( -1 );
  memset( &servAddr, 0, sizeof(servAddr));
  memcpy( &servAddr.sin_addr, host->h_addr, host->h_length );
  servAddr.sin_family = host->h_addrtype;
  servAddr.sin_port   = htons( PORT_NUMBER );

  printf("streznik: %s, ", host -> h_name);
  printf("%s:%d\n", inet_ntoa( servAddr.sin_addr ), PORT_NUMBER);
  if( (sd = socket(AF_INET,SOCK_STREAM,0)) < 0 ) return( -2 );
  if( connect(sd, (struct sockaddr *)&servAddr,sizeof(servAddr)) < 0) return( -3 );
  return( sd );
}