#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "CommsLayer/CommsUtils/commsUtils.h"
#include "CommsLayer/PfdsUtils/pfdUtils.h"

#include "ClientManagmentLayer/clientUtils.h"

#include "ServerManagment/Server.h"

#include "HttpLayer/HttpResponse/HttpResponse.h"
#include "HttpLayer/HttpHeader/HttpHeader/HttpHeader.h"
#include "HttpLayer/HttpMethods/GET/HttpGet.h"
#include "HttpLayer/HttpMethods/POST/HttpPOST.h"
#include "HttpLayer/HttpRequest/HttpRequest.h"
#include "HttpLayer/HttpMethods/HEAD/HttpHead.h"


#include "ContentLayer/ContentLoader/ContentLoader.h"


int validVersion(HttpRequest *request)
{
  if (strcmp(request->version,DEFAULT_HTTP_VERSION)==0)
    return 1;
  return 0;
}

int switchToReadingBodyState(client *client, HttpRequest *request)
{
  char *str_value = getHeaderValue(request->headerList, "Content-Length");
  if (str_value ==NULL)
    return 0;
  long int contentLength = strtol(str_value, NULL ,10);
  client->contentLength = contentLength;
  client->state = READING_BODY;
  free(str_value);
  return 1;
}


httpResponse* routeHttpRequest(client *client , int *errorFlag)
{
  HttpRequest *request = client->request;
  httpResponse *response = (httpResponse *) malloc( sizeof(httpResponse));
  if (response ==NULL)
    return NULL;

  if (initializeHttpResponse(response) ==0)
    return NULL;

  if (!validVersion(request))
  {
    if (getNotValidResponse(response, request) ==0)
      return NULL;
    return response;
  }

  if (strcmp(request->method,"GET")==0)
  {
    if (GETResponse(response,request))
       return response;
  }
  else if (strcmp(request->method,"POST")==0)
  {
      freeHttpResponse(response);
      switchToReadingBodyState(client, request);
      return NULL;
  }
  else if (strcmp(request->method,"HEAD")==0)
  {
    if (HEADResponse(response , request))
      return response;
  }
  else
  {
    response = getBadRequestResponse();
    (*errorFlag) = 1;
    return response;
  }
  return NULL;
}

