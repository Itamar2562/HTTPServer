#include "IPList.h"
#include "stdlib.h"
#include <string.h>

int addToIPs(char ***IPs, const char *IP,  int count, int size)
{
  if (count>= size)
  {
    size*=2;
    char **temp=realloc((*IPs),sizeof(char *) * size );
    if (temp == NULL)
      return 0;
    else
      *IPs=temp;
  }
  (*IPs)[count] = (char *)malloc (strlen(IP) +1 );
  if ((*IPs)[count] == NULL)
    return 0;
  strcpy((*IPs)[count] , IP);
  return 1;
}


void delFromIPs(char **IPs, int i,int count )
{
  free(IPs[i]);
  IPs[i]=IPs[(count-1)];
}

