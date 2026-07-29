#pragma once

#include "../CommsLayer/PfdsUtils/pfdUtils.h"
#include "../ClientManagmentLayer/clientUtils.h"

typedef struct 
{
  struct pollfd *pfds;
  client *clients;

  int max_size;
  int curr_count;
  
}users;

void freeUsers(users *u);
