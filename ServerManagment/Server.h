#pragma once

#include "../ClientManagmentLayer/clientUtils.h"

#include "../CommsLayer/CommsUtils/commsUtils.h"
#include "../ContentLayer/DatabaseHandler/DatabaseHandler.h"

typedef struct 
{
  NetworkList *networkList;
  client *clients;

  int max_size;
  int curr_count;

  sqlite3 *dbConn;

}server;

void freeServer(server *u);
int initializeServer(server *u);
