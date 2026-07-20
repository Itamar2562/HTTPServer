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
  size_t i=0;
  size_t length=strcspn(headers, " "); //get the length until the second space
  if (length>=maxLength)
    return 0;
  memcpy(buffer,headers, length);
  buffer[length]='\0';
  return 1;
}

char *buildCompleteResponse(httpResponse *r)
{
  char *responseHeaders=buildHTTPHeaders(r->headersList);
  size_t responseLength=r->headersList->total_byte_length + r->body_length;
  char *fullResponse =(char *)malloc(responseLength + 1 );

  strcpy(fullResponse, responseHeaders);
  strcat(fullResponse, r->body);
  return fullResponse;
}

httpResponse* routeHttpRequest(char *headers)
{
  size_t maxLength=24;
  char action[maxLength];
  if (getHttpAction(headers, action, maxLength)==0 )
    return NULL;
  if (action==NULL)
    return NULL;
  httpResponse *response = (httpResponse *) malloc( sizeof(httpResponse));
  initializeHttpResponse(response);

  if (strcmp(action,"GET")==0)
  {
    int status = GETResponse(response,headers);
    if (!status)  
      return NULL;
  }
  else;

  return response;

}

void SendHttpResponse(int clientFd, httpResponse *response)
{
  char *fullResponse=buildCompleteResponse(response);
  size_t length=strlen(fullResponse);
  sendDataAll(clientFd, fullResponse, &length);
  printf("\nsending to clientFd: %d msg\n %s\n",clientFd,fullResponse);

  free(fullResponse);
  freeHttpResponse(response);
}




char *getChunk(int clientFd, char **buffer,int *maxLength , int *currLength)
{
  int gotChunk=0;
  int end=0;
  char *header=NULL;
  while(!gotChunk)
  { 
    int remainingSpace=*maxLength-*currLength;
    int nbytes=recv(clientFd, (*buffer)+ (*currLength),remainingSpace-1,0);
    (*currLength) +=nbytes;
    (*buffer)[*currLength]='\0';
    if (nbytes<=0) // got a problem
    {
      if (nbytes==0) // connection closed
      {
        printf("pollserver: socket %d hung up on us :(\n",clientFd);
      }
      else
          perror("recv");
      break;
    }

    header=getHTTPChunk(buffer,maxLength, currLength);
    if (header!=NULL)
      gotChunk=1;
  }
  return header;
}

void handleClientData(int listener, int *curr_count, struct pollfd *pfds,client *clients, int *index)
{
 
  int clientFd=pfds[*index].fd;

  char *requestHeaders=getChunk(clientFd, &clients[*index].buffer, &clients[*index].chunkMaxLength, &clients[*index].chunkCurrLength);
  if (requestHeaders==NULL)
  {
      close(clientFd);
      delFromPfds(pfds, *index, *curr_count);
      delFromClients(clients,*index, *curr_count );
      (*index)--; //delete swaps the last with curr so we need to check again this pos
      (*curr_count)--;
  }
  
  else
  {
    printf("pollserver: recv from fd %d: \n%s\n",clientFd,requestHeaders);
    
    httpResponse *response=routeHttpRequest(requestHeaders);
    if (response ==NULL)
      return;
    else
       SendHttpResponse(clientFd, response);
  }
    free(requestHeaders);
}

void ProccessConnections(int listener, int *curr_count, int *max_size,struct pollfd **pfds, client **clients){
  for (int i=0; i<*curr_count;i++)
  {

    if ((*pfds)[i].revents & (POLLIN | POLLHUP)) // we got new data (smg to read or hang up)
    {
      if ((*pfds)[i].fd==listener) //the listener has smg to read (a new conn)
        {
          int clientFd=handleNewConnection(listener, *curr_count,max_size,pfds); 
          if (clientFd!=-1) // check if accept worked
          {
            addToClients(clients, clientFd, *curr_count, max_size);
            initializeClient(*clients, *curr_count);
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
      int poll_count=poll(pfds, max_size, -1);
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
