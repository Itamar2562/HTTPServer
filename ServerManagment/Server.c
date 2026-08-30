#include "Server.h"
#include <stdlib.h>

void freeServer(server *s)
{
  if (s==NULL)
    return;

  freeClients(s->clients , s->curr_count);
  freeNetworkList(s->networkList, s->curr_count);
  int rc = closeDB(s->dbConn);
  checkSQLErrors(rc ,NULL );
  free(s);

}

int initializeServer(server *s)
{
  s->max_size=5;
  s->curr_count=1;
  s->dbConn = openDB();
  if (s->dbConn == NULL)
    return 0;
  s->networkList = (NetworkList *)malloc(sizeof(NetworkList));
  if (s->networkList ==NULL)
    return 0;

  if (initializeNetworkList(s->networkList, s->max_size) ==0)
    return 0;

  s->clients=(client *)malloc(s->max_size * sizeof(client));
  if (s->networkList->pfds== NULL || s->clients ==NULL)
  {
    freeServer(s);
    return 0;
  }
  return 1;
}
