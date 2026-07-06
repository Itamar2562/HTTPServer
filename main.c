#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


#define MAXCLIENTS 2
#define SERVERPORT "4555"


void printAddressIPV4(struct sockaddr_in* addr)
{
  char buffer[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &addr->sin_addr, buffer, INET_ADDRSTRLEN );
  printf("the ip is %s\n",buffer);
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


int GetServerSocket()
{
  struct addrinfo hints, *res;
  int sockfd=-1;

  memset(&hints,0,sizeof hints); //clear the struct from garabge
  hints.ai_family= AF_INET; 
  hints.ai_socktype =SOCK_STREAM; //tcp
  hints.ai_flags=AI_PASSIVE;

  int status=getaddrinfo(NULL,SERVERPORT,&hints, &res);
  if (status!=0)
  {
    printf("error is %s\n", gai_strerror(status));
    return -1;
  }

  for (struct addrinfo * p=res; p!=NULL; p=p->ai_next )
  {
    if (p->ai_family!=hints.ai_family || p->ai_socktype != hints.ai_socktype)
      continue;
    
      sockfd=socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if (sockfd ==-1)
      {
        perror("server socket error: ");
        continue;
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
  return sockfd;
}

void * getCorrectSinAddress(struct sockaddr* genericAddr)
{
  if (genericAddr->sa_family==AF_INET){
    return &((struct sockaddr_in* )genericAddr)->sin_addr;
  }

  return &((struct sockaddr_in6* )genericAddr)->sin6_addr;
}




int main()
{
  int sockfd=GetServerSocket();
  if (sockfd==-1){
    exit(1);
  }
  
  int Listenstatus= listen(sockfd, MAXCLIENTS);
  if (Listenstatus==-1){
    perror("server listen error");
    exit(1);
  }
 
   printf("listening for incoming clients..\n");
  struct sockaddr_storage ClientAddr;
  socklen_t ClientAddrLen=sizeof(ClientAddr);

  while (1){
      int ClientSockfd = accept(sockfd, (struct sockaddr* )&ClientAddr, &ClientAddrLen);
      if (ClientSockfd==-1)
      {
        perror("Connect error");
        continue;
      }
      char buffer[INET6_ADDRSTRLEN];

      void* sin_addr=getCorrectSinAddress((struct sockaddr* )&ClientAddr);
      inet_ntop(ClientAddr.ss_family,sin_addr ,buffer,INET6_ADDRSTRLEN);

      printf("got connection from %s\n",buffer);
      
  }



  return 0;
}
