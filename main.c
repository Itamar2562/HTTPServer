#define _POSIX_C_SOURCE 200112L


#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
 #include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

// struct addrinfo {
//     int ai_flags; //AI_PASSIVE , AI_CANONNAME
//     int ai_family; //AF_INET :ipv4, AD_INET6 , AF_UNSPEC
//     int ai_socktype; //SOCK_STREAM:TCP , SOCK_DGRAM: UDP
//     int ai_protocol; // protocol 0 for any
//     size_t ai_addrlen; //size of ai_addr in bytes
//     struct sockaddr *ai_addr; //struct sockaddr
//     char * ai_canonname; //full hostname

//     struct addrinfo *ai_next; //linked list for the next node
// };

// struct sockaddr {
//     unsigned short sa_family; //address family, AF_XXX
//     char sa_data[14]; //14 bytes of protocol address (ip,port)
// };
//  //in for internet used for ipv4 to get ip and port more easily than a 14 byte array
//  // this is a used as a specific instance of sockaddr only for ipv4 which makes it easier to use
//  //as its not just 14 bytes but already split into port and ip.
// struct socketaddr_in {
//     short int sin_family; // address family always AF_INET (ipv4) socket internet
//     unsigned short int sin_port; //port number 
//     struct in_addr sin_addr; //internet address
//     unsigned char sin_zero[8]; //8 bytes padding of 0s 
//     //the 14 that is expected is more than ipv4 actually uses as its 2 for port and 4 for ip so we have and 8 bytes padding

// };

// struct in_addr{
//     uint32_t s_addr; //32 bit int (the diff is int might change base on host uint32_t does not)
// };


// VERY INTRESTING TO REMEMBER NETWORK ORDER is BIG ENDIAN and my pc is LITTLE ENDIAN
// so when getting bytes from the internet i gotta FLIP IT so my pc won't read it wrong.


int main()
{
//       struct socketaddr_in sa;

//       char *ip="172.12.4.4";
//       inet_pton(AF_INET, ip, &sa.sin_addr);

//       uint32_t num= ntohl(sa.sin_addr.s_addr);
//       printf("%u \n",num);






//   char ipv4[INET_ADDRSTRLEN]; // set place for ip

//   inet_ntop(AF_INET, &sa.sin_addr, ipv4 , INET_ADDRSTRLEN);
//   printf("The ipv4 address is: %s\n",ipv4);


  struct addrinfo hints;
  struct addrinfo *servinfo; //results

  memset(&hints,0,sizeof hints); //clear the struct from garabge
  hints.ai_family= AF_UNSPEC; //ipv4
  hints.ai_socktype =SOCK_STREAM; //tcp

  char *dns = "www.spotify.com";
  char *port = "443"; //can be number or name :0

  int status=getaddrinfo(dns,port,&hints, &servinfo);
  if (status!=0){

    printf("error is %s\n", gai_strerror(status));
    return 1;
  }
  for (struct addrinfo *p=servinfo; p!=NULL; p=p->ai_next)
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
  freeaddrinfo(servinfo);



// printf("family = %d\n", servinfo->ai_family);
// printf("socktype = %d\n", servinfo->ai_socktype);
// char ip_name[INET_ADDRSTRLEN];
// if (servinfo->ai_family==AF_INET) 
// // ai_addr actually points to a struct sockaddr_in
// // so cast the generic pointer to the correct type
// {
//     struct sockaddr_in *ipv4= (struct sockaddr_in *)servinfo->ai_addr;
//      //if its ipv4 we can treat the addr ptr as sockaddr_in because the general one is a wrapper pointing
//      //to memory built exactly the same.
//     inet_ntop(AF_INET,&ipv4->sin_addr,ip_name,INET_ADDRSTRLEN); // e
//     printf("the ip name is drum rol ...... %s\n ", ip_name);

// }

    return 0;
}