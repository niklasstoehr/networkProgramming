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

hashtable_Element *hashtable;

onion_connection_status data_handler(void *_, onion_request * req, onion_response * res);
void onexit(int _);
