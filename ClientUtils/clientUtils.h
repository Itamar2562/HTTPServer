#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H


typedef struct 
{
  char *buffer;
  int chunkMaxLength;
  int chunkCurrLength;

} client;

void delFromClients(client *clients, int i,int client_count );
int addToClients(client **clients,int clientFd,int clients_count,int *clients_size);
void initializeClient(client * clients, int index);
void freeClients(client *clients,int clients_count);

#endif

