#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "serverUtils.h"
#include "CommsUtils/commsUtils.h"
#include "PfdsUtils/pfdUtils.h"

#define MAX_CHUNK_LENGTH 3


void route(char *buffer)
{
  if (strncmp(buffer,"GET / HTTP/1.1",14)==0);

}



char *handleGet()
{
  char *body=loadContent("index.html");
  char *header=buildHTTPHeaders(strlen(body),200);
  char *buffer=(char *)malloc(strlen(body)+strlen(header)+1);
  if (buffer==NULL){
    printf("complete response memory error");
    return NULL;
  }
  strcpy(buffer,header);
  strcat(buffer,body);
  free (body);
  free(header);
  printf("%s",buffer);
  return buffer;
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

    header=recvHTTPChunk(buffer,maxLength, currLength);
    if (header!=NULL)
      gotChunk=1;
  }
  return header;
}

void handleClientData(int listener, int *fd_count, struct pollfd *pfds, int *pfd_i)
{
  int chunkMaxLength=MAX_CHUNK_LENGTH;
  char *buffer=(char *)malloc(sizeof(char) *chunkMaxLength);


  int clientFd=pfds[*pfd_i].fd;

  int currLength=0;

  char *header=getChunk(clientFd, &buffer, &chunkMaxLength, &currLength);
  if (header==NULL)
  {
      close(clientFd);
      delFromPfds(pfds, *pfd_i, fd_count);
      (*pfd_i)--; //delete swaps the last with curr so we need to check again this pos
  }
  
  else
  {
    printf("pollserver: recv from fd %d: \n%s\n",clientFd, buffer);
    
    if (strncmp(header,"GET / HTTP/1.1",14)==0)
      {
        printf("NEW DATA\n\n");
          char * response=handleGet();
          int bytesSend=strlen(response);
          sendDataAll(clientFd, response, &bytesSend);
      }
  }
    free(buffer);
    free(header);
}

void ProccessConnections(int listener, int *fd_count, int *fd_size,struct pollfd **pfds, int poll_count){
  for (int i=0; i<*fd_count && poll_count>0;i++)
  {

    if ((*pfds)[i].revents & (POLLIN | POLLHUP)) // we got new data (smg to read or hang up)
    {
      if ((*pfds)[i].fd==listener) //the listener has smg to read (a new conn)
        handleNewConnection(listener, fd_count,fd_size,pfds);

      else
        handleClientData(listener, fd_count, *pfds,&i);
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

  int fd_size=5;
  int fd_count=1;

  struct pollfd * pfds=(struct pollfd* )malloc(fd_size* sizeof(struct pollfd));
  pfds[0].fd=sockfd;
  pfds[0].events=POLLIN;

  while (1)
  {
      int poll_count=poll(pfds, fd_size, -1);
      if (poll_count==-1){
        perror("poll error");
        exit(1);
      }

      ProccessConnections(sockfd, &fd_count,&fd_size, &pfds,poll_count);
  }
  close(sockfd);
  free(pfds);

  return 0;
}
