#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <poll.h>

#include "serverUtils.h"


#define MAX_CLIENTS 2
#define SERVER_PORT "4556"


void printAddressIPV4(struct sockaddr_in* addr)
{
  char buffer[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &addr->sin_addr, buffer, INET_ADDRSTRLEN );
  printf("the ip is %s\n",buffer);
}


const char* getPresIpAddr(struct sockaddr* genericAddr, char *buffer, size_t size)
{
  void * src;
  if (genericAddr->sa_family==AF_INET)
  {
    src= &((struct sockaddr_in* )genericAddr)->sin_addr;
  }

  else if (genericAddr->sa_family==AF_INET6){
      src= &((struct sockaddr_in6* )genericAddr)->sin6_addr;
  }
  else
    return NULL;

  return inet_ntop(genericAddr->sa_family, src, buffer, size);
  
}



void printAddresses(struct addrinfo* addresses){
  for (struct addrinfo *p=addresses; p!=NULL; p=p->ai_next)
  {

    struct sockaddr_in* ipv4;
    struct sockaddr_in6* ipv6;
    char * ipversion;

     void* addr;

     if (p->ai_family==AF_INET)// IPV4
     {
           ipv4=(struct sockaddr_in*)p->ai_addr;
           addr = &ipv4->sin_addr;
           ipversion="IPV4";
     }
     else if (p->ai_family==AF_INET6)//IPV6
     {
      ipv6=(struct sockaddr_in6* )p->ai_addr;
      addr=&ipv6->sin6_addr;
      ipversion="IPV6";

     }
     char buffer[INET6_ADDRSTRLEN];// WE USE SIZE OF IPV6 BECAUSE ITS BIGGER SO IT CAN FIT IPV4||IPV6
     inet_ntop(p->ai_family, addr, buffer, INET6_ADDRSTRLEN);
     
     printf("the ip of version %s is %s\n",ipversion,buffer);
  }
}

void getAddrInfo(struct addrinfo** res,int family, int sockType, int flag){
    struct addrinfo hints;
    memset(&hints,0, sizeof hints);
    hints.ai_family= family; 
    hints.ai_socktype =sockType; //tcp
    hints.ai_flags=flag;

    int status=getaddrinfo(NULL,SERVER_PORT,&hints, res);
    if (status!=0)
    {
        printf("error is %s\n", gai_strerror(status));
        exit(1);
    }
}

void gitMergeCheck(){
  printf("hello");
}


int GetListenerSocket()
{
  struct addrinfo *res;
  int sockfd=-1;
\
  getAddrInfo(&res, AF_UNSPEC, SOCK_STREAM, AI_PASSIVE); 


  for (struct addrinfo * p=res; p!=NULL; p=p->ai_next )
  {
      sockfd=socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if (sockfd ==-1)
      {
        perror("server socket error: ");
        continue;
      }
      //makes sure we can reuse port immediatly when we restart server
      int yes=1;
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&yes, sizeof(int))==-1)
      {
        close(sockfd);
        perror("setsockop error");
        return -1;
      }

      int bindStatus=bind(sockfd, p->ai_addr,p->ai_addrlen);
      if (bindStatus==-1){
        perror("server bind error");
        close(sockfd);
        continue;
      }

      break; //finished all steps
  }
  freeaddrinfo(res);
  int Listenstatus= listen(sockfd, MAX_CLIENTS);
  if (Listenstatus==-1){
    perror("server listen error");
    return -1;
    }
 
   printf("listening for incoming clients..\n");
  return sockfd;
}

void * getCorrectSinAddress(struct sockaddr* genericAddr)
{
  if (genericAddr->sa_family==AF_INET){
    return &((struct sockaddr_in* )genericAddr)->sin_addr;
  }

  return &((struct sockaddr_in6* )genericAddr)->sin6_addr;
}


void sendData(int sockfd, char *data,size_t length)
{
  int bytesSend=send(sockfd,data, length,0 );
  printf("send %d\n",bytesSend);
}

int sendDataAll(int sockfd, char *data, int* length)
{
  int offset=0 ;
  int bytesSend=0; // offset by how much already send
  int amountLeft=*length;
  while(offset<*length)
  {
    bytesSend=send(sockfd, data+offset,amountLeft,0);
    if (bytesSend==-1)
    {
      perror("send");
      break;
    }
    offset+=bytesSend;
    amountLeft-=bytesSend;
  }
  *length=offset;
  return bytesSend==-1 ? -1:0;
}


void recvData(int sockfd, char *buffer,size_t length)
{ 

  int bytesrecv=recv(sockfd, buffer, length-1,0);
  buffer[bytesrecv]='\0';
  printf("recived %s\n",buffer);

}

//function gets a ptr to a ptr which is a ptr to an array to 
//possible change the position of the array based on its size and count
void addToPfds(struct pollfd** pfds, int newfd, int *fd_count, int *fd_size)
{
  if (*fd_count== *fd_size)
  {
    (*fd_size)*=2;
    *pfds=realloc(*pfds,sizeof(**pfds) * (*fd_size) );
  }

  (*pfds)[*fd_count].fd=newfd;
  (*pfds)[*fd_count].events=POLLIN; // check if ready to read
  (*pfds)[*fd_count].revents =0; //reset

  (*fd_count)++;
}

void delFromPfds(struct pollfd pollfds[], int i,int *fd_count )
{
  pollfds[i]=pollfds[(*fd_count-1)];
  (*fd_count)--;
}



void handleNewConnection(int listener , int *fd_count, int *fd_size, struct pollfd **pollfd)
{
  struct sockaddr_storage clientAddr;
  socklen_t clientAddrLen;
  int clientFd;
  char clientIP[INET6_ADDRSTRLEN];

  clientAddrLen=sizeof(clientAddr);
  clientFd=accept(listener, (struct sockaddr*)&clientAddr, &clientAddrLen);
  if (clientFd==-1)
    perror("accept error");
  else
  {
    addToPfds(pollfd, clientFd,fd_count,fd_size);
    printf("new conenction from %s from socket %d\n", 
    getPresIpAddr((struct sockaddr*)&clientAddr,clientIP, sizeof(clientIP)),clientFd);
  }
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

void handleClientData(int listener, int *fd_count, struct pollfd *pfds, int *pfd_i )
{
  char buffer[256];

  int clientFd=pfds[*pfd_i].fd;
  int nbytes=recv(clientFd, buffer, sizeof(buffer), 0);

  if (nbytes<=0) // got a problem
  {
    if (nbytes==0) // connection closed
    {
      printf("pollserver: socket %d hung up on us :(\n",clientFd);
    }
    else
      perror("recv error");

    close(clientFd);
    delFromPfds(pfds, *pfd_i, fd_count);
    (*pfd_i)--; //delete swaps the last with curr so we need to check again this pos
  }
  else // got data
  {
    printf("pollserver: recv from fd %d: \n%.*s\n",clientFd, nbytes, buffer);
    if (strncmp(buffer,"GET / HTTP/1.1",14)==0)
    {
      printf("NEW DATA\n\n");
       char * response=handleGet();
        int bytesSend=strlen(response);
        sendDataAll(clientFd, response, &bytesSend);
    }
     
  }

}

void ProccessConnections(int listener, int *fd_count, int *fd_size,struct pollfd **pfds, int poll_count ){
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
