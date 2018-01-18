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
//#include "main.h"
  
#define BACKLOG 10     // how many pending connections queue will hold
#define MAX_HASH 10
#define MAXDATASIZE 100 // max number of bytes we can get at once
#define MAX_MOVIES 100 // maximum amount of movies in CSV
  
  
//Argumente: DOMAIN PORT PATH zum CSV File siehe: /Users/macbookpro/Desktop/client_readIn/client_readIn/filme.csv
  
  
//________________________________________________________________
// Functions
//
//______________________________________________________________
  
  
// CSV Reader Functionality
//________________________________________________________________
  
// CSV Reader
//
//________________________________________________________________
  
  
// gesamten Value der Filme: Titel, Originaltitel, Herstellungsjahr, Laenge, Regie, DarstellerInnen
char* getMovieValue(char* line, int num) {
    char* tok;
      
    // Split at "\n"
    for (tok = strtok(line, "\n"); tok && *tok; tok = strtok(NULL, ";\n")) {
        if (!--num)
            return tok;
    }
    return NULL;
}
  
  
// Titel des Films für den Key des Hashtables
char* getMovieTitle(char* line, int num) {
    char* tok;
      
    // Split at ";"
    for (tok = strtok(line, ";"); tok && *tok; tok = strtok(NULL, ";\n")) {
        if (!--num)
            return tok;
    }
    return NULL;
}
  
  
  
// Network Functionality
//________________________________________________________________
  
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
      
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
  
  
//________________________________________________________________
// Main
//
//________________________________________________________________
  
 
int main(int argc, char *argv[])
{

    // Korrekte Anzahl der Konsolenargumente: 2
    if (argc != 4) {
        fprintf(stderr,"3 Argumente müssen übergeben werden - DOMAIN PORT PATH \n");
        exit(1);
    }
      
      
// CSV Reader Functionality
// Output: 25 random movies are being safed to 2d-array randomMovies[25][2] with [Key][Value]
//________________________________________________________________
      
    FILE* stream = fopen(argv[3], "r");
      
    char line[1000];
    char* movieValues[MAX_MOVIES];
    char* movieTitles[MAX_MOVIES];
    int count = 0;
      
    while (fgets(line, 1000, stream))
    {
        char* tmp = strdup(line);
          
        char* tempValue = (char *) malloc(1000);
        strcpy(tempValue, getMovieValue(tmp, 1));
        movieValues[count] = tempValue;
          
        char* tempTitle = (char *) malloc(1000);
        strcpy(tempTitle, getMovieTitle(tmp, 1));
        movieTitles[count] = tempTitle;
          
        count++;
          
        //printf("\n%s\n", getMovieTitle(tmp, 1));
          
        free(tmp);
    }

  
    //int selectMovie = 40;
    //printf("\n\nValue des Movie #%i ist %s\n",  (selectMovie), movies[selectMovie]);


    // Get 25 random movies in a row from movies
    //
    //________________________________________________________________

    char* randomMovies[25][2];
    srand ( time(NULL) );
    int random = rand() % (count-1);
    //printf("\nRandom Number is: %i\n", random);

    for (int i = 0; i<25; i++) {

        if (random <= (count-1)) {
            randomMovies[i][0] = movieTitles[random];
            randomMovies[i][1] = movieValues[random];
            random++;
        } else {
            random = 0;
            randomMovies[i][0] = movieTitles[random];
            randomMovies[i][1] = movieValues[random];
            random++;
        }

        printf("\nRandommovie #%i\nKey: %s\nValue: %s\n\n",  (i+1), randomMovies[i][0], randomMovies[i][1]);
    }


	// set, get, delete and get movies


	setMovies( *randomMovies, argv[1], argv[2]);
	getMovies( *randomMovies, argv[1], argv[2]);
	deleteMovies( *randomMovies, argv[1], argv[2]);
	getMovies( *randomMovies, argv[1], argv[2]);

        return 0;
}
