//
//  main.c
//  aufgabe_6
//
//  Created by Niklas Stöhr on 04/11/17.
//  Copyright © 2017 Niklas Stoehr. All rights reserved.
//

/*
client.c -- a stream socket client demo
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

#include <time.h>




//Argumente: DOMAIN PORT

//______________________________________________________________


#define MAXDATASIZE 1000 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
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

    double time1=0.0, tstart;      // time measurment variables
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;


    // Korrekte Anzahl der Konsolenargumente: 2
    if (argc != 4) {
        fprintf(stderr,"3 Argumente müssen übergeben werden - DOMAIN PORT MESSAGE(ziaspace.com 17) \n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);

    // AF_UNSPEC = IPv4 and IPv6
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;


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


    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }



    if (p == NULL) {
        fprintf(stderr, "client: failed to create socket\n");
        return 2;
    }

    tstart = clock();              // start

    if ((numbytes = sendto(sockfd, argv[3], strlen(argv[3]), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }

    time1 += clock() - tstart;     // end


    freeaddrinfo(servinfo);

    printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
    printf("t = %f ms\n", time1);

    close(sockfd);

    return 0;
}

