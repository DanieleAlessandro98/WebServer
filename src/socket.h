#ifndef SOCKET_H
#define SOCKET_H

#include "win_definition.h"
#include "address.h"

int create_socket(SOCKET *_socket);
int bind_socket(SOCKET *sock_fd, NetAddress *address);
int listen_socket(SOCKET *_socket);
int accept_socket(SOCKET *sock_fd_server, SOCKET *sock_fd_client);
int close_socket(SOCKET *_socket);

#endif // SOCKET_H