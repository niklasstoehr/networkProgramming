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

#define BACKLOG 10     // how many pending connections queue will hold
#define MAX_HASH 10
#define MAXDATASIZE 10000

int sockfd, new_fd, clientSock, someSock, succSocket, headerSocket;  // listen on sock_fd, new connection on new_fd
struct addrinfo hints, *servinfo, *p;
struct sockaddr_storage their_addr; // connector's address information
socklen_t sin_size;
struct sigaction sa;
int yes=1;
char s[INET6_ADDRSTRLEN];
int rv;
int numbytes;
int hashSize = 65536;	// 2^16
//int onlyNode = 0;	// flag for first node


int eigID = -1;
char* eigIP;
uint32_t eigIpInt;
char* eigPort;
int eigPortInt;

int succID = -1;
char* succIP;
char* succPort;

int predID = -1;
char* predIP;
char* predPort;

int looker(int hash, int eigeneID, int pred);
int join(int neueID, char* neueIP, char* neuerPort, char* header);
int notify(int notifyerID, char* notifyerIP, char* notifyerPort);
int stabilize(int incomingID, char* incomingIP, char* incomingPort);
void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
