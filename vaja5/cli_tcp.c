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
#include <sys/stat.h>
#include <fcntl.h>
 
//odjemalec-terminal: gst-launch-1.0 -v filesrc location=video ! h264parse ! avdec_h264 ! autovideosink sync=false
// v direktoriju: mkfifo video
 
 
#define PORT_NUMBER 55000
 
 
int main( int argc, char **argv )//ODJEMALEC - PC
{
  int    n, sd, initClient( char * );
  char   buf[128];
  int fd;
  int pack;
  char *buffer;
  int buffer_size = 1280*720*4;
 
  buffer = (char *)malloc(buffer_size);
 
  if( argc != 1 ){
    printf("Uporaba: %s ime_streznika\n", argv[0]);
    exit( 1 );
  }
 
  if ((fd = open("video", O_RDWR)) < 0){
    printf("napaka open\n");
    exit(1);
  }
 
  // vstavi ip server pc-ja
  if( (sd = initClient( "192.168.1.100" )) < 0 ){
    printf("napaka init\n");  exit( 1 );
  }
  else{
    while( (pack = read(sd, buffer, buffer_size)) > 0 ){
      printf("pack: %d\n", pack);
      if (write(fd, buffer, pack) < 0) {
        printf("napaka write\n");
        exit(1);
      }
   
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