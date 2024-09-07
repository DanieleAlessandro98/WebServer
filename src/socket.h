#ifndef SOCKET_H
#define SOCKET_H

#include "definitions.h"
#include "address.h"

bool create_socket(SOCKET *sock_fd);
bool bind_socket(SOCKET sock_fd, NetAddress *address);
bool listen_socket(SOCKET sock_fd);
bool accept_socket(SOCKET sock_fd_server, SOCKET *sock_fd_client);
void close_socket(SOCKET sock_fd);

#endif
