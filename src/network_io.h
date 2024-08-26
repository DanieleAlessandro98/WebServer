#ifndef NETWORK_IO_H
#define NETWORK_IO_H

#include "win_definition.h"

int recv_all(SOCKET *sock_fd, char *buf, int total_len);
int send_all(SOCKET *sock_fd, const char *buf, int total_len);

#endif // NETWORK_IO_H