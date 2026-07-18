#include "pfdUtils.h"
#include <poll.h>
#include <stdlib.h>
//function gets a ptr to a ptr which is a ptr to an array to 
//possible change the position of the array based on its size and count
void addToPfds(struct pollfd** pfds, int newfd, int fd_count, int *fd_size)
{
  if (fd_count== *fd_size)
  {
    (*fd_size)*=2;
    *pfds=realloc(*pfds,sizeof(**pfds) * (*fd_size) );
  }

  (*pfds)[fd_count].fd=newfd;
  (*pfds)[fd_count].events=POLLIN; // check if ready to read
  (*pfds)[fd_count].revents =0; //reset

}

void delFromPfds(struct pollfd pollfds[], int i,int fd_count )
{
  pollfds[i]=pollfds[(fd_count-1)];
}

