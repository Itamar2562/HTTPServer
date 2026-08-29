#pragma once

#include "../CommsLayer/PfdsUtils/pfdUtils.h"
#include "../ClientManagmentLayer/clientUtils.h"
#include "../CommsLayer/CommsUtils/commsUtils.h"

typedef struct 
{
  pfdsWrapper *pw;
  client *clients;

  int max_size;
  int curr_count;
  
}users;

void freeUsers(users *u);
int initializeUsers(users *u);
