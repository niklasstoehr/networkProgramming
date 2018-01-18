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
 
 
hashtable_Element *ht_create( int size );
//unsigned int ht_hash (hashtable_Element *hashtable, const char *str, unsigned int length);
entry_t *newpair_hashtable( char *key, char *value );
void set_hashtable( hashtable_Element *hashtable, char *key, char *value );
char* get_entry( hashtable_Element *hashtable, char *key );
int delete_entry( hashtable_Element *hashtable, char *deleteKey );
unsigned int ht_hash (int sizeHashtable, const char *filmname, unsigned int length);
