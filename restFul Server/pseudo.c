//################################ DELETE
} else if (flagextraction == OR_DELETE) {
        onion_response_printf(res, "Received delete.\n");

        // prüfe, ob in der Anfrage ein zu löschendes Element enthalten ist

        if (requestBody == leer){
            delete_all(hashtable);  // lösche alle Elemente
        }
        else{
            delete_entry(hashtable, key);   // lösche das angefragte Element
        }

        //DELETE:
        //curl -v -X DELETE http://localhost:4711/filme

//################################ GET
} else if (flagextraction == OR_GET) {
    onion_response_printf(res, "Received get.\n");


    // prüfe, ob in der Anfrage ein zu suchendes Element enthalten ist

    if (requestBody == leer){

        char** liste = get_all(hashtable);  // speichere alle Elemente in der Liste

        // Konkatenation von key und value für das JSON-Format
        // Ziel: {"key":"Movie Title","value":"Regisseur: Mister X, Year: 2017"}

        for( int i = 0; i < hashtable->size; i++ ){

                    if( strcmp(liste[i]->key, "") != 0 ){

                        char* fullString = malloc(10000);
                        strcpy(fullString, "{\"key\":\"");
                        strcat(fullString, liste[i]->key);
                        strcat(fullString, "\",\"value\":\"");
                        strcat(fullString, liste[i]->value);
                        strcat(fullString, "\"}");

                        //      fullString als JSON parsen
                }
        }
    }
    else{

        value = get_entry(hashtable, key);   // speichere value des angefragten Elements
        char* fullString = malloc(10000);
        strcpy(fullString, "{\"key\":\"");
        strcat(fullString, liste[i]->key);
        strcat(fullString, "\",\"value\":\"");
        strcat(fullString, liste[i]->value);
        strcat(fullString, "\"}");

        // fullString als JSON parsen
    }

    // GET:
    //curl -v http://localhost:4711/

}