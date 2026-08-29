#include "NetworkList.h"
#include <stdlib.h>

void delFromNetworkList(NetworkList *networkList , int i, int count)
{
  delFromIPs(networkList->ips, i, count);
  delFromPfds(networkList->pfds , i ,count);
}

int initializeNetworkList(NetworkList *networkList , int max_size)
{
  networkList->pfds=(struct pollfd* )malloc(max_size* sizeof(struct pollfd));
  networkList->ips = (char **)malloc(max_size* sizeof(char *));

  if (networkList->ips == NULL || networkList->ips == NULL)
    {
      free(networkList->ips);
      free(networkList->pfds);
      return 0;
    }
  return 1;
}

void freeNetworkList(NetworkList *networkList , int count)
{
  if (networkList ==NULL)
    return;
  for (int i=0; i < count; i++)
  {
    free(networkList->ips[i]);
  }
  free(networkList->ips);
  free(networkList->pfds);
  free(networkList);
}
