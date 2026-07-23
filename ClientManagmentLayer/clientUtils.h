#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#define STARTING_BUFFER_SIZE 256
#define MAX_BUFFER_SIZE 16384

typedef struct 
{
  char *buffer;
  int chunkMaxLength;
  int chunkCurrLength;

} client;

void delFromClients(client *clients, int i,int client_count );
int addToClients(client **clients,int clients_count,int *clients_size);
void freeClients(client *clients,int clients_count);

#endif

