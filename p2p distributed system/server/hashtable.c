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
#include "hashtable.h"
 
#define BACKLOG 10     // how many pending connections queue will hold
#define MAX_HASH 10
 
// Hashtable Setup
// orientiert an https://gist.github.com/tonious/1377667
//____________________________________________
   
   
// New Hashtable
   
   
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
unsigned int ht_hash ( int sizeHashtable, const char *str, unsigned int length ) {
    unsigned int hash = 5381;
    unsigned int i = 0;
    //int sizeHashtable = hashtable -> size;
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
    bin = ht_hash( hashtable->size, key, keyLength);
 
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
    printf("%s was set \n\n", hashtable->table[bin]->key);
       
}
   
   
// retrieve key from hashtable
char *get_entry( hashtable_Element *hashtable, char *key ) {
    int bin = 0;
    entry_t *pair;
       
    // hash bin
    bin = ht_hash( hashtable->size, key, strlen(key));
       
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
    bin = ht_hash( hashtable->size, deleteKey, strlen(deleteKey));
       
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
