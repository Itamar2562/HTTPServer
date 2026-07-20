#include "commsUtils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../PfdsUtils/pfdUtils.h"

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

int sendDataAll(int sockfd, char *data, size_t length)
{
  size_t offset=0 ;
  size_t bytesSend=0; // offset by how much already send
  size_t amountLeft=length;
  while(offset<length)
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
  return bytesSend;
}


void recvData(int sockfd, char *buffer,size_t length)
{ 

  int bytesrecv=recv(sockfd, buffer, length-1,0);
  buffer[bytesrecv]='\0';
  printf("recived %s\n",buffer);

}

char* getHTTPChunk(char **buffer,int *maxLength, int *currLength)
{ 
    int foundChunk=0;
    int chunkEnd=0;
    for (int i=0; i<*currLength ; i++)
    {
      if ((*buffer)[i]=='\r'&& strncmp(&(*buffer)[i],"\r\n\r\n",4)==0)
        {
          chunkEnd=i+4;
          foundChunk=1;
          break;
        }
    }
  if (*currLength+1>=*maxLength)
  {
    (*maxLength)*=2;
    char *temp=realloc(*buffer, *maxLength);
    if (temp!=NULL)
    {
       (*buffer) = temp;
    }
    else
      return NULL;  
  }
  char *header=NULL;
  
  if (foundChunk){
    int rest=*currLength - chunkEnd;
    header=(char *)malloc(chunkEnd+1);
    if (header==NULL)
      return NULL;
    strncpy(header, *buffer, chunkEnd);
    header[chunkEnd]='\0';
    memcpy(*buffer, (*buffer)+chunkEnd, rest);
    (*currLength)=rest;
    (*buffer)[rest]='\0';
  }
  return header;
}



int handleNewConnection(int listener , int fd_count, int *fd_size, struct pollfd **pollfd)
{
  struct sockaddr_storage clientAddr;
  socklen_t clientAddrLen;
  int clientFd;
  char clientIP[INET6_ADDRSTRLEN];

  clientAddrLen=sizeof(clientAddr);
  clientFd=accept(listener, (struct sockaddr*)&clientAddr, &clientAddrLen);
  if (clientFd==-1)
  {
      perror("accept error");
      return 0;
  }
  else
  {
    addToPfds(pollfd, clientFd, fd_count,fd_size);
    printf("new conenction from %s from socket %d\n", 
    getPresIpAddr((struct sockaddr*)&clientAddr,clientIP, sizeof(clientIP)),clientFd);
    return 1;
  }
}

