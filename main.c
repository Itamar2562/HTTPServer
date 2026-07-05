#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>


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


int main()
{

  struct addrinfo hints;
  struct addrinfo *res; //results

  memset(&hints,0,sizeof hints); //clear the struct from garabge
  hints.ai_family= AF_UNSPEC; 
  hints.ai_socktype =SOCK_STREAM; //tcp
  hints.ai_flags=AI_PASSIVE;

  char *port = "6380"; //can be number or name :0

  int status=getaddrinfo(NULL,port,&hints, &res);
  if (status!=0){

    printf("error is %s\n", gai_strerror(status));
    return 1;
  }
  int sockfd= socket(res->ai_family, res->ai_socktype , res->ai_protocol);

  bind (sockfd, res->ai_addr, res->ai_addrlen);

  printf("im on test");


 
  freeaddrinfo(res);

    return 0;
}