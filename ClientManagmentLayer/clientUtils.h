#pragma once
#include <stddef.h>
#include "../HttpLayer/HttpRequest/HttpRequest.h"

#define STARTING_BUFFER_SIZE 256
#define MAX_BUFFER_SIZE 16384

typedef enum {
    READING_HEADERS,
    READING_BODY
} RequestState;


typedef struct 
{
  char *buffer;
  size_t chunkMaxLength; 
  size_t chunkCurrLength;

  RequestState state;

  HttpRequest *request;

  size_t contentLength;

} client;



void delFromClients(client *clients, int i,int client_count );
int addToClients(client **clients,int clients_count,int *clients_size);
void freeClients(client *clients,int clients_count);



