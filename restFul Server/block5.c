#include <signal.h>
#include <onion/log.h>
#include <onion/onion.h>
#include <onion/block.h>
#include <onion/shortcuts.h>
#include "cJSON.h"
#include <stdio.h>
#include "string.h"
#include <hashtable.h>
#include <cJSON.h>
#include <block5.h>
#include <stdlib.h>
 
onion *o = NULL;
 
 
onion_connection_status data_handler(void *_, onion_request * req,
                     onion_response * res)
{
    int flags = onion_request_get_flags(req);
    int flagextraction = flags & 7;
 
 
    const onion_block *dreq = onion_request_get_data(req);
 
    //################################ PUT
    if (flagextraction == OR_PUT) {
        onion_response_printf(res, "put succesfull\n\n");
 
        // PUT:
        //curl -v -H "Content-Type: application/json" -X PUT -d '{"key":"Movie Title","value":"Regisseur: Mister X, Year: 2017"}' http://localhost:4711/filme
 
        if (dreq) {
             
            /* libonion stores curl body on tmpfile */
            char buffer[512];
            FILE *tmpfile;
            size_t nread;
            char* s = malloc(500);
 
 
            // ################ start Niklas #########################
 
            /* Curl Message Options:
 
            only key and value attribute
            curl -v -H "Content-Type: application/json" -X PUT -d '{"key":"Movie Title","value":"Regisseur: Mister X, Year: 2017"}' http://localhost:4711/filme
 
            with detailed value information
            curl -v -H "Content-Type: application/json" -X PUT -d '{"key":"Movie Title","value":{"regisseur":"Mister X","year":"2017"}}' http://localhost:4711/filme
             
            */
 
            // curl body encoded in tmpfile
            printf("server received put\n\n");
 
            /* Rewind file pointer */
            tmpfile = fopen(onion_block_data(dreq), "r");
            rewind(tmpfile);
 
            int character = 0;
            int count = 0;
            char body [1000];
 
            while(!feof(tmpfile)){
               character = fgetc(tmpfile);
 
               body[count] = (char) character;
                
               count++;
 
               // print every character
               //printf("%c", character);
            }
 
            // delete question mark
            body[count-1] = '\0';
  
            // Output body
            int x;
            for (x = 0; x < count; x++) {
            printf("%c", body[x]);
            }
 
            printf("\n\n");
 
            // Handle JSON Body
            //convert char array to char pointer
            char *bodyJson = malloc(count * sizeof(char));
            bodyJson = &body[0];
  
            //printf("Json elements:\n");
 
                // convert the array pointer to json root pointer
                cJSON *root = cJSON_Parse(bodyJson);
                 
                // get key and value
                char *key = cJSON_GetObjectItem(root, "key")->valuestring;
                char *value = cJSON_GetObjectItem(root, "value")->valuestring;
                printf("key: %s\nvalue: %s\n\n", key, value);  
 
                set_hashtable(hashtable, key, value);
 
                /*
                // retrieve more detailed value information
                cJSON *values = cJSON_GetObjectItem(root, "value");
 
                    printf("value attributes:\n");
                    // Get the JSON element and then get the values as before
                    cJSON *movie_values = cJSON_GetArrayItem(values, 0);
                    char *regisseur = cJSON_GetObjectItem(movie_values, "regisseur")->valuestring;
                    int year = cJSON_GetObjectItem(movie_values, "year")->valueint;
                    printf("%s, %d \n", regisseur, year);
                */
 
                char* pullAnswer = malloc(1000);
                strcpy(pullAnswer, "\n");
                strcpy(pullAnswer, "{\"key\":\"");
                strcat(pullAnswer, key);
                strcat(pullAnswer, "\",\"value\":\"");
                strcat(pullAnswer, value);
                strcat(pullAnswer, "\",");
                strcat(pullAnswer, "\"href\":\"filme/");
                strcat(pullAnswer, key);
                strcat(pullAnswer, "\"}\n");
                onion_response_printf(res,"%s",pullAnswer);
                onion_response_printf(res, "\n");
 
 
            cJSON_Delete(root);
 
        // #################### end Niklas ############################
 
 
            if (tmpfile) {
                while ((nread = fread(buffer, 1, 511, tmpfile)) > 0) {
                    buffer[nread] = '\0';
                    onion_response_printf(res, "%s",
                                  buffer);
                    //int len = strlen(s);
                    //s[len] = buffer;
                    //s[len+1] = '\0';
 
                }
                fclose(tmpfile);
            }
            onion_response_printf(res, "\n");
 
        }
 
        return OCS_PROCESSED;
 
         
    //################################ POST
    } else if (flagextraction == OR_POST) {
         
        // POST:
        //curl -v -H "Content-Type: application/json" -X POST -d '{"key":"Movie Title","value":"Regisseur: Mister X, Year: 2017"}' http://localhost:4711/filme
 
        onion_response_printf(res, "post succesfull\n\n");


        if (dreq) {
             
        	const char* dataRead = onion_block_data( onion_request_get_data(req) );

            // curl body encoded in tmpfile
            printf("server received post\n\n");
 
  
            //printf("Json elements:\n");
 
            // convert the array pointer to json root pointer
            cJSON *root = cJSON_Parse(dataRead);
             
            // get key and value
            char *key = cJSON_GetObjectItem(root, "key")->valuestring;
            char *value = cJSON_GetObjectItem(root, "value")->valuestring;
            printf("key: %s\nvalue: %s\n\n", key, value);  

            set_hashtable(hashtable, key, value);

            /*
            // retrieve more detailed value information
            cJSON *values = cJSON_GetObjectItem(root, "value");

                printf("value attributes:\n");
                // Get the JSON element and then get the values as before
                cJSON *movie_values = cJSON_GetArrayItem(values, 0);
                char *regisseur = cJSON_GetObjectItem(movie_values, "regisseur")->valuestring;
                int year = cJSON_GetObjectItem(movie_values, "year")->valueint;
                printf("%s, %d \n", regisseur, year);
            */

            char* pullAnswer = malloc(1000);
            strcpy(pullAnswer, "\n");
            strcpy(pullAnswer, "{\"key\":\"");
            strcat(pullAnswer, key);
            strcat(pullAnswer, "\",\"value\":\"");
            strcat(pullAnswer, value);
            strcat(pullAnswer, "\",");
            strcat(pullAnswer, "\"href\":\"filme/");
            strcat(pullAnswer, key);
            strcat(pullAnswer, "\"}\n");
            onion_response_printf(res,"%s",pullAnswer);
            onion_response_printf(res, "\n");
 
 
            cJSON_Delete(root);
 
            onion_response_printf(res, "\n");
 
        }
 
        return OCS_PROCESSED;
 
 
 
    //################################ DELETE
    } else if (flagextraction == OR_DELETE) {
        onion_response_printf(res, "Received delete.\n");
 
        // prüfe, ob in der Anfrage ein zu löschendes Element enthalten ist
 
        const char* getPath = onion_request_get_fullpath(req);
        printf("%s\n", getPath); 
 
        char* path = malloc(400);
        strcpy(path, getPath);
         
        const char s[2] = "/";
        char *key;
        
        key = strtok(path, s);
        key = strtok(NULL, s);
        printf("key: %s\n", key);
 
        if (key == NULL){
            delete_all(hashtable);  // lösche alle Elemente
        }
        else{
            delete_entry(hashtable, key);   // lösche das angefragte Element
        }
 
 
        //DELETE: curl -v -X DELETE http://localhost:4711/filme
         
 
 
    //################################ GET
    } else if (flagextraction == OR_GET) {
            onion_response_printf(res, "Received get.\n");
 
 
            printf("GET:\n");
 
            // GET:
            //curl -v http://localhost:4711/
 
            // prüfe, ob in der Anfrage ein zu suchendes Element enthalten ist
             
            // get path of get
            const char* getPath = onion_request_get_fullpath(req);
            printf("%s\n", getPath);
 
            char* path = malloc(400);
            strcpy(path, getPath);
             
            const char s[2] = "/";
            char *key;
            
            key = strtok(path, s);
            key = strtok(NULL, s);
            printf("key: %s\n", key);
 
            
 
 
            if (key == NULL){
                 
              char** liste = get_all(hashtable);  // speichere alle Elemente in der Liste
            // Konkatenation von key und value für das JSON-Format
            // Ziel: {"key":"Movie Title","value":"Regisseur: Mister X, Year: 2017"}
              onion_response_printf(res, "\nGET:\n");
 
            for( int i = 0; i < 2*hashtable->size; i++ ){
 
                if( strcmp(liste[i], "") != 0 ){
 
                    char* fullString = malloc(10000);
                    strcpy(fullString, "{\"key\":\"");
                    strcat(fullString, liste[i]);
                    i++;
                    strcat(fullString, "\",\"value\":\"");
                    strcat(fullString, liste[i]);
                    strcat(fullString, "\",");
                    strcat(fullString, "\"href\":\"filme/");
                    i--;
                    strcat(fullString, liste[i]);
                    i++;
                    strcat(fullString, "\"""}\n");


 
                    printf("\n%s\n", fullString);
 
                    // fullString als JSON parsen
                    // convert the array pointer to json root pointer
                    cJSON *rootGet = cJSON_Parse(fullString);
                    free(fullString);
 
                    // get key and value
                    char *keyGet = cJSON_GetObjectItem(rootGet, "key")->valuestring;
                    char *valueGet = cJSON_GetObjectItem(rootGet, "value")->valuestring;
                    printf("key: %s\nvalue: %s\n", keyGet, valueGet); 
 
                    char* returnGet = malloc(10000);
                    strcat(returnGet, "\nKey is: "); 
                    strcat(returnGet, keyGet); 
                    strcat(returnGet, "\nValue is: "); 
                    strcat(returnGet, valueGet); 
 
                    // respone to client
                    onion_response_printf(res, "%s\n", returnGet);
                    onion_response_printf(res, "\n");
                }
            }
 
            onion_response_printf(res, "\n\n");
             
            } else{
 
                char* value = malloc(5000);
                value = get_entry(hashtable, key);   // speichere value des angefragten Elements
                char* fullString = malloc(10000);
 
                if(value != NULL){
                    strcpy(fullString, "\n");
                    strcpy(fullString, "{\"key\":\"");
                    strcat(fullString, key);
                    strcat(fullString, "\",\"value\":\"");
                    strcat(fullString, value);
                    strcat(fullString, "\",");
                    strcat(fullString, "\"href\":\"filme/");
                    strcat(fullString, key);
                    strcat(fullString, "\"""}\n");
                }
                else{
                    strcpy(fullString, "Movie is not in database");
                }
 
    //          // fullString als JSON parsen
 
                printf("\n%s\n", fullString);
 
                onion_response_printf(res, "%s", fullString);
                onion_response_printf(res, "\n");
             }
         
     
 
 
 
    } else {
        onion_response_printf(res, "Method not supported!\n\n");
 
        return OCS_PROCESSED;
    }
 
 
    if (dreq) {
        onion_response_printf(res, "Request Body \n\n");
         
 
        //onion_block_data(dreq));
    }
 
    return OCS_PROCESSED;
}
 
 
 
 
void onexit(int _)
{
    ONION_INFO("Exit");
    if (o) {
        onion_listen_stop(o);
    }
}
 
int main(int argc, char **argv)
{
    o = onion_new(O_ONE_LOOP);
    onion_set_port(o, "4711");
    printf("\n\n######### Restful Server running on port 4711 #########\n\n");
 
    onion_url *urls = onion_root_url(o);
 
    //onion_url_add_static(urls, "", "Hello World!\n", HTTP_OK);
 
    hashtable = ht_create(65536);
 
    // Filmdatenbank
    onion_url_add(urls, "", data_handler);
    onion_url_add(urls, "^(.*)$", data_handler);
 
    signal(SIGTERM, onexit);
    signal(SIGINT, onexit);
    onion_listen(o);
 
    onion_free(o);
    return 0;
}