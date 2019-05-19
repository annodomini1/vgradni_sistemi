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
#include <sys/stat.h>
#include <fcntl.h>


#define PORT_NUMBER 55000

int main( int argc, char **argv )//STREŽNIK - RASPBERRY
{
  struct sockaddr_in cliAddr;
  socklen_t size;
  char      buf[128];
 
  int       initServer( char * );
  int       sd, sn, n, fd;
  int buffer_size = 1280*720*3;
  char *buffer;

  buffer = (char *)malloc(buffer_size);

  if( argc != 1 ){
    printf("Uporaba: %s ime_naprave\n", argv[0]);
    exit( 1 );
  }

  // vstavi ip server pc-ja
  if( (sd = initServer( "192.168.1.100" )) < 0){
    printf("Napaka: init server\n"); exit( 1 );
  }

  if ((fd = open("test.h264", O_RDONLY))  < 0){ //tuki boš dal ime fifota  + read-write
    printf("napaka open\n");
    exit(1);
  }

  printf("fd: %d\n", fd);
  
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
      while( (n = read( fd, buffer, buffer_size)) > 0 ){
        if( write(sn, buffer, n) == -1)
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
