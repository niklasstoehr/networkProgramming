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
#include "network.h"
#include "main.h"

#define MAXDATASIZE 10000

void setMovies( char** randomMovies, char* domain, char* port ){

    int numbytes;
    int sockfd;
    char s[INET6_ADDRSTRLEN];
    struct addrinfo hints, *p, *servinfo;
    int rv;

    for(int i = 0; i < 50; i=i+2){

	char* key = malloc(2000);
	char* value= malloc(2000);
	char* buf = malloc(MAXDATASIZE);

        strcpy(key, &randomMovies[i][0]);
        int kl = strlen(key);

        strcpy(value, &randomMovies[i+1][0]);
        int vl= strlen(value);

	int headerSize = 48 + kl*sizeof(char) + vl*sizeof(char);
	char* sendHeader = malloc(headerSize);

        sendHeader[0] = 0;
        sendHeader[0] |= 1 << 1;        // set set-bit to 1

        sendHeader[1] = i;      // write tid in header

        sendHeader[2] = ( kl >> 8 );              // set keyLength
        sendHeader[3] = ( kl & 0xFF );

        sendHeader[4] = ( vl >> 8 );              // set keyLength
        sendHeader[5] = ( vl & 0xFF );

        strncpy( &sendHeader[6], key, kl );
        strncpy( &sendHeader[6+kl*sizeof(char)], value, vl );


	memset(&hints, 0, sizeof hints);
      
        // AF_UNSPEC = IPv4 and IPv6
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
      
      
        if ((rv = getaddrinfo(domain, port, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return;
        }


	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
         
            // Find Socket
            if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
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
            return ;
        }
     
        inet_ntop((*p).ai_family, get_in_addr((struct sockaddr *)(*p).ai_addr),
              s, sizeof s);
        printf("\nclient: connecting to %s\n", s);
     
        freeaddrinfo(servinfo); // all done with this structure, free the linked-list

	if (send(sockfd, sendHeader, headerSize, 0) == -1){
            printf("Fehler send\n");
            perror("send");
        }

	clock_t start, end;
	start = clock();

        if ((numbytes = recv(sockfd, buf, 10000, 0)) == -1) {
            perror("recv");
            exit(1);
        }

	end = clock();

	double realTime = ((double)(end - start))/CLOCKS_PER_SEC;

	if (realTime > 2){	// send same message again when time is above 2 seconds
     		i = i-2;
	}

	unmarshall(buf);

        close(sockfd);

	free(buf);
	free(key);
	free(value);
	free(sendHeader);


    }

}

void getMovies( char** randomMovies, char* domain, char* port ){

    int numbytes;
    int sockfd;
    char s[INET6_ADDRSTRLEN];
    struct addrinfo hints, *p, *servinfo;
    int rv;

    for(int i = 0; i < 50; i=i+2){

        char* key = malloc(2000);
        char* value = malloc(2000);
        char* buf = malloc(MAXDATASIZE);

        strcpy(key, &randomMovies[i][0]);
        int kl = strlen(key);

        strcpy(value, &randomMovies[i+1][0]);
        int vl= strlen(value);

        int headerSize = 48 + kl*sizeof(char) + vl*sizeof(char);
        char* sendHeader = malloc(headerSize);

        sendHeader[0] = 0;
        sendHeader[0] |= 1 << 2;        // set get-bit to 1

        sendHeader[1] = i;      // write tid in header

        sendHeader[2] = ( kl >> 8 );              // set keyLength
        sendHeader[3] = ( kl & 0xFF );

        sendHeader[4] = ( vl >> 8 );              // set keyLength
        sendHeader[5] = ( vl & 0xFF );

        strncpy( &sendHeader[6], key, kl );
        strncpy( &sendHeader[6+kl*sizeof(char)], value, vl );



        memset(&hints, 0, sizeof hints);

        // AF_UNSPEC = IPv4 and IPv6
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;


        if ((rv = getaddrinfo(domain, port, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return;
        }


        // loop through all the results and connect to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {

            // Find Socket
            if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
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
            return ;
        }

        inet_ntop((*p).ai_family, get_in_addr((struct sockaddr *)(*p).ai_addr),
              s, sizeof s);
        printf("\nclient: connecting to %s\n", s);

        freeaddrinfo(servinfo); // all done with this structure, free the linked-list

        if (send(sockfd, sendHeader, headerSize, 0) == -1){
            printf("Fehler send\n");
            perror("send");
        }

        clock_t start, end;
        start = clock();

        if ((numbytes = recv(sockfd, buf, 10000, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        end = clock();

        double realTime = ((double)(end - start))/CLOCKS_PER_SEC;

        if (realTime > 2){      // send same message again when time is above 2 seconds
                i = i-2;
        }

        unmarshall(buf);

        close(sockfd);

        free(buf);
        free(key);
        free(value);
        free(sendHeader);

	}

}

void deleteMovies( char** randomMovies, char* domain, char* port ){

    int numbytes;
    int sockfd;
    char s[INET6_ADDRSTRLEN];
    struct addrinfo hints, *p, *servinfo;
    int rv;

    for(int i = 0; i < 50; i=i+2){

        char* key = malloc(2000);
        char* value = malloc(2000);
        char* buf = malloc(MAXDATASIZE);

        strcpy(key, &randomMovies[i][0]);
        int kl = strlen(key);

        strcpy(value, &randomMovies[i+1][0]);
        int vl= strlen(value);

        int headerSize = 48 + kl*sizeof(char) + vl*sizeof(char);
        char* sendHeader = malloc(headerSize);

        sendHeader[0] = 0;
        sendHeader[0] |= 1 << 0;        // set delete-bit to 1

        sendHeader[1] = i;      // write tid in header

        sendHeader[2] = ( kl >> 8 );              // set keyLength
        sendHeader[3] = ( kl & 0xFF );

        sendHeader[4] = ( vl >> 8 );              // set keyLength
        sendHeader[5] = ( vl & 0xFF );

        strncpy( &sendHeader[6], key, kl );
        strncpy( &sendHeader[6+kl*sizeof(char)], value, vl );



        memset(&hints, 0, sizeof hints);

        // AF_UNSPEC = IPv4 and IPv6
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;


        if ((rv = getaddrinfo(domain, port, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return;
        }


        // loop through all the results and connect to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {

            // Find Socket
            if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
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
            return ;
        }

        inet_ntop((*p).ai_family, get_in_addr((struct sockaddr *)(*p).ai_addr),
              s, sizeof s);
        printf("\nclient: connecting to %s\n", s);

        freeaddrinfo(servinfo); // all done with this structure, free the linked-list

        if (send(sockfd, sendHeader, headerSize, 0) == -1){
            printf("Fehler send\n");
            perror("send");
        }

        clock_t start, end;
        start = clock();

        if ((numbytes = recv(sockfd, buf, 10000, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        end = clock();

        double realTime = ((double)(end - start))/CLOCKS_PER_SEC;

        if (realTime > 2){      // send same message again when time is above 2 seconds
                i = i-2;
        }


        unmarshall(buf);

        close(sockfd);

        free(buf);
        free(key);
        free(value);
        free(sendHeader);

        }

}



void unmarshall( char* buf ){

	char operations = buf[0];
	int checkBit = 8;
	for( int i = 0; i < 4; i++ ){
		if( operations & checkBit ){

			switch(i) {
                    		case 0:
                        		printf("ack:1\n");
                        		break;

                    		case 1:
					printf("get:1\n");
                        		break;

                    		case 2:
					printf("set:1\n");
                        		break;

                    		case 3:
					printf("del:1\n");
                        		break;

                    		default:
                        		printf("Keine Operation\n"); break;
                	}

		}
		else{

			switch(i) {
                                case 0:
                                        printf("ack:0\n");
                                        break;

                                case 1:
                                        printf("get:0\n");
                                        break;

                                case 2:
                                        printf("set:0\n");
                                        break;

                                case 3:
                                        printf("del:0\n");
                                        break;

                                default:
                                        printf("Keine Operation\n"); break;
                        }

		}
		checkBit = checkBit >> 1;
	}

        // get keyLength from buffer
        int keyLength = buf[2];
        keyLength = keyLength << 8;
        if( buf[3] >= 0 ){
                keyLength = keyLength + buf[3];
        }
        else{
                keyLength += buf[3]+256;
        }

        // get valueLength from buffer
        int valueLength = buf[4];
        valueLength = valueLength << 8;
        if( buf[5] >= 0 ){
                valueLength += buf[5];
        }
        else{
                valueLength += buf[5]+256;
        }

        char* key = malloc(keyLength+1);
        strncpy(key, &buf[6], keyLength);
        key[keyLength] = '\0';

        char* value = malloc(valueLength+1);
        strncpy(value, &buf[6+keyLength], valueLength);
        value[valueLength] = '\0';

	printf("key:%s\n", key);
	printf("keyLen:%d\n", keyLength);
        printf("value:%s\n", value);
        printf("valueLen:%d\n", valueLength);
	char tid = buf[1];
	printf("transaction id:%d\n", tid);

}
