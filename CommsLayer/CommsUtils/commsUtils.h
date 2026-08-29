#pragma once

//#define _POSIX_C_SOURCE 200112L
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>

#include "../NetworkList/NetworkList.h"



void printAddressIPV4(struct sockaddr_in* addr);
const char* getPresIpAddr(struct sockaddr* genericAddr, char *buffer, size_t size);
void printAddresses(struct addrinfo* addresses);

int GetListenerSocket();
void sendData(int sockfd, char *data,size_t length);
int sendDataAll(int sockfd, char *data, size_t length);
int recvChunk(int clientFd, char *buffer,size_t *maxLength , size_t *currLength);

int handleNewConnection(int listener , int count, int size, NetworkList *networkList);

int addToIPs(char ***IPs, const char *IP,  int count, int size);


