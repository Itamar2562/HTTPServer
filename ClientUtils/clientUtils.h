#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include <poll.h>

typedef struct 
{
  struct pollfd pfd;

  char *buffer;
  int chunkMaxLength;
  int chunkCurrLength;

} client;


void delFromClients(client **clients, int i,int *clientAmount );
int addToClients(client **clients,int clientFd,int *clients_count,int *clients_size);

#endif

