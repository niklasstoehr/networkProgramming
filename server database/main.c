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


// Hashtable Setup
// orientiert an https://gist.github.com/tonious/1377667
//____________________________________________


// New Hashtable
typedef struct hashtable hashtable_Element;

struct hashtable {
    int size;
    struct entry_s **table;
};


// New Entry

typedef struct entry_s entry_t;

struct entry_s {
    char *key;
    char *value;
    
    // linked List falls hash gleich für verschiedene keys
    struct entry_s *next;
    
};


//____________________________________________

// New Hashtable

// size of hashtable
hashtable_Element *ht_create( int size ) {
    
    hashtable_Element *hashtable = NULL;
    int i;
    
    // size too small
    if( size < 1 ) return NULL;
    
    // Größe des Hashtables allokieren
    if( ( hashtable = malloc( sizeof( hashtable_Element ) ) ) == NULL ) {
        return NULL;
    }
    
    // Pointer auf die Headnodes des Tables
    if( ( hashtable->table = malloc( sizeof( entry_t * ) * size ) ) == NULL ) {
        return NULL;
    }
    
    // Hashtable als Array fühlen
    for( i = 0; i < size; i++ ) {
        hashtable->table[i] = NULL;
    }
    
    // set Hashtable Attribut Size
    hashtable->size = size;
    
    return hashtable;
}



// Hashfunction (leicht variiert, übergibt Hashtable um auf Hashtable->Size zuzugreifen)
unsigned int ht_hash (hashtable_Element *hashtable, const char *str, unsigned int length)
{
    unsigned int hash = 5381;
    unsigned int i = 0;
    int sizeHashtable = hashtable -> size;
    for (i = 0; i < length; ++str, ++i){
        hash = ((hash << 5) + hash) + (*str);
    }
    
    // Bildet den Hashwert auf die Größe des Hashtables ab
    hash = hash % sizeHashtable;
    
    return hash;
}





// Key-Value Eintrag generieren
entry_t *newpair_hashtable( char *key, char *value ) {
    
    entry_t *newpair;
    
    // assign size
    if( ( newpair = malloc( sizeof( entry_t ) ) ) == NULL ) {
        return NULL;
    }
    
    // assign key
    if( ( newpair->key = strdup( key ) ) == NULL ) {
        return NULL;
    }
    
    // assign value
    if( ( newpair->value = strdup( value ) ) == NULL ) {
        return NULL;
    }
    
    // Verweis auf next
    newpair->next = NULL;
    
    return newpair;
}


// Key-Value Pair eintragen in Hashtable
void set_hashtable( hashtable_Element *hashtable, char *key, char *value ) {
    
    // Bin im Hashtable
    int bin = 0;
    
    // neues Paar im Hashtable
    entry_t *newpair = NULL;
    
    // neues Paar im Hashtable
    entry_t *next = NULL;
    entry_t *last = NULL;
    
    int keyLength = strlen(key);
    
    // Bin in Hashtable ermitteln über Hashfunktion
    bin = ht_hash( hashtable, key, keyLength);
    
    // mehrere Einträge für gleichen Hash-Bin?
    next = hashtable->table[ bin ];
    
    // while (es gibt verschiedene elemente in einem bin und auch keys sind dafür gespeichert und der neue key ist größer als der gespeicherte)
    while( next != NULL && next->key != NULL && strcmp( key, next->key ) > 0 ) {
        
        // speichere temporär eintrag in last
        last = next;
        
        // such weiter im bin
        next = next->next;
    }
    
    // gleicher bin (gleicher hash) und sogar auch gleicher key
    if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {
        
        // das element wird einfach ersetzt
        next->value = strdup( value );
        
        // im gegebenen Bin ist noch kein Eintrag
    } else {
        
        // neuer Eintrag wird im Bin erstellt
        newpair = newpair_hashtable( key, value );
        
        // Anfang der linkedList in diesem Bin
        if( next == hashtable->table[ bin ] ) {
            newpair->next = next;
            hashtable->table[ bin ] = newpair;
            
            // Ende der linkedList in diesem Bin
        } else if ( next == NULL ) {
            last->next = newpair;
            
            // In der Mitte der linkedList
        } else  {
            newpair->next = next;
            last->next = newpair;
        }
    }
    
    // Neuer Eintrag wurde aufgenommen
    printf("%s wurde aufgenommen\n", hashtable->table[bin]->key);
    
}


// retrieve key from hashtable
char *get_entry( hashtable_Element *hashtable, char *key ) {
    int bin = 0;
    entry_t *pair;
    
    // hash bin
    bin = ht_hash( hashtable, key, strlen(key));
    
    // browse through bin searching for value
    
    if( hashtable->table[ bin ] == NULL ){
        return NULL;
    }
    
    pair = hashtable->table[ bin ];
    while( pair != NULL && pair->key != NULL && strcmp( key, pair->key ) > 0 ) {
        pair = pair->next;
    }
    
    // gefundes pair existiert nicht oder der key ist nicht gespeichert oder key entspricht nicht dem gesuchten
    if( pair == NULL || pair->key == NULL || strcmp( key, pair->key ) != 0 ) {
        return NULL;
        
    } else {
        
        // Paar gefunden
        return pair->value;
    }
    
}


