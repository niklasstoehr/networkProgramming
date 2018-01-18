//
//  main.c
//  aufgabe_7
//
//  Created by Niklas Stöhr on 05/11/17.
//  Copyright © 2017 Niklas Stoehr. All rights reserved.
//


// Konsolenübergabe - URL im Format: http://i.imgur.com/Et2gFH3.jpg
//______________________________________________________


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


#define MAXDATASIZE 100000 // max number of bytes we can get at once

//______________________________________________________



void urlCutter ( const char *readIn, char **hostname, char **path){
    
    //count number of rows
    printf("URL lautet: %s \n\n", readIn);
    
    char *part = strstr(readIn, "//");
    part += 2;
    char *back = strstr(part, "/");
    
    
    int urlLength = (int) strlen(part);
    int pathLength = (int) strlen(back);
    int hostLength = urlLength - pathLength;
    printf("Domain Name Length: %i\n", hostLength);
    
    char *front = (char*) malloc(MAXDATASIZE);
    
    strncpy(front, part, hostLength);
    
    *hostname = (char*) malloc(MAXDATASIZE);
    *hostname = front;
    
    *path = (char*) malloc(MAXDATASIZE);
    *path = back;
    
}





//_____________________________________________________________



// get sockaddr to avoid packing socket by hand and enable IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


//_____________________________________________________________



int main(int argc, const char * argv[]) {
    
    
    
    //_____________________________________________________________
    
    // read url
    const char *readIn = argv[1];
    // nach der Zeichenkette '//
    char *hostname;
    // nach dem dritten '/'
    char *path;
    
    //_____________________________________________________________
    
    
    urlCutter(readIn, &hostname, &path);
    
    printf("\nHostname: %s\n", hostname);
    printf("Path: %s\n \n", path);
    // Retrieving Quote
    //const char *quote = urlCutter(readIn);
    //printf("\n Neuer ausgewählter Spruch ist: %s \n", quote);
    
    
    // String Ersteller
    char * str = (char *) malloc(1000);
    
    strcat(str, "GET ");
    strcat(str, path);
    strcat(str, " HTTP/1.0\r\n");
    strcat(str, "HOST: ");
    strcat(str, hostname);
    strcat(str, "\r\n");
    strcat(str, "\r\n");
    
    printf("Übergabestring lautet %s", str);
    
    
    
    //___________________________________________________________
    
    int sockfd, numbytes;
    struct addrinfo hints, *servinfo;   // serverinfo will point to the results, hints are filled manually
    struct addrinfo *p;                 // Serversocket von Kommunikationspartner
    int rv;
    char s[INET6_ADDRSTRLEN];
    
    // Korrekte Anzahl der Konsolenargumente: 2
    if (argc != 2) {
        fprintf(stderr,"Ein Argument muss übergeben werden - URL im Format http://httpbin.org/ip \n");
        exit(1);
    }
    
    memset(&hints, 0, sizeof hints);    // make sure the struct is empty
    
    // AF_UNSPEC = IPv4 and IPv6
    hints.ai_family = AF_UNSPEC;        // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP stream sockets
    
    
    char* domainURL = (char*) malloc(MAXDATASIZE);
    strcat(domainURL, hostname);
    //strcat(domainURL, path);
    printf("domainURL: %s\n", domainURL);
    
    if ((rv = getaddrinfo(domainURL, "http", &hints, &servinfo)) != 0) {
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
    
    
    //___________________________________________________________
    
    
    
    // convert to network byte order
    //int temp = htonl(str);
    //printf("Converted send is %i", temp);
    
    
    // send data normally:
    size_t bytes_sent       = 0;
    size_t total_bytes_sent = 0;
    size_t bytes_to_send    = strlen(str);
    
    
    while (1) {
        bytes_sent = send(sockfd, str, strlen(str), 0);
        total_bytes_sent += bytes_sent;
        
        if (total_bytes_sent >= bytes_to_send) {
            break;
        }
    }
    
    char buf[MAXDATASIZE];
    
    // while until
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) < 0) {
        perror("recv");
        exit(1);
    }
    
    buf[numbytes] = '\0';
    printf("client: received from server '%s'\n", buf);
    
    
    //___________________________________________________________
    
    
    FILE *fp;
    
    fp = fopen("image.jpg", "w");
    
    if(fp == NULL) {
        printf("Datei konnte nicht geoeffnet werden.\n");
    }
    
    
    
    char *http = strstr( buf, "\r\n\r\n" );
    char *contentLengthString = (char*) malloc(MAXDATASIZE);
    int contentLengthArray[4];
    int contentLength;
    
    printf("\nHTTP Content: %s", http);
    
    char *save = http;
    
    if ( http != NULL )
    {
        http = strstr( buf, "Content-Length:" );
        http += 16;
        
        strncpy(contentLengthString, http, 300);
        sscanf(contentLengthString, "%d", contentLengthArray);
        contentLength = contentLengthArray[0];
        
        http = save;
        
        // speichere Content im file
        for(int i = 4; i < contentLength+3; i++){
            fprintf(fp, "%c", http[i]);
        }
        
        printf("\n\nContent-Length of HTTP: %i\n\n", contentLength);
    }
    
    fclose(fp);
    //___________________________________________________________
    
    close(sockfd);
    
    
    return 0;
}

