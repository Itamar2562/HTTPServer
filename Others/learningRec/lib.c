#include <string.h>
#include <stdlib.h>
#include <stdint.h>


struct addrinfo {
    int ai_flags; //AI_PASSIVE , AI_CANONNAME
    int ai_family; //AF_INET :ipv4, AD_INET6 , AF_UNSPEC
    int ai_socktype; //SOCK_STREAM:TCP , SOCK_DGRAM: UDP
    int ai_protocol; // protocol 0 for any
    size_t ai_addrlen; //size of ai_addr in bytes
    struct sockaddr *ai_addr; //struct sockaddr
    char * ai_canonname; //full hostname

    struct addrinfo *ai_next; //linked list for the next node
};

struct sockaddr {
    unsigned short sa_family; //address family, AF_XXX
    char sa_data[14]; //14 bytes of protocol address (ip,port)
};



 //in for internet used for ipv4 to get ip and port more easily than a 14 byte array
 // this is a used as a specific instance of sockaddr only for ipv4 which makes it easier to use
 //as its not just 14 bytes but already split into port and ip.
struct socketaddr_in {
    short int sin_family; // address family always AF_INET (ipv4) socket internet
    unsigned short int sin_port; //port number 
    struct in_addr sin_addr; //internet address
    unsigned char sin_zero[8]; //8 bytes padding of 0s 
    //the 14 that is expected is more than ipv4 actually uses as its 2 for port and 4 for ip so we have and 8 bytes padding

};
struct in_addr{
    uint32_t s_addr; //32 bit int (the diff is int might change base on host uint32_t does not)
};




//ipv6
struct sockaddr_in6
{
    uint16_t sin6_family; //address family alwats AF_INET6 (ipv6)
    uint16_t sin6_port; // port, in network byte order big endian
    uint32_t sin6_flowinfo; // ipv6 flow inforamtion
    struct in6_addr sin6_addr;

    uint32_t sin6_scope_id // SCOPE ID
};

struct in6_addr {
    unsigned char s6_addr[16];
};
