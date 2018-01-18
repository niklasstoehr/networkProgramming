//
//  main.c
//  aufgabe_5
//
//  Created by Niklas Stöhr on 02/11/17.
//  Copyright © 2017 Niklas Stoehr. All rights reserved.
//
//
// code mainly taken from http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html


/*
 ** server.c -- a stream socket server demo
 */

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

#define BACKLOG 10     // how many pending connections queue will hold



// arguments are: PORT und TEXTDATEI (inklusive Pfad und .txt)

//_____________________________________________________________

const char * sentenceFinder ( char *filename ){
    
    //count number of rows
    int numRows = 0;
    int i = 0;
    
    // sentences from Textfile
    char* sentences[100];
    
    
    // open file
    FILE *file = fopen ( filename, "r" );
    if ( file != NULL )
    {
        char line [ 10000 ]; /* or other suitable maximum line size */
        
        while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */
        {
            //fputs ( line, stdout ); /* write the line */
            
            //add each rows into array of sentences
            sentences[i]=strdup(line);
            i++;
            numRows++;
        }
        fclose ( file );
    }
    else
    {
        perror ( filename ); /* why didn't the file open? */
        exit(-1);
    }
    
    printf("\n");
    
    //check to be sure going into array correctly
    for (int j=0 ; j<numRows; j++) {
        printf("%s", sentences[j]);
    }
    printf("\n");
    
    srand ( time(NULL) );
    int random = rand() % numRows;
    
    char *selectedSentence = sentences[random];
    
    return selectedSentence;
}


//_____________________________________________________________




void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    
    while(waitpid(-1, NULL, WNOHANG) > 0);
    
    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


//_____________________________________________________________


// Main

int main(int argc, char *argv[])
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    
    
    
    
    // Korrekte Anzahl der Konsolenargumente: 2
    if (argc != 3) {
        fprintf(stderr,"Bitte 2 Konsolenargumente übergeben\n");
        exit(1);
    }
    
    
    
    // read argument Filename
    char *readIn = argv[2];
    
    
    char * filename = (char *) malloc(1000);
    strcat(filename, readIn);
    
    
    printf("Pfad und Name der Datei: %s \n", filename);
    
    
    
    
    
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    
    if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        
        break;
    }
    
    freeaddrinfo(servinfo); // all done with this structure
    
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    
    printf("server: waiting for connections...\n");
    
    
    
    
    
    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        
        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);
        
        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            
            
            
            // Retrieving Quote
            const char *quote = sentenceFinder(filename);
            printf("\n Neuer ausgewählter Spruch ist: %s \n", quote);
            
            
            
            if (send(new_fd, quote, strlen(quote)+1, 0) == -1)
                perror("send");
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }
    
    return 0;
}

