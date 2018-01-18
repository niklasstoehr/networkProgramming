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
#include "node.h"
#include "hashtable.h"

int looker(int hash, int eigeneID, int pred){

    if(succID < 0 && predID < 0){	// einziger node, immer zuständig
        return 1;
    }

    if(eigeneID - pred > 0){

        if(hash > pred && hash <= eigeneID){
            return 1;
        }

    }
    else{
        if(hash > pred || hash <= eigeneID){
            return 1;
        }
    }

    return 0;

}


int join(int neueID, char* neueIP, char* neuerPort, char* header){


    if( looker( neueID, eigID, predID  ) ) { // in seinem Bereich

        if( succID < 0){

            succID = neueID;
            strcpy(succIP, neueIP);
            strcpy(succPort, neuerPort);

        }

        predID = neueID;
        strcpy(predIP, neueIP);
        strcpy(predPort, neuerPort);



        char* notifyHeader = malloc(10000);

        notifyHeader[0] |= 1 << 7;    // set internal bit
        notifyHeader[0] |= 1 << 5;    // set notify bit

        notifyHeader[6] = eigID >> 8;     // set ID MSB
        notifyHeader[7] = eigID & 0xFF;   // set ID LSB

        int ip = atoi(eigIP);
        notifyHeader[8] = (ip & 0xFF000000) >> 24;
        notifyHeader[9] = (ip & 0x00FF0000) >> 16;
        notifyHeader[10] = (ip & 0x0000FF00) >> 8;
        notifyHeader[11] = ip & 0x000000FF;

        notifyHeader[12] = eigPortInt >> 8;
        notifyHeader[13] = eigPortInt & 0xFF;

        // send notifyHeader to new node

        int notifySocket;

        memset(&hints, 0, sizeof hints);
            
        // AF_UNSPEC = IPv4 and IPv6
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        
        
        if ((rv = getaddrinfo(neueIP, neuerPort, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 0;
        }
      
      
        // loop through all the results and connect to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {
               
                // Find Socket
                if ((notifySocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("client: socket");
                continue;
                }
               
                // bind it to the port we passed in to getaddrinfo():
                // Connect to socket
                if (connect(notifySocket, p->ai_addr, p->ai_addrlen) == -1) {
                close(notifySocket);
                perror("client: connect");
                continue;
                }
               
                break;
        }
           
        // p ist Server
        if (p == NULL) {
            fprintf(stderr, "client: failed to connect\n");
            return 0;
        }
       
        inet_ntop((*p).ai_family, get_in_addr((struct sockaddr *)(*p).ai_addr),
              s, sizeof s);
        printf("\nnode with ID %d connecting to %s\n", eigID, s);
       
        freeaddrinfo(servinfo); // all done with this structure, free the linked-list
      
        if (send(notifySocket, notifyHeader, MAXDATASIZE, 0) == -1){
                printf("Fehler send\n");
                perror("send");
        }
      
        close(notifySocket);


        sleep(1);

        char* stabilizeHeader = malloc(10000);

        stabilizeHeader[0] |= 1 << 7;    // set internal bit
        stabilizeHeader[0] |= 1 << 4;    // set stabilize bit

        stabilizeHeader[6] = eigID >> 8;     // set ID MSB
        stabilizeHeader[7] = eigID & 0xFF;   // set ID LSB

        ip = atoi(eigIP);
        stabilizeHeader[8] = (ip & 0xFF000000) >> 24;
        stabilizeHeader[9] = (ip & 0x00FF0000) >> 16;
        stabilizeHeader[10] = (ip & 0x0000FF00) >> 8;
        stabilizeHeader[11] = ip & 0x000000FF;

        stabilizeHeader[12] = eigPortInt >> 8;
        stabilizeHeader[13] = eigPortInt & 0xFF;

        // send stabilizeHeader to successor

        int stabSocket;

        memset(&hints, 0, sizeof hints);
            
        // AF_UNSPEC = IPv4 and IPv6
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        
        
        if ((rv = getaddrinfo(succIP, succPort, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 0;
        }
      
      
        // loop through all the results and connect to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {
               
                // Find Socket
                if ((stabSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("client: socket");
                continue;
                }
               
                // bind it to the port we passed in to getaddrinfo():
                // Connect to socket
                if (connect(stabSocket, p->ai_addr, p->ai_addrlen) == -1) {
                close(stabSocket);
                perror("client: connect");
                continue;
                }
               
                break;
        }
           
        // p ist Server
        if (p == NULL) {
            fprintf(stderr, "client: failed to connect\n");
            return 0;
        }
       
        inet_ntop((*p).ai_family, get_in_addr((struct sockaddr *)(*p).ai_addr),
              s, sizeof s);
        printf("\nnode with ID %d: connecting to %s\n", eigID, s);
       
        freeaddrinfo(servinfo); // all done with this structure, free the linked-list
      
        if (send(stabSocket, stabilizeHeader, MAXDATASIZE, 0) == -1){
                printf("Fehler send\n");
                perror("send");
        }
      
        close(stabSocket);

        return 0;

    }
    else{

        // send joinHeader to successor

        int joinSocket;

        memset(&hints, 0, sizeof hints);
            
        // AF_UNSPEC = IPv4 and IPv6
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        
        
        if ((rv = getaddrinfo(succIP, succPort, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 0;
        }
      
      
        // loop through all the results and connect to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {
               
                // Find Socket
                if ((joinSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("client: socket");
                continue;
                }
               
                // bind it to the port we passed in to getaddrinfo():
                // Connect to socket
                if (connect(joinSocket, p->ai_addr, p->ai_addrlen) == -1) {
                close(joinSocket);
                perror("client: connect");
                continue;
                }
               
                break;
        }
           
        // p ist Server
        if (p == NULL) {
            fprintf(stderr, "client: failed to connect\n");
            return 0;
        }
       
        inet_ntop((*p).ai_family, get_in_addr((struct sockaddr *)(*p).ai_addr),
              s, sizeof s);
        printf("\nnode with ID %d: connecting to %s\n", eigID, s);
       
        freeaddrinfo(servinfo); // all done with this structure, free the linked-list
      
        if (send(joinSocket, header, MAXDATASIZE, 0) == -1){
                printf("Fehler send\n");
                perror("send");
        }
      
        close(joinSocket);

    }
    return 0;
            
}


int notify(int notifyerID, char* notifyerIP, char* notifyerPort){

    if(succID < 0){

        succID = notifyerID;
        strcpy(succIP, notifyerIP);
        strcpy(succPort, notifyerPort);

    }

    else{

        succID = notifyerID;
        strcpy(succIP, notifyerIP);
        strcpy(succPort, notifyerPort);


        char* stabilizeHeader = malloc(10000);

        stabilizeHeader[0] |= 1 << 7;    // set internal bit
        stabilizeHeader[0] |= 1 << 4;    // set stabilize bit

        stabilizeHeader[6] = eigID >> 8;     // set ID MSB
        stabilizeHeader[7] = eigID & 0xFF;   // set ID LSB

        int ip = atoi(eigIP);
        stabilizeHeader[8] = (ip & 0xFF000000) >> 24;
        stabilizeHeader[9] = (ip & 0x00FF0000) >> 16;
        stabilizeHeader[10] = (ip & 0x0000FF00) >> 8;
        stabilizeHeader[11] = ip & 0x000000FF;
        stabilizeHeader[12] = eigPortInt >> 8;
        stabilizeHeader[13] = eigPortInt & 0xFF;

        // send stabilizeHeader to successor

        int stabSocket;

        memset(&hints, 0, sizeof hints);
            
        // AF_UNSPEC = IPv4 and IPv6
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        
        
        if ((rv = getaddrinfo(succIP, succPort, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 0;
        }
      
      
        // loop through all the results and connect to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {
               
                // Find Socket
                if ((stabSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("client: socket");
                continue;
                }
               
                // bind it to the port we passed in to getaddrinfo():
                // Connect to socket
                if (connect(stabSocket, p->ai_addr, p->ai_addrlen) == -1) {
                close(stabSocket);
                perror("client: connect");
                continue;
                }
               
                break;
        }
           
        // p ist Server
        if (p == NULL) {
            fprintf(stderr, "client: failed to connect\n");
            return 0;
        }
       
        inet_ntop((*p).ai_family, get_in_addr((struct sockaddr *)(*p).ai_addr),
              s, sizeof s);
        printf("\nnode with ID %d: connecting to %s\n", eigID, s);
       
        freeaddrinfo(servinfo); // all done with this structure, free the linked-list
      
        if (send(stabSocket, stabilizeHeader, MAXDATASIZE, 0) == -1){
                printf("Fehler send\n");
                perror("send");
        }
      
        close(stabSocket);
    }

    return 0;
}


int stabilize(int incomingID, char* incomingIP, char* incomingPort){

    int stabSocket;

    if( incomingID == predID ){

        char* stabilizeHeader = malloc(10000);

        stabilizeHeader[0] |= 1 << 7;    // set internal bit
        stabilizeHeader[0] |= 1 << 4;    // set stabilize bit

        stabilizeHeader[6] = eigID >> 8;     // set ID MSB
        stabilizeHeader[7] = eigID & 0xFF;   // set ID LSB

        int ip = atoi(eigIP);
        stabilizeHeader[8] = (ip & 0xFF000000) >> 24;
        stabilizeHeader[9] = (ip & 0x00FF0000) >> 16;
        stabilizeHeader[10] = (ip & 0x0000FF00) >> 8;
        stabilizeHeader[11] = ip & 0x000000FF;

        stabilizeHeader[12] = eigPortInt >> 8;
        stabilizeHeader[13] = eigPortInt & 0xFF;

        // send stabilizeHeader to successor

        memset(&hints, 0, sizeof hints);
            
        // AF_UNSPEC = IPv4 and IPv6
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        
        
        if ((rv = getaddrinfo(succIP, succPort, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 0;
        }
      
      
        // loop through all the results and connect to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {
               
                // Find Socket
                if ((stabSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("client: socket");
                continue;
                }
               
                // bind it to the port we passed in to getaddrinfo():
                // Connect to socket
                if (connect(stabSocket, p->ai_addr, p->ai_addrlen) == -1) {
                close(stabSocket);
                perror("client: connect");
                continue;
                }
               
                break;
        }
           
        // p ist Server
        if (p == NULL) {
            fprintf(stderr, "client: failed to connect\n");
            return 0;
        }
       
        inet_ntop((*p).ai_family, get_in_addr((struct sockaddr *)(*p).ai_addr),
              s, sizeof s);
        printf("\nnode with ID %d: connecting to %s\n", eigID, s);
       
        freeaddrinfo(servinfo); // all done with this structure, free the linked-list
      
        if (send(stabSocket, stabilizeHeader, MAXDATASIZE, 0) == -1){
                printf("Fehler send\n");
                perror("send");
        }
      
        close(stabSocket);

    }

    if ( predID < 0 ){

        predID = incomingID;
        strcpy(predIP, incomingIP);
        strcpy(predPort, incomingPort);

        return 0;

    }


    if( predID != incomingID && predID >= 0 ){

        char* notifyHeader = malloc(10000);

        notifyHeader[0] |= 1 << 7;    // set internal bit
        notifyHeader[0] |= 1 << 5;    // set notify bit

        notifyHeader[6] = predID >> 8;     // set ID MSB
        notifyHeader[7] = predID & 0xFF;   // set ID LSB

        int ip = atoi(predIP);
        notifyHeader[8] = (ip & 0xFF000000) >> 24;
        notifyHeader[9] = (ip & 0x00FF0000) >> 16;
        notifyHeader[10] = (ip & 0x0000FF00) >> 8;
        notifyHeader[11] = ip & 0x000000FF;

        int predPortInt = atoi(predPort);
        notifyHeader[12] = predPortInt >> 8;
        notifyHeader[13] = predPortInt & 0xFF;

        // send notifyHeader with predecessor to incomingNode

        memset(&hints, 0, sizeof hints);
            
        // AF_UNSPEC = IPv4 and IPv6
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        
        
        if ((rv = getaddrinfo(incomingIP, incomingPort, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 0;
        }
      
      
        // loop through all the results and connect to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {
               
                // Find Socket
                if ((stabSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("client: socket");
                continue;
                }
               
                // bind it to the port we passed in to getaddrinfo():
                // Connect to socket
                if (connect(stabSocket, p->ai_addr, p->ai_addrlen) == -1) {
                close(stabSocket);
                perror("client: connect");
                continue;
                }
               
                break;
        }
           
        // p ist Server
        if (p == NULL) {
            fprintf(stderr, "client: failed to connect\n");
            return 0;
        }
       
        inet_ntop((*p).ai_family, get_in_addr((struct sockaddr *)(*p).ai_addr),
              s, sizeof s);
        printf("\nnode with ID %d: connecting to %s\n", eigID, s);
       
        freeaddrinfo(servinfo); // all done with this structure, free the linked-list
      
        if (send(stabSocket, notifyHeader, MAXDATASIZE, 0) == -1){
                printf("Fehler send\n");
                perror("send");
        }
      
        close(stabSocket);

    }

    return 0;
}

void sigchld_handler(int s){
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
       
    while(waitpid(-1, NULL, WNOHANG) > 0);
       
    errno = saved_errno;
}
   
   
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
       
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
   
int main(int argc, char * argv[]) {
       
    hashtable_Element *hashtable = ht_create( 65536 );

    char* bekPort = malloc(32*sizeof(char));
    eigPort = malloc(32*sizeof(char));
	
    // IP auslesen
    unsigned int ip[4];

    char* ipArg = malloc(4*sizeof(char));

    strcpy(ipArg, argv[1]);

    if( strcmp(ipArg, "localhost") == 0 ){
        ipArg = "127.0.0.1";
    }

    sscanf(ipArg, "%u.%u.%u.%u", &ip[0], &ip[1], &ip[2], &ip[3]);

    eigIpInt = ip[3] + ip[2] * 0x100 + ip[1] * 0x10000ul + ip[0] * 0x1000000ul;
    eigIP = malloc(32*sizeof(char));
    sprintf(eigIP, "%d", eigIpInt);

    // Port auslesen
    strcpy(eigPort, argv[2]);
    eigPortInt = atoi(argv[2]);

    if( argc > 3 ){
        // ID auslesen
        eigID = atoi(argv[3]);
    }
    else{
        eigID = 0;
    }


    succIP = malloc(32*sizeof(char));
    succPort = malloc(32*sizeof(char));

    predIP = malloc(32*sizeof(char));
    predPort = malloc(32*sizeof(char));

    char* buffer = malloc(MAXDATASIZE);

    // falls ein node bekannt ist, leite join ein
    if( argc == 6 ){

        // bekannte IP auslesen
        unsigned int bekIP[4];

        char* bekIpArg = malloc(4*sizeof(char));

        strcpy(bekIpArg, argv[4]);

        if( strcmp(bekIpArg, "localhost") == 0 ){
            bekIpArg = "127.0.0.1";
        }

        sscanf(bekIpArg, "%u.%u.%u.%u", &bekIP[0], &bekIP[1], &bekIP[2], &bekIP[3]);

        uint32_t bekIpInt = bekIP[3] + bekIP[2] * 0x100 + bekIP[1] * 0x10000ul + bekIP[0] * 0x1000000ul;
        char* bekIpString = malloc(32*sizeof(char));
        sprintf(bekIpString, "%d", bekIpInt);

        // bekannten Port auslesen
        strcpy(bekPort, argv[5]);



        char* joinHeader = malloc(10000);

        joinHeader[0] |= 1 << 7;    // set internal bit
        joinHeader[0] |= 1 << 6;    // set join bit

        joinHeader[6] = eigID >> 8;     // set ID MSB
        joinHeader[7] = eigID & 0xFF;   // set ID LSB

        joinHeader[8] = ip[0];
        joinHeader[9] = ip[1];
        joinHeader[10] = ip[2];
        joinHeader[11] = ip[3];

        joinHeader[12] = eigPortInt >> 8;
        joinHeader[13] = eigPortInt & 0xFF;


        memset(&hints, 0, sizeof hints);
            
        // AF_UNSPEC = IPv4 and IPv6
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        
        
        if ((rv = getaddrinfo(bekIpString, bekPort, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 0;
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
            return 0;
        }
       
        inet_ntop((*p).ai_family, get_in_addr((struct sockaddr *)(*p).ai_addr),
              s, sizeof s);
        printf("\nnode with ID %d: connecting to %s\n", eigID, s);
       
        freeaddrinfo(servinfo); // all done with this structure, free the linked-list
      
        if (send(sockfd, joinHeader, MAXDATASIZE, 0) == -1){
                printf("Fehler send\n");
                perror("send");
        }
      
        close(sockfd);

    }


    // starte node als server und warte auf Verbindung


       
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
       
    if ((rv = getaddrinfo(eigIP, eigPort, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
       
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((someSock = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
           
        if (setsockopt(someSock, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
           
        if (bind(someSock, p->ai_addr, p->ai_addrlen) == -1) {
            close(someSock);
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
       
    if (listen(someSock, BACKLOG) == -1) {
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
       
    printf("node with ID %d: waiting for connections...\n", eigID);
       
       
    while(1) {  // main accept() loop
           
        loop:

        sin_size = sizeof their_addr;
        new_fd = accept(someSock, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
           
        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("node with ID %d: got connection from %s\n", eigID, s);
           
           
        if ((numbytes = recv(new_fd, buffer, 10000, 0)) == -1){
            perror("recv");
        }
 

        if( buffer[0] & 128 ){ // Internalbit = 1

            // auslesen von ID, IP und Port aus dem Header
            int headerID = buffer[6] << 8;
            unsigned char temp = buffer[7];
            headerID = headerID + temp;

            uint32_t headerIpInt = buffer[11] + buffer[10] * 0x100 + buffer[9] * 0x10000ul + buffer[8] * 0x1000000ul;
            char* headerIP = malloc(32*sizeof(char));
            sprintf(headerIP, "%d", headerIpInt);

            char* headerPort = malloc(32*sizeof(char));
            int headerPortInt = buffer[12] << 8;
            if( buffer[13] >= 0 ){
                headerPortInt = headerPortInt + buffer[13];
            }
            else{
                headerPortInt += buffer[13]+256;
            }
            sprintf(headerPort, "%d", headerPortInt);

            int keyLength = buffer[2] << 8;
            temp = buffer[3];
            keyLength = keyLength + temp;
            int valueLength = buffer[4] << 8;
            temp = buffer[5];
            valueLength = valueLength + temp;

            char* key = malloc(MAXDATASIZE);
            char* value = malloc(MAXDATASIZE);

            strncpy(key, &buffer[14], keyLength);
            strncpy(value, &buffer[14+keyLength], valueLength);

            uint32_t hash = ht_hash(hashSize, key, keyLength);

            if( buffer[0] & 64 ){   // joinBit = 1
                join(headerID, headerIP, headerPort, buffer);
                close(new_fd);
                goto loop;
            }


            if( buffer[0] & 32 ){   // notifyBit = 1
                notify(headerID, headerIP, headerPort);
                close(new_fd);
                goto loop;
            }

            if( buffer[0] & 16 ){   // stabilizeBit = 1
                stabilize(headerID, headerIP, headerPort);
                close(new_fd);
                goto loop;
            }

            // header wurde von einem anderen node weitergereicht

            // prüfen ob headerID mit eigID übereinstimmt
            if( headerID == eigID ){

            	// internalHeader in clientHeader umwandeln und ack bit setzen
            	char* clientHeader = malloc(MAXDATASIZE);

            	clientHeader[0] = buffer[0] & 0xF;
            	clientHeader[0] |= 1 << 3;    // set ack bit

            	clientHeader[1] = buffer[1];	// tid

            	clientHeader[2] = buffer[2];	// keyLength
            	clientHeader[3] = buffer[3];	// keyLength

            	clientHeader[4] = buffer[4];	// valueLength
            	clientHeader[5] = buffer[5];	// valueLength

            	strncpy(&clientHeader[6], key, keyLength);
            	strncpy(&clientHeader[6+keyLength], value, valueLength);

            	// zurück an clientSock
		        if (send(clientSock, clientHeader, MAXDATASIZE, 0) == -1){
		                printf("Fehler send\n");
		                perror("send");
		        }
		      
		        close(clientSock);

            }
            else{
            	// prüfen auf zuständigkeit
            	if ( looker(hash, eigID, predID)  == 1 ){		// Internal = 1 und zuständig

            		// führe operationen get, set, delete aus
            		char operations = buffer[0];

                    int checkBit = 8;

                    for( int i = 0; i < 4; i++ ){
                        if( operations & checkBit ){
                              
                            switch(i) {
                                case 0:
                                    printf("ack\n");
                                    break;
                                      
                                case 1:
                                    printf("node with ID %d: get\n", eigID);
				    value = get_entry(hashtable, key);
				    if (value == NULL) printf("couldn't get value for %s\n", key);
				    else printf("got value for %s successfully\n", key);
                                    break;
                                      
                                case 2:
                                    printf("node with ID %d: set\n", eigID);
                                    set_hashtable(hashtable, key, value);
                                    break;
                                      
                                case 3:
                                    printf("node with ID %d: del\n", eigID);
                                    delete_entry(hashtable, key);
                                    break;
                                      
                                default:
                                    printf("Keine Operation\n"); break;
                            }
                          
                    	}
                      
                    	checkBit = checkBit >> 1;
                	}

			if (value == NULL){
				valueLength = 0;
				buffer[4] = 0;
				buffer[5] = 0;
			}
	                else{
				valueLength = strlen(value);
			}
	                strncpy(&buffer[14+keyLength], value, valueLength);


            		// send to headerID node
            		memset(&hints, 0, sizeof hints);
            
			        // AF_UNSPEC = IPv4 and IPv6
			        hints.ai_family = AF_INET;
			        hints.ai_socktype = SOCK_STREAM;
			        
			        
			        if ((rv = getaddrinfo(headerIP, headerPort, &hints, &servinfo)) != 0) {
			            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			            return 0;
			        }
			      
			      
			        // loop through all the results and connect to the first we can
			        for(p = servinfo; p != NULL; p = p->ai_next) {
			               
			                // Find Socket
			                if ((headerSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			                perror("client: socket");
			                continue;
			                }
			               
			                // bind it to the port we passed in to getaddrinfo():
			                // Connect to socket
			                if (connect(headerSocket, p->ai_addr, p->ai_addrlen) == -1) {
			                close(headerSocket);
			                perror("client: connect");
			                continue;
			                }
			               
			                break;
			        }
			           
			        // p ist Server
			        if (p == NULL) {
			            fprintf(stderr, "client: failed to connect\n");
			            return 0;
			        }
			       
			        inet_ntop((*p).ai_family, get_in_addr((struct sockaddr *)(*p).ai_addr),
			              s, sizeof s);
			        printf("\nnode with ID %d: connecting to %s\n", eigID, s);
			       
			        freeaddrinfo(servinfo); // all done with this structure, free the linked-list
			      
			        if (send(headerSocket, buffer, MAXDATASIZE, 0) == -1){
			                printf("Fehler send\n");
			                perror("send");
			        }
			      
			        close(headerSocket);

            	}
            	else{		// nicht zuständig

            		// send to successor
            		memset(&hints, 0, sizeof hints);
            
			        // AF_UNSPEC = IPv4 and IPv6
			        hints.ai_family = AF_INET;
			        hints.ai_socktype = SOCK_STREAM;
			        
			        
			        if ((rv = getaddrinfo(succIP, succPort, &hints, &servinfo)) != 0) {
			            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			            return 0;
			        }
			      
			      
			        // loop through all the results and connect to the first we can
			        for(p = servinfo; p != NULL; p = p->ai_next) {
			               
			                // Find Socket
			                if ((succSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			                perror("client: socket");
			                continue;
			                }
			               
			                // bind it to the port we passed in to getaddrinfo():
			                // Connect to socket
			                if (connect(succSocket, p->ai_addr, p->ai_addrlen) == -1) {
			                close(succSocket);
			                perror("client: connect");
			                continue;
			                }
			               
			                break;
			        }
			           
			        // p ist Server
			        if (p == NULL) {
			            fprintf(stderr, "client: failed to connect\n");
			            return 0;
			        }
			       
			        inet_ntop((*p).ai_family, get_in_addr((struct sockaddr *)(*p).ai_addr),
			              s, sizeof s);
			        printf("\nnode with ID %d: connecting to %s\n", eigID, s);
			       
			        freeaddrinfo(servinfo); // all done with this structure, free the linked-list
			      
			        if (send(succSocket, buffer, MAXDATASIZE, 0) == -1){
			                printf("Fehler send\n");
			                perror("send");
			        }
			      
			        close(succSocket);

            	}

            }



        }
        else{	// internalBit = 0 -> Nachricht vom Client

        	// header auslesen
        	int keyLength = buffer[2] << 8;
            unsigned char temp = buffer[3];
            keyLength = keyLength + temp;
            int valueLength = buffer[4] << 8;
            temp = buffer[5];
            valueLength = valueLength + temp;

            char* key = malloc(MAXDATASIZE);
            char* value = malloc(MAXDATASIZE);

            strncpy(key, &buffer[6], keyLength);
            strncpy(value, &buffer[6+keyLength], valueLength);

            uint32_t hash = ht_hash(hashSize, key, keyLength);

        	// zuständigkeit prüfen
        	if ( looker(hash, eigID, predID) == 1 ){		// zuständig

        		char operations = buffer[0];

        		int checkBit = 8;

        		for( int i = 0; i < 4; i++ ){
                    if( operations & checkBit ){
                          
                        switch(i) {
                            case 0:
                                printf("ack:\n");
                                break;
                                  
                            case 1:
                                printf("node with ID %d: get:\n", eigID);
                                value = get_entry(hashtable, key);
				if (value == NULL ) printf("couldn't get value for %s\n", key);
				else printf("got value for %s successfully\n", key);
                                break;
                                  
                            case 2:
                                printf("node with ID %d: set:\n", eigID);
                                set_hashtable(hashtable, key, value);
                                break;
                                  
                            case 3:
                                printf("node with ID %d: del:\n", eigID);
                                delete_entry(hashtable, key);
                                break;
                                  
                            default:
                                printf("Keine Operation\n"); break;
                        }
                          
                    }
                      
                    checkBit = checkBit >> 1;
                }

			if (value == NULL){
                                valueLength = 0;
                                buffer[4] = 0;
                                buffer[5] = 0;
                        }
                        else{
                                valueLength = strlen(value);
                        }
                        strncpy(&buffer[14+keyLength], value, valueLength);


        		// sende zurück an den Client
        		if (send(new_fd, buffer, MAXDATASIZE, 0) == -1)
        			perror("send");

        		close(new_fd);

        	}
        	else{		// nicht zuständig

        		// sichere den eingegangenen Socket als clientSock für die spätere Antwort an den Client
        		clientSock = new_fd;

        		// packe header in internalHeader
        		char* internalHeader = malloc(MAXDATASIZE);

        		internalHeader[0] = buffer[0];
        		internalHeader[0] |= 1 << 7;    // set internal bit

        		internalHeader[1] = buffer[1];	// tid

        		internalHeader[2] = buffer[2];	// keyLength
        		internalHeader[3] = buffer[3];	// keyLength

        		internalHeader[4] = buffer[4];	// valueLength
        		internalHeader[5] = buffer[5];	// valueLength

        		internalHeader[6] = eigID >> 8;		// eigID
        		internalHeader[7] = eigID & 0xFF;	// eigID

        		internalHeader[8] = ip[0];	// IP
        		internalHeader[9] = ip[1];	// IP
        		internalHeader[10] = ip[2];	// IP
        		internalHeader[11] = ip[3];	// IP

        		internalHeader[12] = eigPortInt >> 8;	// Port
        		internalHeader[13] = eigPortInt & 0xFF;	// Port

        		strncpy(&internalHeader[14], key, keyLength);	// key
        		strncpy(&internalHeader[14+keyLength], value, valueLength);	// value


        		// send to successor
        		memset(&hints, 0, sizeof hints);
            
		        // AF_UNSPEC = IPv4 and IPv6
		        hints.ai_family = AF_INET;
		        hints.ai_socktype = SOCK_STREAM;
		        
		        
		        if ((rv = getaddrinfo(succIP, succPort, &hints, &servinfo)) != 0) {
		            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		            return 0;
		        }
		      
		      
		        // loop through all the results and connect to the first we can
		        for(p = servinfo; p != NULL; p = p->ai_next) {
		               
		                // Find Socket
		                if ((succSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
		                perror("client: socket");
		                continue;
		                }
		               
		                // bind it to the port we passed in to getaddrinfo():
		                // Connect to socket
		                if (connect(succSocket, p->ai_addr, p->ai_addrlen) == -1) {
		                close(succSocket);
		                perror("client: connect");
		                continue;
		                }
		               
		                break;
		        }
		           
		        // p ist Server
		        if (p == NULL) {
		            fprintf(stderr, "client: failed to connect\n");
		            return 0;
		        }
		       
		        inet_ntop((*p).ai_family, get_in_addr((struct sockaddr *)(*p).ai_addr),
		              s, sizeof s);
		        printf("\nnode with ID %d: connecting to %s\n", eigID, s);
		       
		        freeaddrinfo(servinfo); // all done with this structure, free the linked-list
		      
		        if (send(succSocket, internalHeader, MAXDATASIZE, 0) == -1){
		                printf("Fehler send\n");
		                perror("send");
		        }
		      
		        close(succSocket);

        	}


        }



    }
       
//    free(buffer);
       
    return 0;
}
