//
//  main.c
//  aufgabe_4
//
//  Created by Niklas Stöhr on 02/11/17.
//  Copyright © 2017 Niklas Stoehr. All rights reserved.
//
// code mainly taken from http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html


/*
 ** client.c -- a stream socket client demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>




//Argumente: DOMAIN PORT (ziaspace.com 17)

//______________________________________________________________


#define MAXDATASIZE 100 // max number of bytes we can get at once


/*
 
int getaddrinfo(const char *node,     // e.g. "www.example.com" or IP
                const char *service,  // e.g. "http" or port number
                const struct addrinfo *hints,
                struct addrinfo **res);

*/

// get sockaddr to avoid packing socket by hand and enable IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Main
int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo;   // serverinfo will point to the results, hints are filled manually
    struct addrinfo *p;                 // Serversocket von Kommunikationspartner
    int rv;
    char s[INET6_ADDRSTRLEN];
    
    // Korrekte Anzahl der Konsolenargumente: 2
    if (argc != 3) {
        fprintf(stderr,"2 Argumente müssen übergeben werden - DOMAIN PORT (ziaspace.com 17) \n");
        exit(1);
    }
    
    memset(&hints, 0, sizeof hints);    // make sure the struct is empty
    
    // AF_UNSPEC = IPv4 and IPv6
    hints.ai_family = AF_UNSPEC;        // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP stream sockets
    
    
    /*    struct addrinfo {
     int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
     int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
     int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
     int              ai_protocol;  // use 0 for "any"
     size_t           ai_addrlen;   // size of ai_addr in bytes
     struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
     char            *ai_canonname; // full canonical hostname
     
     struct addrinfo *ai_next;      // linked list, next node
     };
     */
    
    /*
     
    struct sockaddr_in {
        short int          sin_family;  // Address family, AF_INET
        unsigned short int sin_port;    // Port number
        struct in_addr     sin_addr;    // Internet address
        unsigned char      sin_zero[8]; // Same size as struct sockaddr
    };
     
    */
    
    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        // If there's an error (getaddrinfo() returns non-zero), we can print it out using the function gai_strerror(),
        return 1;
    }
    
    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        
        // Find Socket
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        
        // bind it to the port we passed in to getaddrinfo():
        // Connect to socket
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        
        break;
    }
    
    // p ist Server
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    
    inet_ntop((*p).ai_family, get_in_addr((struct sockaddr *)(*p).ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);
    
    freeaddrinfo(servinfo); // all done with this structure, free the linked-list
    
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    
    buf[numbytes] = '\0';
    
    printf("client: received '%s'\n",buf);
    
    close(sockfd);
    
    return 0;
}