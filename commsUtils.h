#ifndef COMMS_UTILS_H
#define COMMS_UTILS_H

#define _POSIX_C_SOURCE 200112L
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>

void printAddressIPV4(struct sockaddr_in* addr);
const char* getPresIpAddr(struct sockaddr* genericAddr, char *buffer, size_t size);
void printAddresses(struct addrinfo* addresses);

int GetListenerSocket();
void sendData(int sockfd, char *data,size_t length);
int sendDataAll(int sockfd, char *data, int* length);
int  recvHTTPChunk(char **buffer,char **leftData, int *maxLength, int *end);

void addToPfds(struct pollfd** pfds, int newfd, int *fd_count, int *fd_size);
void delFromPfds(struct pollfd pollfds[], int i,int *fd_count );

void handleNewConnection(int listener , int *fd_count, int *fd_size, struct pollfd **pollfd);



#endif

