#pragma once

#include <poll.h>


int addToPfds(struct pollfd** pfds, int newfd, int fd_count, int fd_size);
void delFromPfds(struct pollfd pollfds[], int i,int fd_count );



