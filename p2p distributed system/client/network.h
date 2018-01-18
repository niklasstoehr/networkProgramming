#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>

//void sendArray( int sockfd, char** randomMovies, struct addrinfo *p );
void setMovies( char** randomMovies, char* domain, char* port );
void getMovies( char** randomMovies, char* domain, char* port );
void deleteMovies( char** randomMovies, char* domain, char* port );
void unmarshall( char* buf );

