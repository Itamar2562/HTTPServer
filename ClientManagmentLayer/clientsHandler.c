#include "clientUtils.h"
#include <stdlib.h>

#define MAX_BUFFER_CHUNK_SIZE 256


void delFromClients(client *clients, int i,int client_count )
{
  free(clients[i].buffer);
  clients[i]=clients[(client_count-1)];
}

int addToClients(client **clients,int clientFd,int clients_count,int *clients_size)
{
    if (clients_count>= *clients_size)
  {
    (*clients_size)*=2;
    *clients=realloc(*clients,sizeof(**clients) * (*clients_size) );
  }

  (*clients)[clients_count].chunkMaxLength=MAX_BUFFER_CHUNK_SIZE; 
  (*clients)[clients_count].buffer =(char *)malloc(MAX_BUFFER_CHUNK_SIZE); 
}

void initializeClient(client * clients, int index)
{
  clients[index].buffer=(char *)malloc(MAX_BUFFER_CHUNK_SIZE);
  clients[index].chunkCurrLength=0;
  clients[index].chunkMaxLength=MAX_BUFFER_CHUNK_SIZE;
}

void freeClients(client *clients,int clients_count)
{
  for (int i=0; i<clients_count; i++)
  {
    free(clients[i].buffer);
  }
  free(clients);

}