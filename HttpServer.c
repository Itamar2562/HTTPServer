#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#include "CommsLayer/CommsUtils/commsUtils.h"
#include "CommsLayer/PfdsUtils/pfdUtils.h"

#include "ClientManagmentLayer/clientUtils.h"


#include "HttpLayer/HttpResponse/HttpResponse.h"
#include "HttpLayer/HttpHeader/HttpHeader.h"
#include "HttpLayer/HttpMethods/GET/HttpGet.h"


#include "ContentLayer/contentUtils.h"



int getHttpAction(char *headers,char *buffer, size_t maxLength)
{
  size_t length=strcspn(headers, " "); //get the length until the first space
  if (length>=maxLength)
  {
    fprintf(stderr, "Http action buffer overflow\n");
    return 0;
  }
  memcpy(buffer,headers, length);
  buffer[length]='\0';
  return 1;
}

char *buildCompleteResponse(httpResponse *r)
{
  char *responseHeaders=buildHTTPHeadersFromHeaderList(r->headersList);
  printf("response headers: %s\n",responseHeaders);
  size_t responseLength=r->headersList->total_byte_length + r->body_length;
  char *fullResponse =(char *)malloc(responseLength);

  if (responseHeaders==NULL || fullResponse ==NULL)
    return NULL;

  memcpy(fullResponse, responseHeaders, r->headersList->total_byte_length);
  memcpy(fullResponse +r->headersList->total_byte_length, r->body , r->body_length);

  free(responseHeaders);
  return fullResponse;
}

httpResponse* routeHttpRequest(char *headers)
{
  httpResponse *response = (httpResponse *) malloc( sizeof(httpResponse));
  if (response ==NULL)
    return NULL;

  if (initializeHttpResponse(response) ==0)
    return NULL;

  size_t maxLength=24;
  char action[maxLength];
  if (getHttpAction(headers, action, maxLength)==0 )
    return NULL;

  if (strcmp(action,"GET")==0)
  {
    if (GETResponse(response,headers) == 0)
       return NULL;
  }
  else;

  return response;

}

void SendHttpResponse(int clientFd, httpResponse *response)
{
  char *fullResponse=buildCompleteResponse(response);
  size_t length=response->body_length + response->headersList->total_byte_length;
  if (sendDataAll(clientFd, fullResponse, length) >0)
    //printf("\nsending to clientFd: %d msg\n%.*s\n",clientFd,(int)length,fullResponse); //temp, I dont really need to print what I send

  free(fullResponse);
  freeHttpResponse(response);
}

int searchForHttpHeadersChunkEnd(client *c , int *chunkEndIndex)
{
  int foundChunk=0;
   for (int i=0; i<c->chunkCurrLength ; i++)
    {
      if ((c->buffer)[i]=='\r'&& strncmp(&(c->buffer)[i],"\r\n\r\n",4)==0)
        {
          (*chunkEndIndex)=i+4;
          foundChunk=1;
          break;
        }
    }
    return foundChunk;
}


char* getHTTPChunk(int clientFd,  client *c , int *errorFlag , int *gotChunk)
{ 
    int status = recvChunk(clientFd , c->buffer, &c->chunkMaxLength, &c->chunkCurrLength);
    if (!status)  
    {
      (*errorFlag)=1;
      return NULL;
    }
    int chunkEndIndex=0;

    if (c->chunkCurrLength+1>=c->chunkMaxLength)
    {
      c->chunkMaxLength*=2;
      char *temp=realloc(c->buffer, c->chunkMaxLength +1);
      if (temp!=NULL)
      {
          (c->buffer) = temp;
      }
      else
      {
        (*errorFlag)=1;
        return NULL;
      }
    }

  (*gotChunk)= searchForHttpHeadersChunkEnd(c, &chunkEndIndex);
   
  char *header=NULL;
  
  if (*gotChunk){
    int rest=c->chunkCurrLength - chunkEndIndex;
    header=(char *)malloc(chunkEndIndex+1);
    if (header==NULL)
    {
        (*errorFlag)=1;
        return NULL;
    }
    strncpy(header, c->buffer, chunkEndIndex);
    header[chunkEndIndex]='\0';
    memcpy(c->buffer, c->buffer+chunkEndIndex, rest);
    (c->chunkCurrLength)=rest;
    c->buffer[rest]='\0';
  }
  return header;
}



void handleClientData(int listener, int *curr_count, struct pollfd *pfds,client *clients, int *index)
{
 
  int clientFd=pfds[*index].fd;

  int gotChunk=0;
  int errorFlag =0;
  char *requestHeaders=getHTTPChunk(clientFd, &clients[*index], &errorFlag, &gotChunk );
  if (errorFlag)
  {
      close(clientFd);
      delFromPfds(pfds, *index, *curr_count);
      delFromClients(clients,*index, *curr_count );
      (*index)--; //delete swaps the last with curr so we need to check again this pos
      (*curr_count)--;
      return;
  }
  if (gotChunk)
  {
    printf("pollserver: recv from fd %d: \n%s\n",clientFd,requestHeaders);
    
    httpResponse *response=routeHttpRequest(requestHeaders);
    if (response ==NULL)
      return;
    else
       SendHttpResponse(clientFd, response);

    free(requestHeaders);
  }
}

void ProccessConnections(int listener, int *curr_count, int *max_size,struct pollfd **pfds, client **clients){
  for (int i=0; i<*curr_count;i++)
  {

    if ((*pfds)[i].revents & (POLLIN | POLLHUP)) // we got new data (smg to read or hang up)
    {
      if ((*pfds)[i].fd==listener) //the listener has smg to read (a new conn)
        {
          int clientFd=handleNewConnection(listener, *curr_count, *max_size,pfds); 
          if (clientFd!=0) // no error in comms layer
          {
            int status = addToClients(clients,*curr_count, max_size);
            if (status) // no need to delete the last pfd on error as we dont increase count
              (*curr_count)++;
          }
        }

      else
        handleClientData(listener, curr_count, *pfds,*clients,&i);
    }

  }
}


int main(int argc, int **argv)
{
  int sockfd=GetListenerSocket();
  if (sockfd==-1)
  {
    perror("error getting a listener");
    exit(1);
  }

  int max_size=5;
  int curr_count=1;

  struct pollfd * pfds=(struct pollfd* )malloc(max_size* sizeof(struct pollfd));
  client *clients = (client *)malloc(max_size * sizeof(client));
  pfds[0].fd=sockfd;
  pfds[0].events=POLLIN;

  while (1)
  {
      int poll_count=poll(pfds, curr_count, -1);
      if (poll_count==-1){
        perror("poll error");
        exit(1);
      }

      ProccessConnections(sockfd, &curr_count,&max_size, &pfds, &clients);
  }
  close(sockfd);
  free(pfds);
  freeClients(clients, curr_count);

  return 0;
}
