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


#include "ContentLayer/contentUtils.h"



char* getHttpAction(char *header,char *buffer, size_t maxLength)
{
  size_t i=0;
  for (i=0; i<maxLength-1 && header[i]!=' '; i++)
    buffer[i]=header[i];
  buffer[i]='\0';
}

void routeHttpRequest(char *header)
{
  size_t maxLength=24;
  char action[maxLength];
  getHttpAction(header, action, maxLength);

  httpResponse *response = (httpResponse *) malloc( sizeof(httpResponse));
  initializeHttpResponse(response);

  if (strcmp(action,"GET")==0)
  {
    ;
  }
  else;
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

  char *header=getChunk(clientFd, &clients[*index].buffer, &clients[*index].chunkMaxLength, &clients[*index].chunkCurrLength);
  if (header==NULL)
  {
      close(clientFd);
      delFromPfds(pfds, *index, *curr_count);
      delFromClients(clients,*index, *curr_count );
      (*index)--; //delete swaps the last with curr so we need to check again this pos
      (*curr_count)--;
  }
  
  else
  {
    printf("pollserver: recv from fd %d: \n%s\n",clientFd, header);
    
      if (strncmp(header,"GET / HTTP/1.1",14)==0)
        {
          routeHttpRequest(header);
            // char * response=handleGet();
            // if (response==NULL)
            //   return;
            // int bytesSend=strlen(response);
            // printf("\nsending to clientFd: %d msg\n %s\n",clientFd,response);
            // sendDataAll(clientFd, response, &bytesSend);
      }
  }
    free(header);
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
