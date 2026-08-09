#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#include "CommsLayer/CommsUtils/commsUtils.h"
#include "CommsLayer/PfdsUtils/pfdUtils.h"

#include "ClientManagmentLayer/clientUtils.h"

#include "UsersManagment/Users.h"

#include "HttpLayer/HttpResponse/HttpResponse.h"
#include "HttpLayer/HttpHeader/HttpHeader/HttpHeader.h"
#include "HttpLayer/HttpMethods/GET/HttpGet.h"
#include "HttpLayer/HttpRequest/HttpRequest.h"


#include "ContentLayer/contentUtils.h"


int validVersion(HttpRequest *request)
{
  if (strcmp(request->version,"HTTP/1.1")==0)
    return 1;
  return 0;
}

httpResponse* routeHttpRequest(HttpRequest *request)
{
  httpResponse *response = (httpResponse *) malloc( sizeof(httpResponse));
  if (response ==NULL)
    return NULL;

  if (initializeHttpResponse(response) ==0)
    return NULL;

  if (!validVersion(request))
  {
    if (getNotValidResponse(response, request) ==0)
      return NULL;
  }

  else if (strcmp(request->method,"GET")==0)
  {
    if (GETResponse(response,request) == 0)
       return NULL;
  }
  else;

  return response;

}

void SendHttpResponse(int clientFd, httpResponse *response)
{
  size_t fullResponseLength=0;
  char *fullResponse=buildCompleteResponse(response , &fullResponseLength);
  if (fullResponse !=NULL)
      sendDataAll(clientFd, fullResponse, fullResponseLength);

  free(fullResponse);
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
      if (c->chunkMaxLength > MAX_BUFFER_SIZE)
      {
        (*errorFlag)=1;
        return NULL;
      }
      char *temp=realloc(c->buffer, c->chunkMaxLength +1);
      if (temp!=NULL)
          (c->buffer) = temp;
      else
      {
        (*errorFlag)=1;
        return NULL;
      }
    }

  (*gotChunk)= searchForHttpHeadersChunkEnd(c, &chunkEndIndex);
   
  char *header=NULL;
  
  if (*gotChunk)
  {
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



void handleClientData(int listener, users *users, int *index)
{
 
  int clientFd=users->pfds[*index].fd;

  int gotChunk=0;
  int errorFlag =0;
  char *RawRequest=getHTTPChunk(clientFd, &users->clients[*index], &errorFlag, &gotChunk );
  if (errorFlag)
  {
      printf("removed socket %d\n",clientFd);
      close(clientFd);
      delFromPfds(users->pfds, *index, users->curr_count);
      delFromClients(users->clients,*index, users->curr_count );
      (*index)--; //delete swaps the last with curr so we need to check again this pos
      users->curr_count--;
      return;
  }
  if (gotChunk)
  {
    printf("pollserver: recv from fd %d: \n%s\n",clientFd,RawRequest);
    
    HttpRequest  *parsedRequest= buildHttpRequest(RawRequest);
    if (parsedRequest== NULL)
      return;
    httpResponse *response=routeHttpRequest(parsedRequest);
    if (response ==NULL)
    {
      freeRequest(parsedRequest);
      free(RawRequest);
      return;
    }
    else
       SendHttpResponse(clientFd, response);

    freeHttpResponse(response);
    freeRequest(parsedRequest);
    free(RawRequest);
  }
}

void ProccessConnections(int listener, users *users, int poll_count){
  for (int i=0; i<users->curr_count && poll_count >0;i++)
  {

    if (users->pfds[i].revents & (POLLIN | POLLHUP)) // we got new data (smg to read or hang up)
    {
      if (users->pfds[i].fd==listener) //the listener has smg to read (a new conn)
        {
          int clientFd=handleNewConnection(listener, users->curr_count, users->max_size,&users->pfds); 
          if (clientFd!=0) // no error in comms layer
          {
            int status = addToClients(&users->clients,users->curr_count,  &users->max_size);
            if (status) // no need to delete the last pfd on error as we dont increase count
              users->curr_count++;
          }
        }

      else
        handleClientData(listener, users,&i);
      poll_count--;
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

  users *u=(users *)malloc(sizeof(users));
  if (u==NULL)
  {
    close(sockfd);
    return 1;
  }
  u->max_size=5;
  u->curr_count=1;
  u->pfds=(struct pollfd* )malloc(u->max_size* sizeof(struct pollfd));
  u->clients=(client *)malloc(u->max_size * sizeof(client));
  if (u->pfds== NULL || u->clients ==NULL)
  {
    free(u->pfds);
    free(u->clients);
    free(u);
    close (sockfd);
    return 1;
  }
  u->pfds[0].fd=sockfd;
  u->pfds[0].events=POLLIN;

  while (1)
  {
      int poll_count=poll(u->pfds, u->curr_count, -1);
      if (poll_count==-1){
        perror("poll error");
        exit(1);
      }

      ProccessConnections(sockfd, u, poll_count);
  }
  close(sockfd);
  freeUsers(u);

  return 0;
}
