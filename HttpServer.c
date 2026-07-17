#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "serverUtils.h"
#include "commsUtils.h"

#define MAX_CHUNK_LENGTH 256


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


int getChunk(int clientFd, char **buffer, char **dataLeft , int *maxLength)
{
  int gotChunk=0;
  int end=0;

  while(!gotChunk)
  { 
    int nbytes=recv(clientFd, *buffer,*maxLength-1,0);
    (*buffer)[nbytes]='\0';
    if (nbytes<=0) // got a problem
    {
      if (nbytes==0) // connection closed
      {
        printf("pollserver: socket %d hung up on us :(\n",clientFd);
        return 0;
      }
      else{
          perror("recv");
          return -1;
      }
    }

    gotChunk=recvHTTPChunk(buffer,dataLeft, maxLength, &end);
  }
  return 1;
}

void handleClientData(int listener, int *fd_count, struct pollfd *pfds, int *pfd_i, char **dataLeft )
{
  int chunkMaxLength=MAX_CHUNK_LENGTH;
  char *buffer=(char *)malloc(sizeof(char) *chunkMaxLength);


  int clientFd=pfds[*pfd_i].fd;

  int end=0;
  int gotChunk=0;

  int status=getChunk(clientFd, &buffer, dataLeft,&chunkMaxLength);
  if (status<=0)
  {
      close(clientFd);
      delFromPfds(pfds, *pfd_i, fd_count);
      (*pfd_i)--; //delete swaps the last with curr so we need to check again this pos
  }
  
  else
  {
    printf("pollserver: recv from fd %d: \n%s\n",clientFd, buffer);
    
    if (strncmp(buffer,"GET / HTTP/1.1",14)==0)
      {
        printf("NEW DATA\n\n");
          char * response=handleGet();
          int bytesSend=strlen(response);
          sendDataAll(clientFd, response, &bytesSend);
      }
  }
    free(buffer);
}

void ProccessConnections(int listener, int *fd_count, int *fd_size,struct pollfd **pfds, int poll_count, char **dataLeft){
  for (int i=0; i<*fd_count && poll_count>0;i++)
  {

    if ((*pfds)[i].revents & (POLLIN | POLLHUP)) // we got new data (smg to read or hang up)
    {
      if ((*pfds)[i].fd==listener) //the listener has smg to read (a new conn)
        handleNewConnection(listener, fd_count,fd_size,pfds);

      else
        handleClientData(listener, fd_count, *pfds,&i ,dataLeft);
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

  char *dataLeft=(char *)malloc(sizeof(char) *MAX_CHUNK_LENGTH);
  dataLeft[0]='\0';
  while (1)
  {
      int poll_count=poll(pfds, fd_size, -1);
      if (poll_count==-1){
        perror("poll error");
        exit(1);
      }

      ProccessConnections(sockfd, &fd_count,&fd_size, &pfds,poll_count,&dataLeft);
  }
  close(sockfd);
  free(pfds);
  free(dataLeft);

  return 0;
}
