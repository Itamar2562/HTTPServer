#include "clientUtils.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define MAX_BUFFER_CHUNK_SIZE 256

int initializeClient(client * clients, int index)
{
  clients[index].buffer=(char *)malloc(MAX_BUFFER_CHUNK_SIZE);
  if (clients[index].buffer ==NULL)
    return 0;
  clients[index].chunkCurrLength=0;
  clients[index].chunkMaxLength=MAX_BUFFER_CHUNK_SIZE;
  return 1;
}


void delFromClients(client *clients, int i,int client_count )
{
  free(clients[i].buffer);
  clients[i]=clients[(client_count-1)];
}


int addToClients(client **clients,int clients_count,int *clients_size)
{
  if (clients_count>= *clients_size)
  {
    (*clients_size)*=2;
    client *temp= realloc(*clients,sizeof(client) * (*clients_size));
    if (temp==NULL)
    {
      perror("memory error");
      return 0;
    }
    (*clients)= temp;
  }
  int status=initializeClient(*clients, clients_count);
  return status;
}


void freeClients(client *clients,int clients_count)
{
  for (int i=0; i<clients_count; i++)
  {
    free(clients[i].buffer);
  }
  free(clients);

}