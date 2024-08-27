#ifndef SOCKET_H
#define SOCKET_H

#include "win_definition.h"
#include "address.h"

bool create_socket(SOCKET *_socket);
bool bind_socket(SOCKET *sock_fd, NetAddress *address);
bool listen_socket(SOCKET *_socket);
bool accept_socket(SOCKET *sock_fd_server, SOCKET *sock_fd_client);
void close_socket(SOCKET *_socket);

#endif // SOCKET_H