#include "Users.h"
#include <stdlib.h>

void freeUsers(users *u)
{
  if (u==NULL)
    return;

  freeClients(u->clients , u->curr_count);
  freePfdWrapper(u->pw, u->curr_count);

  free(u);

}

int initializeUsers(users *u)
{
  u->max_size=5;
  u->curr_count=1;
 
  u->pw = (pfdsWrapper *)malloc(sizeof(pfdsWrapper));
  if (u->pw ==NULL)
    return 0;

  if (initializePfdsWrapper(u->pw, u->max_size) ==0)
    return 0;

  u->clients=(client *)malloc(u->max_size * sizeof(client));
  if (u->pw->pfds== NULL || u->clients ==NULL)
  {
    freeUsers(u);
    return 0;
  }
  return 1;
}
