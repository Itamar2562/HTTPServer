#pragma once
#include "../PfdsUtils/pfdUtils.h"
#include "../IPList/IPList.h"


typedef struct
 {
    struct pollfd *pfds;
    char **ips;
} NetworkList;


void delFromNetworkList(NetworkList *networkList , int i, int count);
int initializeNetworkList(NetworkList *networkList , int max_size);
void freeNetworkList(NetworkList *networkList , int count);