httpResponse *routeHttpRequestWithBody(server *s, int index, const char *body)
{
  client *c=&s->clients[index];
  httpResponse *response = (httpResponse *) malloc( sizeof(httpResponse));
  if (response ==NULL)
    return NULL;
  if (initializeHttpResponse(response) ==0)
    return NULL;

  if (strcmp(c->request->method, "POST")==0)
  {
    if (POSTResponse(response, c->request, body ,s->dbConn))
        return response;
  }
  return NULL;
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

int extractByLength(client *c, int *chunkEndIndex)
{
  if (c->chunkCurrLength >= c->contentLength)
  {
    (*chunkEndIndex) = c->contentLength;
    c->contentLength=0;
    return 1;
  }
  return 0;
}

char *getChunkFromBuffer(client *c, int chunkEndIndex)
{
    int rest=c->chunkCurrLength - chunkEndIndex;
    char *chunk=(char *)malloc(chunkEndIndex+1);
    if (chunk==NULL)
      return NULL;
    strncpy(chunk, c->buffer, chunkEndIndex);
    chunk[chunkEndIndex]='\0';
    memcpy(c->buffer, c->buffer+chunkEndIndex, rest);
    (c->chunkCurrLength)=rest;
    c->buffer[rest]='\0';
    return chunk;
}

char *findChunkInBuffer(client *c , int *gotChunk)
{
    int chunkEndIndex= 0;
    if (c->state==READING_HEADERS)
      (*gotChunk)= searchForHttpHeadersChunkEnd(c, &chunkEndIndex);
    else  
      (*gotChunk) = extractByLength(c, &chunkEndIndex);
     
    if (*gotChunk)
    {
    char *chunk = getChunkFromBuffer(c, chunkEndIndex);
    return chunk;
    }
    return NULL;
}

char* getHTTPChunk(int clientFd,  client *c , int *errorFlag , int *gotChunk)
{ 
     int status = recvChunk(clientFd , c->buffer, &c->chunkMaxLength, &c->chunkCurrLength);
    if (!status)  
    {
      (*errorFlag)=1;
      return NULL;
    }    
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
    return findChunkInBuffer(c,gotChunk);

}

void disconnectUser(int clientFd,server *s, int *index)
{
  printf("removed socket %d\n",clientFd);
    close(clientFd);
    delFromNetworkList(s->networkList, *index, s->curr_count);
    delFromClients(s->clients,*index, s->curr_count );
    (*index)--; //delete swaps the last with curr so we need to check again this pos
    s->curr_count--;
}

char *getHttpChunkWrapper(int clientFd,server *s ,int *index, int *gotChunk)
{

  int errorFlag =0;
  char *RawChunk=getHTTPChunk(clientFd, &s->clients[*index], &errorFlag, gotChunk );
  if (errorFlag)
  {
    disconnectUser(clientFd, s, index);
    return NULL;

  }
  return RawChunk;
}

httpResponse *processBufferBodyData(server *s, int index , int *gotChunk , int *errorFlag)
{
  client *client= &s->clients[index];
  (*gotChunk ) = 0;
    if (client->contentLength <=0) //there is no body
      {
          freeRequest(client->request);
          client->state=READING_HEADERS;
          (*errorFlag)=0;
          return NULL;
      }

    //the body is fully in buffer already
    else if (client->chunkCurrLength>0 && client->chunkCurrLength>=client->contentLength)
    {
      char *RawChunk = findChunkInBuffer(client,gotChunk);  
      if (gotChunk)
      {
        httpResponse *response=routeHttpRequestWithBody(s,index,RawChunk);
        client->state=READING_HEADERS;
        free(RawChunk);
        if (response ==NULL)
        {
          response = getBadRequestResponse();
          (*errorFlag) = 1;
        }
        return response;
      }
    }
    return NULL;
}

void handleClientData(int listener, server *s, int *index)
{
 
  int clientFd=s->networkList->pfds[*index].fd;

  client *client =&s->clients[*index];
  int gotChunk =0;
  char *RawChunk = getHttpChunkWrapper(clientFd,s,index , &gotChunk);
  while (gotChunk) //go through the buffer (the loop doesn't do anymore recv it searches for chunks in the buffer)
  {
    if (RawChunk ==NULL)
      RawChunk = findChunkInBuffer(client , &gotChunk);

    if (!gotChunk || RawChunk == NULL)
    break;

    printf("pollserver: recv from fd %d: \n%s\n",clientFd,RawChunk);
    
    int errorFlag=0;
    httpResponse* response = NULL;
    if (client->state== READING_HEADERS)
    {
      client->request= buildHttpRequest(RawChunk);
      free(RawChunk);
      RawChunk=NULL;
      if (client->request== NULL)
      {
        response = getBadRequestResponse();
        errorFlag =1;
      }
      else
        response=routeHttpRequest(client , &errorFlag);
    }
    else //got full body chunk
    {
      response=routeHttpRequestWithBody(s,*index,RawChunk);
      if (response ==NULL)
      {
        response = getBadRequestResponse();
        errorFlag = 1;
      } 
      client->state=READING_HEADERS;
      free(RawChunk);
      RawChunk = NULL;
    }
    //there is a body to the request. if it got here it means its still in buffer
    if (client->state == READING_BODY)
    {
      free(RawChunk);
      RawChunk = NULL;
      response = processBufferBodyData(s,*index, &gotChunk , &errorFlag);
      if (response ==NULL && !errorFlag) //there is no response but a body exists
        continue;
    }
    if (response !=NULL)
    {
        SendHttpResponse(clientFd, response);
        freeHttpResponse(response);
    }
    freeRequest(client->request);
    client->request=NULL;
    if (errorFlag)
    {
      disconnectUser(clientFd, s, index);
      break;
    }

  }

}

void ProccessConnections(int listener, server *s, int poll_count){
  for (int i=0; i<s->curr_count && poll_count >0;i++)
  {

    if (s->networkList->pfds[i].revents & (POLLIN | POLLHUP)) // we got new data (smg to read or hang up)
    {
      if (s->networkList->pfds[i].fd==listener) //the listener has smg to read (a new conn)
        {
          int clientFd=handleNewConnection(listener, s->curr_count, s->max_size,s->networkList); 
          if (clientFd!=0) // no error in comms layer
          {
            int status = addToClients(&s->clients,s->curr_count,  &s->max_size);
            if (status) // no need to delete the last pfd on error as we dont increase count
              s->curr_count++;
          }
        }

      else
        handleClientData(listener, s,&i);
      poll_count--;
    }

  }
}


int main(int argc, char **argv)
{
  int sockfd=GetListenerSocket();
  if (sockfd==-1)
  {
    perror("error getting a listener");
    exit(1);
  }

  server *s=(server *)malloc(sizeof(server));
  if (s==NULL)
  {
    close(sockfd);
    return 1;
  }

  if (initializeServer(s) == 0)
  {
    close(sockfd);
    return 1;
  }

  s->networkList->pfds[0].fd=sockfd;
  s->networkList->pfds[0].events=POLLIN;

  while (1)
  {
      int poll_count=poll(s->networkList->pfds, s->curr_count, -1);
      if (poll_count==-1){
        perror("poll error");
        exit(1);
      }

      ProccessConnections(sockfd, s, poll_count);
  }
  close(sockfd);
  freeServer(s);

  return 0;
}
