#pragma once

#include "../ClientManagmentLayer/clientUtils.h"

#include "../CommsLayer/CommsUtils/commsUtils.h"

typedef struct 
{
  NetworkList *networkList;
  client *clients;

  int max_size;
  int curr_count;
  
}users;

void freeUsers(users *u);
int initializeUsers(users *u);
