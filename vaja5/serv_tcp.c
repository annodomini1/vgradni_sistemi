/* Povezaven - TCP streznik */
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
  struct sockaddr_in cliAddr;
  socklen_t size;
  char      buf[128];
 
  int       initServer( char * );
  int       sd, sn, n;

  if( argc != 2 ){
    printf("Uporaba: %s ime_naprave\n", argv[0]);
    exit( 1 );
  }
  if( (sd = initServer( argv[1] )) < 0){
    printf("Napaka: init server\n"); exit( 1 );
  }
  
  listen( sd, 5 );
  alarm( 60 ); /* koncaj po eni minuti */

  while( 1 ){
    size = sizeof(cliAddr);
    memset( &cliAddr, 0, size );
    
    if( (sn = accept(sd, (struct sockaddr *)&cliAddr, &size)) < 0){
      perror("accept err"); exit( 2 ); 
    }
    /* zveza je vzpostavljena, ustvari strezni proces */
    if( fork() == 0 ){
      printf("odjemalec: %s:%d\n", inet_ntoa( cliAddr.sin_addr ), ntohs( cliAddr.sin_port ) );
      while( (n = read( sn, buf, sizeof( buf ))) > 0 ){
	memcpy(buf,"s-> ",4);
        if( write(sn, buf, n) == -1)
          perror("write err");
      }
      printf("odjemalec: %s:%d je prekinil povezavo\n", inet_ntoa( cliAddr.sin_addr ), ntohs( cliAddr.sin_port ));
      close( sn );
      exit( 0 );
    }
  }
}

int initServer( char *hostName )
{
  struct sockaddr_in  servAddr; 
  struct hostent     *host;
  int    sd;

  if( (host = gethostbyname( hostName )) == NULL) return( -1 );
  memset( &servAddr, 0, sizeof(servAddr) );
  memcpy( &servAddr.sin_addr, host->h_addr, host->h_length );
  servAddr.sin_family  = host->h_addrtype;
  servAddr.sin_port    = htons( PORT_NUMBER );

  printf("streznik: %s, ", host -> h_name);
  printf("%s:%d\n", inet_ntoa(servAddr.sin_addr), PORT_NUMBER );

  if( (sd = socket(AF_INET, SOCK_STREAM,0)) < 0 ) return( -2 );
  if( bind(sd, (struct sockaddr *)&servAddr, sizeof( servAddr )) < 0) return( -3 );
  return( sd );
}
