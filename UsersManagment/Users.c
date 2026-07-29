#include "Users.h"
#include <stdlib.h>

void freeUsers(users *u)
{
  freeClients(u->clients, u->curr_count);
  free(u->pfds);
}