// Entry löschen
int delete_entry( hashtable_Element *hashtable, char *deleteKey ) {
    int bin = 0;
    
    // Verweis auf Entry vor dem zu löschenden Entry
    entry_t *last = NULL;
    entry_t *pair;
    
    // Delete Checker, hat keine Funktionalität
    char* name = malloc(200);
    strcpy(name, deleteKey);
    
    // hash bin of key you want to delete
    bin = ht_hash( hashtable, deleteKey, strlen(deleteKey));
    
    //nach Wert suchen und temp speichern
    pair = hashtable->table[ bin ];
    last = pair;
    
    // wenn pair existiert und key existiert und deleteKey ist > aktueller Key
    while( pair != NULL && pair->key != NULL && strcmp( deleteKey, pair->key ) > 0 ) {
        
        // Gehe weiter in Liste und speichere das vordere Pair in Last
        pair = pair->next;
        last = pair;
    }
    
    // wenn pair existiert und key existiert und deleteKey ist < aktueller Key
    if( pair == NULL || pair->key == NULL || strcmp( deleteKey, pair->key ) < 0 ) {
        
        // Element exisitiert nicht oder wurde schon gelöscht
        printf("already deleted or does not exist %s\n\n", name);
        return -1;
        
    } else {
        
        // sonst: Verweis von last auf next->next
        last -> next = pair -> next;
        
        //Paar löschen
        pair -> key = NULL;
        printf("%s was deleted successfully\n\n", name);
        return 0;
    }
}


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

void sendAck(int new_fd, char tid, int get, char* key, char* value, int kl, int vl, char op){
    
    char* sendHeader;
    
    if( get == 0 ){
        
        sendHeader = malloc(48);
        
        sendHeader[0] = op;
        sendHeader[0] |= 1 << 3;        // set acknoledge bit to 1
        
        sendHeader[1] = tid;      // write tid in header
        
        for( int i = 2; i < 6; i++ ){
            sendHeader[i] = 0;
        }
        
    }
    else{
        
        sendHeader = malloc(48+(kl+vl)*sizeof(char));
        
        sendHeader[0] = op;
        sendHeader[0] |= 1 << 3;        // set acknoledge bit to 1
        
        sendHeader[1] = tid;      // write tid in header
        
        
        
        sendHeader[2] = ( kl >> 8 );              // keyLength = 0
        sendHeader[3] = ( kl & 0xFF );
        
        sendHeader[4] = ( vl >> 8 );              // valueLength = 0
        sendHeader[5] = ( vl & 0xFF );
        
        strncpy( &sendHeader[6], key, kl );
        strncpy( &sendHeader[6+kl*sizeof(char)], value, vl );
        
    }
    
    if (send(new_fd, sendHeader, 48, 0) == -1)
        perror("send");
    
    
    free(sendHeader);
    
    return;
    
}


int main(int argc, char * argv[]) {
    
    hashtable_Element *hashtable = ht_create( 65536 );
    
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    int maxBufLength = 10000;
    char* buffer = malloc(maxBufLength);
    
    
    // Korrekte Anzahl der Konsolenargumente: 1
    if (argc != 2) {
        fprintf(stderr,"Bitte Port übergeben\n");
        exit(1);
    }
    
    
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
        
        
        if (recv(new_fd, buffer, maxBufLength, 0) == -1){
            perror("recv");
        }
        
        // get keyLength from buffer
        int keyLength = buffer[2];
        keyLength = keyLength << 8;
        keyLength = keyLength + buffer[3];
        
        // get valueLength from buffer
        int valueLength = buffer[4];
        valueLength = valueLength << 8;
        valueLength = valueLength + buffer[5];
        
        // get transactionID from buffer
        char tid = buffer[1];
        
        char* key = malloc(keyLength+1);
        strncpy(key, &buffer[6], keyLength);
        key[keyLength] = '\0';
        
        char* value = malloc(valueLength+1);
        strncpy(value, &buffer[6+keyLength], valueLength);
        value[valueLength] = '\0';
        
        int op = 0;
        int numBit = 0;
        char operations = buffer[0];
        operations = operations << 4;
        int get = 0;
        int del = 0;
        
        int getOp = 0;
        int setOp = 0;
        int deleteOp = 0;
        
        while(numBit < 4){
            
            if( operations & 0x80 ){
                
                switch(op) {
                    case 0:
                        printf("ack kann nicht ausgeführt werden\n");
                        break;
                        
                    case 1:
                        getOp = 1;
                        break;
                        
                    case 2:
                        setOp = 1;
                        break;
                        
                    case 3:
                        deleteOp = 1;
                        break;
                        
                    default:
                        printf("Keine Operation\n"); break;
                }
                
            }
            
            op++;
            numBit++;
            operations = operations << 1;
            
        }
        
        if( setOp == 1 ){
            printf("Operation: set\n");
            set_hashtable( hashtable, key, value );
            setOp = 0;
        }
        
        
        if( getOp == 1 ){
            printf("Operation: get\n");
            value = get_entry(hashtable, key );
            if(value != NULL){
                valueLength = strlen(value);
            }
            else{
                valueLength = 0;
                del = -1;
            }
            get = 1;
            getOp = 0;
        }
        
        
        
        if( deleteOp == 1 ){
            printf("Operation: delete\n");
            del = delete_entry( hashtable, key);
            deleteOp = 0;
        }
        
        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            
            if( del == 0 ){
                sendAck(new_fd, tid, get, key, value, keyLength, valueLength, buffer[0]);
            }
            
            close(new_fd);
            
            exit(0);
            
        }
        close(new_fd);  // parent doesn't need this
    }
    
    free(buffer);
    
    return 0;
}


