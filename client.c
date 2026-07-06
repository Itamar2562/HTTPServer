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
#define SERVER_PORT "4555"


void * getCorrectSinAddress(struct sockaddr* genericAddr)
{
  if (genericAddr->sa_family==AF_INET){
    return &((struct sockaddr_in* )genericAddr)->sin_addr;
  }

  return &((struct sockaddr_in6* )genericAddr)->sin6_addr;
}




int getAddress(char * buffer,socklen_t length, struct sockaddr* addr)
{
  void *correctAddr=getCorrectSinAddress(addr);
  if ( inet_ntop(addr->sa_family, correctAddr, buffer, length )== NULL)
  {
    perror("inet_ntop error");
    return 0;
  }
  return 1;
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


void getAddrInfo(struct addrinfo** res,char * host,int family, int sockType, int flag){
    struct addrinfo hints;
    memset(&hints,0, sizeof hints);
    hints.ai_family= family; 
    hints.ai_socktype =sockType; //tcp
    hints.ai_flags=flag;

    int status=getaddrinfo(host,SERVER_PORT,&hints, res);
    if (status!=0)
    {
        printf("error is %s\n", gai_strerror(status));
        exit(1);
    }
}



int GetClientSocket(char *host )
{
  struct addrinfo *res;
  struct addrinfo *p=NULL;
  int sockfd=-1;

  getAddrInfo(&res,host,AF_UNSPEC,SOCK_STREAM, 0);
 

  for (p=res; p!=NULL; p=p->ai_next )
  {
      sockfd=socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if (sockfd ==-1)
      {
        perror("client socket error: ");
        continue;
      }
      char serverIP[INET6_ADDRSTRLEN];
      if (getAddress(serverIP,INET6_ADDRSTRLEN,p->ai_addr ))
        printf("attempting to connect to %s\n",serverIP);
      else
        printf("attempting to connect...\n");

      int connectStatus= connect(sockfd, p->ai_addr, p->ai_addrlen);
      if (connectStatus==-1)
      {
        perror("client connect error: ");
        close(sockfd);
        continue;
      }
      break; //finished all steps
  }
  if (p!=NULL)
  {
     char buffer[INET6_ADDRSTRLEN];
     void * addr=getCorrectSinAddress(p->ai_addr);
     inet_ntop(p->ai_family,  addr, buffer,INET6_ADDRSTRLEN);
     printf("connected to %s\n",buffer);
  }
 
  freeaddrinfo(res);
  return sockfd;
}





int main(int argc, char *argv[])
{
    int socketfd=GetClientSocket(argv[1]);

    close(socketfd);

      
  



  return 0;
}
